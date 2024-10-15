#include "turing_forge/optimizer/likelihood/gaussian_likelihood.hpp"
#include "turing_forge/optimizer/solvers/sgd.hpp"
#include <chrono>
#include <cmath>
#include <cstdlib>

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <memory>
#include <thread>

#include <taskflow/taskflow.hpp>
#if TF_MINOR_VERSION > 2
#include <taskflow/algorithm/reduce.hpp>
#endif

#include "turing_forge/algorithms/nsga2.hpp"
#include "turing_forge/core/version.hpp"
#include "turing_forge/core/problem.hpp"
#include "turing_forge/formatter/formatter.hpp"
#include "turing_forge/interpreter/interpreter.hpp"
#include "turing_forge/operators/creator.hpp"
#include "turing_forge/operators/crossover.hpp"
#include "turing_forge/operators/evaluator.hpp"
#include "turing_forge/operators/generator.hpp"
#include "turing_forge/operators/initializer.hpp"
#include "turing_forge/operators/mutation.hpp"
#include "turing_forge/operators/non_dominated_sorter.hpp"
#include "turing_forge/operators/reinserter.hpp"
#include "turing_forge/operators/selector.hpp"
#include "turing_forge/optimizer/optimizer.hpp"

#include "util.hpp"
#include "operator_factory.hpp"

auto main(int argc, char** argv) -> int
{
    auto opts = Turingforge::InitOptions("turingforge_gp", "Genetic programming symbolic regression");
    auto result = Turingforge::ParseOptions(std::move(opts), argc, argv);

    // parse and set default values
    Turingforge::AlgorithmConfig config{};
    config.Generations = result["generations"].as<size_t>();
    config.PopulationSize = result["population-size"].as<size_t>();
    config.PoolSize = result["pool-size"].as<size_t>();
    config.Epsilon = result["epsilon"].as<Turingforge::Scalar>();
    config.Evaluations = result["evaluations"].as<size_t>();
    config.Iterations = result["iterations"].as<size_t>();
    config.CrossoverProbability = result["crossover-probability"].as<Turingforge::Scalar>();
    config.MutationProbability = result["mutation-probability"].as<Turingforge::Scalar>();
    config.LocalSearchProbability = result["local-search-probability"].as<Turingforge::Scalar>();
    config.LamarckianProbability = result["lamarckian-probability"].as<Turingforge::Scalar>();
    config.TimeLimit = result["timelimit"].as<size_t>();
    config.Seed = std::random_device {}();

    // parse remaining config options
    Turingforge::Range trainingRange;
    Turingforge::Range testRange;
    std::unique_ptr<Turingforge::Dataset> dataset;
    std::string targetName;
    bool showPrimitiveSet = false;
    auto threads = std::thread::hardware_concurrency();
    Turingforge::FunctionType primitiveSetConfig = Turingforge::PrimitiveSet::Arithmetic;

    auto maxLength = result["maxlength"].as<size_t>();
    auto maxDepth = result["maxdepth"].as<size_t>();
    auto crossoverInternalProbability = result["crossover-internal-probability"].as<Turingforge::Scalar>();

    auto symbolic = result["symbolic"].as<bool>();

    try {
        for (const auto& kv : result.arguments()) {
            const auto& key = kv.key();
            const auto& value = kv.value();

            if (key == "dataset") {
                dataset = std::make_unique<Turingforge::Dataset>(value, true);
                        ENSURE(!dataset->IsView());
            }
            if (key == "seed") {
                config.Seed = kv.as<size_t>();
            }
            if (key == "train") {
                trainingRange = Turingforge::ParseRange(value);
            }
            if (key == "test") {
                testRange = Turingforge::ParseRange(value);
            }
            if (key == "target") {
                targetName = value;
            }
            if (key == "maxlength") {
                maxLength = kv.as<size_t>();
            }
            if (key == "maxdepth") {
                maxDepth = kv.as<size_t>();
            }
            if (key == "enable-symbols") {
                auto mask = Turingforge::ParsePrimitiveSetConfig(value);
                primitiveSetConfig |= mask;
            }
            if (key == "disable-symbols") {
                auto mask = ~Turingforge::ParsePrimitiveSetConfig(value);
                primitiveSetConfig &= mask;
            }
            if (key == "threads") {
                threads = static_cast<decltype(threads)>(kv.as<size_t>());
            }
            if (key == "show-primitives") {
                showPrimitiveSet = true;
            }
        }

        if (showPrimitiveSet) {
            Turingforge::PrintPrimitives(primitiveSetConfig);
            return EXIT_SUCCESS;
        }

        // set the target
        Turingforge::Variable target;
        auto res = dataset->GetVariable(targetName);
        if (!res) {
            fmt::print(stderr, "error: target variable {} does not exist in the dataset.", targetName);
            return EXIT_FAILURE;
        }
        target = *res;
        auto const rows{ dataset->Rows<std::size_t>() };
        if (result.count("train") == 0) {
            trainingRange = Turingforge::Range{ 0, 2 * rows / 3 }; // by default use 66% of the data as training
        }
        if (result.count("test") == 0) {
            // if no test range is specified, we try to infer a reasonable range based on the trainingRange
            if (trainingRange.Start() > 0) {
                testRange = Turingforge::Range{ 0, trainingRange.Start() };
            } else if (trainingRange.End() < rows) {
                testRange = Turingforge::Range{ trainingRange.End(), dataset->Rows<std::size_t>() };
            } else {
                testRange = Turingforge::Range{ 0, 1};
            }
        }
        // validate training range
        if (trainingRange.Start() >= rows || trainingRange.End() > rows) {
            fmt::print(stderr, "error: the training range {}:{} exceeds the available data range ({} rows)\n", trainingRange.Start(), trainingRange.End(), dataset->Rows());
            return EXIT_FAILURE;
        }

        if (trainingRange.Start() > trainingRange.End()) {
            fmt::print(stderr, "error: invalid training range {}:{}\n", trainingRange.Start(), trainingRange.End());
            return EXIT_FAILURE;
        }

        std::vector<Turingforge::Hash> inputs;
        if (result.count("inputs") == 0) {
            inputs = dataset->VariableHashes();
            std::erase(inputs, target.Hash);
        } else {
            auto str = result["inputs"].as<std::string>();
            auto tokens = Turingforge::Split(str, ',');

            for (auto const& tok : tokens) {
                if (auto res = dataset->GetVariable(tok); res.has_value()) {
                    inputs.push_back(res->Hash);
                } else {
                    fmt::print(stderr, "error: variable {} does not exist in the dataset.", tok);
                    return EXIT_FAILURE;
                }
            }
        }
        Turingforge::Problem problem(*dataset, trainingRange, testRange);
        problem.SetTarget(target.Hash);
        problem.SetInputs(inputs);
        problem.ConfigurePrimitiveSet(primitiveSetConfig);

        std::unique_ptr<Turingforge::CreatorBase> creator;
        creator = ParseCreator(result["creator"].as<std::string>(), problem.GetPrimitiveSet(), problem.GetInputs());

        auto [amin, amax] = problem.GetPrimitiveSet().FunctionArityLimits();
        Turingforge::UniformIndividualInitializer individualInitializer(*creator);

        auto const initialMinDepth = result["creator-mindepth"].as<std::size_t>();
        auto const initialMaxDepth = result["creator-mindepth"].as<std::size_t>();
        auto const initialMaxLength = result["creator-maxlength"].as<std::size_t>();
        individualInitializer.ParameterizeDistribution(amin+1, initialMaxLength);

        std::unique_ptr<Turingforge::CoefficientInitializerBase> coeffInitializer;
        std::unique_ptr<Turingforge::MutatorBase> onePoint;
        if (symbolic) {
            using Dist = std::uniform_int_distribution<int>;
            coeffInitializer = std::make_unique<Turingforge::CoefficientInitializer<Dist>>();
            int constexpr range{5};
            dynamic_cast<Turingforge::CoefficientInitializer<Dist>*>(coeffInitializer.get())->ParameterizeDistribution(-range, +range);
            onePoint = std::make_unique<Turingforge::OnePointMutation<Dist>>();
            dynamic_cast<Turingforge::OnePointMutation<Dist>*>(onePoint.get())->ParameterizeDistribution(-range, +range);
        } else {
            using Dist = std::normal_distribution<Turingforge::Scalar>;
            coeffInitializer = std::make_unique<Turingforge::CoefficientInitializer<Dist>>();
            dynamic_cast<Turingforge::NormalCoefficientInitializer*>(coeffInitializer.get())->ParameterizeDistribution(Turingforge::Scalar{0}, Turingforge::Scalar{1});
            onePoint = std::make_unique<Turingforge::OnePointMutation<Dist>>();
            dynamic_cast<Turingforge::OnePointMutation<Dist>*>(onePoint.get())->ParameterizeDistribution(Turingforge::Scalar{0}, Turingforge::Scalar{1});
        }

        Turingforge::IndividualCrossover crossover{ crossoverInternalProbability, maxLength };
        Turingforge::MultiMutation mutator{};

        Turingforge::ChangeFunctionMutation changeFunc { problem.GetPrimitiveSet() };
        Turingforge::ReplaceInteractionMutation replaceInteraction { *creator, *coeffInitializer, maxLength };
        Turingforge::InsertInteractionMutation insertInteraction { *creator, *coeffInitializer, maxLength };
        Turingforge::RemoveInteractionMutation removeInteraction { problem.GetPrimitiveSet() };
        Turingforge::DiscretePointMutation discretePoint;
        for (auto v : Turingforge::Math::Constants) {
            discretePoint.Add(static_cast<Turingforge::Scalar>(v), 1);
        }
        mutator.Add(*onePoint, 1.0);
        mutator.Add(changeFunc, 1.0);
        mutator.Add(replaceInteraction, 1.0);
        mutator.Add(insertInteraction, 1.0);
        mutator.Add(removeInteraction, 1.0);
        mutator.Add(discretePoint, 1.0);

        Turingforge::DefaultDispatch dtable;
        auto scale = result["linear-scaling"].as<bool>();
        auto errorEvaluator = Turingforge::ParseEvaluator(result["objective"].as<std::string>(), problem, dtable, scale);
        errorEvaluator->SetBudget(config.Evaluations);

        auto optimizer = std::make_unique<Turingforge::LevenbergMarquardtOptimizer<decltype(dtable), Turingforge::OptimizerType::Eigen>>(dtable, problem);
        optimizer->SetIterations(config.Iterations);
        Turingforge::LengthEvaluator lengthEvaluator(problem, maxLength);

        Turingforge::MultiEvaluator evaluator(problem);
        evaluator.SetBudget(config.Evaluations);
        evaluator.Add(*errorEvaluator);
        evaluator.Add(lengthEvaluator);

        EXPECT(problem.TrainingRange().Size() > 0);

        Turingforge::CrowdedComparison comp;

        auto femaleSelector = Turingforge::ParseSelector(result["female-selector"].as<std::string>(), comp);
        auto maleSelector = Turingforge::ParseSelector(result["male-selector"].as<std::string>(), comp);
        Turingforge::CoefficientOptimizer cOpt{*optimizer, config.LamarckianProbability};

        auto generator = Turingforge::ParseGenerator(result["offspring-generator"].as<std::string>(), evaluator, crossover, mutator, *femaleSelector, *maleSelector, &cOpt);
        auto reinserter = Turingforge::ParseReinserter(result["reinserter"].as<std::string>(), comp);

        Turingforge::RandomGenerator random(config.Seed);
        if (result["shuffle"].as<bool>()) {
            problem.GetDataset().Shuffle(random);
        }
        if (result["standardize"].as<bool>()) {
            problem.StandardizeData(problem.TrainingRange());
        }

        tf::Executor executor(threads);

        auto t0 = std::chrono::steady_clock::now();
        Turingforge::RankIntersectSorter sorter;
        Turingforge::NSGA2 gp { problem, config, individualInitializer, *coeffInitializer, *generator, *reinserter, sorter };

        auto targetValues = problem.TargetValues();
        auto targetTrain = targetValues.subspan(trainingRange.Start(), trainingRange.Size());
        auto targetTest = targetValues.subspan(testRange.Start(), testRange.Size());

        // some boilerplate for reporting results
        const size_t idx { 0 };
        auto getBest = [&](Turingforge::Span<Turingforge::Individual const> pop) -> Turingforge::Individual {
            const auto minElem = std::min_element(pop.begin(), pop.end(), [&](auto const& lhs, auto const& rhs) { return lhs[idx] < rhs[idx]; });
            return *minElem;
        };

        Turingforge::Individual best;
        auto getSize = [](Turingforge::Individual const& ind) { return sizeof(ind) + sizeof(ind) + sizeof(Turingforge::Function); };

        auto report = [&]() {
            auto const& pop = gp.Parents();
            auto const& off = gp.Offspring();

            best = getBest(pop);

            Turingforge::Vector<Turingforge::Scalar> estimatedTrain;
            Turingforge::Vector<Turingforge::Scalar> estimatedTest;

            tf::Taskflow taskflow;

            using DT = Turingforge::DefaultDispatch;

            auto evalTrain = taskflow.emplace([&]() {
                estimatedTrain = Turingforge::Interpreter<Turingforge::Scalar, DT>::Evaluate(best, problem.GetDataset(), trainingRange);
            });

            auto evalTest = taskflow.emplace([&]() {
                estimatedTest = Turingforge::Interpreter<Turingforge::Scalar, DT>::Evaluate(best, problem.GetDataset(), testRange);
            });

            // scale values
            Turingforge::Scalar a{1.0};
            Turingforge::Scalar b{0.0};
            auto linearScaling = taskflow.emplace([&]() {
                auto [a_, b_] = Turingforge::FitLeastSquares(estimatedTrain, targetTrain);
                a = static_cast<Turingforge::Scalar>(a_);
                b = static_cast<Turingforge::Scalar>(b_);
                // add scaling terms to the individual
                auto& individual = best;
                // Scale all coefficients by 'a'
                for (auto& coeff : individual.Coefficients) {
                    coeff *= a;
                }
            });

            double r2Train{};
            double r2Test{};
            double nmseTrain{};
            double nmseTest{};
            double maeTrain{};
            double maeTest{};

            auto scaleTrain = taskflow.emplace([&]() {
                Eigen::Map<Eigen::Array<Turingforge::Scalar, -1, 1>> estimated(estimatedTrain.data(), std::ssize(estimatedTrain));
                estimated = estimated * a + b;
            });

            auto scaleTest = taskflow.emplace([&]() {
                Eigen::Map<Eigen::Array<Turingforge::Scalar, -1, 1>> estimated(estimatedTest.data(), std::ssize(estimatedTest));
                estimated = estimated * a + b;
            });

            auto calcStats = taskflow.emplace([&]() {
                // negate the R2 because this is an internal fitness measure (minimization) which we here repurpose
                r2Train = -Turingforge::R2{}(estimatedTrain, targetTrain);
                r2Test = -Turingforge::R2{}(estimatedTest, targetTest);

                nmseTrain = Turingforge::NMSE{}(estimatedTrain, targetTrain);
                nmseTest = Turingforge::NMSE{}(estimatedTest, targetTest);

                maeTrain = Turingforge::MAE{}(estimatedTrain, targetTrain);
                maeTest = Turingforge::MAE{}(estimatedTest, targetTest);
            });

            double avgLength = 0;
            double avgQuality = 0;
            double totalMemory = 0;

            auto calculateLength = taskflow.transform_reduce(pop.begin(), pop.end(), avgLength, std::plus{}, [](auto const& ind) { return ind.Length; });
            auto calculateQuality = taskflow.transform_reduce(pop.begin(), pop.end(), avgQuality, std::plus{}, [idx=idx](auto const& ind) { return ind[idx]; });
            auto calculatePopMemory = taskflow.transform_reduce(pop.begin(), pop.end(), totalMemory, std::plus{}, [&](auto const& ind) { return getSize(ind); });
            auto calculateOffMemory = taskflow.transform_reduce(off.begin(), off.end(), totalMemory, std::plus{}, [&](auto const& ind) { return getSize(ind); });

            // define task graph
            linearScaling.succeed(evalTrain, evalTest);
            linearScaling.precede(scaleTrain, scaleTest);
            calcStats.succeed(scaleTrain, scaleTest);
            calcStats.precede(calculateLength, calculateQuality, calculatePopMemory, calculateOffMemory);

            executor.corun(taskflow);

            avgLength /= static_cast<double>(pop.size());
            avgQuality /= static_cast<double>(pop.size());

            auto t1 = std::chrono::steady_clock::now();
            auto elapsed = static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count()) / 1e6;

            using T = std::tuple<std::string, double, std::string>;
            auto const* format = ":>#8.3g"; // see https://fmt.dev/latest/syntax.html

            auto [resEval, jacEval, callCount, cfTime ] = evaluator.Stats();
            std::array stats {
                    T{ "iteration", gp.Generation(), ":>" },
                    T{ "r2_tr", r2Train, format },
                    T{ "r2_te", r2Test, format },
                    T{ "mae_tr", maeTrain, format },
                    T{ "mae_te", maeTest, format },
                    T{ "nmse_tr", nmseTrain, format },
                    T{ "nmse_te", nmseTest, format },
                    T{ "avg_fit", avgQuality, format },
                    T{ "avg_len", avgLength, format },
                    T{ "eval_cnt", callCount, ":>" },
                    T{ "res_eval", resEval, ":>" },
                    T{ "jac_eval", jacEval, ":>" },
                    T{ "opt_time", cfTime, ":>" },
                    T{ "seed", config.Seed, ":>" },
                    T{ "elapsed", elapsed, ":>"},
            };
            Turingforge::PrintStats({ stats.begin(), stats.end() }, gp.Generation() == 0);
        };

        gp.Run(executor, random, report);
        fmt::print("{}\n", Turingforge::IndividualFormatter::Format(best, problem.GetDataset(), std::numeric_limits<Turingforge::Scalar>::digits));
    } catch (std::exception& e) {
        fmt::print(stderr, "error: {}\n", e.what());
        return EXIT_FAILURE;
    }

    return 0;
}