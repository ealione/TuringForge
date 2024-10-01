//#define CATCH_CONFIG_MAIN
//#include <catch2/catch_test_macros.hpp>
//#include <catch2/matchers/catch_matchers_vector.hpp>
//#include <catch2/matchers/catch_matchers.hpp>
//
//#include <Eigen/Dense>
//
//#include "turing_forge/core/dataset.hpp"
//#include "turing_forge/core/primitive_set.hpp"
//#include "turing_forge/operators/creator.hpp"
//#include "turing_forge/operators/initializer.hpp"
////#include "turing_forge/optimizer/optimizer.hpp"
//#include "turing_forge/core/individual.hpp"
//#include "turing_forge/core/problem.hpp"
////#include "turing_forge/operators/evaluator.hpp"
////#include "turing_forge/interpreter/dispatch_table.hpp"
//#include "turing_forge/algorithms/config.hpp"
//
//TEST_CASE( "Quick check", "[main]" ) {
//    // Variables
//    int minSize = 1;
//    int maxSize = 4;
//    int popSize = 150;
//    int nterms = 1;
//    int expolim = 3;
//    int generations = 50;
//    int selectedSize = 50;
//    double stopScore = 0.99;
//    bool shuffle = true;
//    bool standardize = true;
//
//    Turingforge::AlgorithmConfig config{};
//    config.Generations = 50;
//    config.PopulationSize = 150;
//    config.PoolSize = 50;
////    config.Evaluations = ;
////    config.Iterations = ;
////    config.CrossoverProbability = ;
////    config.MutationProbability = ;
////    config.TimeLimit = ;
//    config.Seed = std::random_device {}();
//
//    // Dataset
//    Turingforge::Range trainingRange;
//    Turingforge::Range testRange;
//    std::unique_ptr<Turingforge::Dataset> dataset;
//    dataset = std::make_unique<Turingforge::Dataset>("../../data/simple.csv", /*hasHeader=*/true);
//    Turingforge::FunctionType primitiveSetConfig = Turingforge::PrimitiveSet::Arithmetic | Turingforge::FunctionType::Log | Turingforge::FunctionType::Exp;
//    std::string targetName = "ΔP";
//
//    Turingforge::Variable target;
//    auto res = dataset->GetVariable(targetName);
//    target = *res;
////    auto const rows { dataset->Rows<std::size_t>() };
//
////    trainingRange = Turingforge::Range{ 0, 2 * rows / 3 }; // by default use 66% of the data as training
////
////    if (trainingRange.Start() > 0) {
////        testRange = Turingforge::Range{ 0, trainingRange.Start() };
////    } else if (trainingRange.End() < rows) {
////        testRange = Turingforge::Range{ trainingRange.End(), rows };
////    } else {
////        testRange = Turingforge::Range{ 0, 1};
////    }
////
////    // validate training range
////    REQUIRE(!(trainingRange.Start() >= rows || trainingRange.End() > rows));
////    REQUIRE(!(trainingRange.Start() > trainingRange.End()));
//
//    std::vector<Turingforge::Hash> inputs;
//    inputs = dataset->VariableHashes();
//    std::erase(inputs, target.Hash);
//
//    Turingforge::Problem problem(*dataset, trainingRange, testRange);
//    problem.SetTarget(target.Hash);
//    problem.SetInputs(inputs);
//    problem.ConfigurePrimitiveSet(primitiveSetConfig);
//
//    std::unique_ptr<Turingforge::BalancedCreator> creator;
//    creator = std::make_unique<Turingforge::BalancedCreator>(problem.GetPrimitiveSet(), problem.GetInputs(), /* bias= */ 0.0);
//
//    std::unique_ptr<Turingforge::RandomGenerator> generator;
//    generator = std::make_unique<Turingforge::RandomGenerator>(std::random_device{}());
//
//    Turingforge::UniformIndividualInitializer individualInitializer(*creator);
//    individualInitializer.SetExponentLimit(expolim);
//    individualInitializer.SetTerms(nterms);
//
//    std::unique_ptr<Turingforge::CoefficientInitializerBase> coeffInitializer;
//    std::unique_ptr<Turingforge::MutatorBase> onePoint;
//
////    Turingforge::DefaultDispatch dtable;
////    auto scale = true;
//
////    auto evaluator = std::make_unique<Turingforge::Evaluator<Turingforge::DefaultDispatch>>(problem, dtable, Turingforge::MSE{}, scale);
////    evaluator->SetBudget(config.Evaluations);
//
////    auto optimizer = std::make_unique<Turingforge::LevenbergMarquardtOptimizer<decltype(dtable), Turingforge::OptimizerType::Eigen>>(dtable, problem);
////    optimizer->SetIterations(config.Iterations);
//
//    Turingforge::RandomGenerator random(config.Seed);
////    if (shuffle) problem.GetDataset().Shuffle(random);
////    if (standardize) problem.StandardizeData(problem.TrainingRange());
////
////    auto t0 = std::chrono::steady_clock::now();
//
//    //Turingforge::SimulatedAnnealing gp { problem, config, treeInitializer, *coeffInitializer, *generator, *reinserter };
//
////    Turingforge::Individual best{};
//
//    // Initial Population
//    int rangeSize = maxSize - minSize + 1;
//    int numIndividualsPerExpSize = static_cast<int>(std::round(static_cast<double>(popSize) / rangeSize));
//
//    std::vector<Turingforge::Individual> pop(popSize);
//
//    Eigen::Ref<const Turingforge::Dataset::Matrix> dataMatrix = dataset->Values();
//    Eigen::Index targetIndex = dataMatrix.cols() - 1;
//    Eigen::Map<const Eigen::Matrix<Turingforge::Scalar, Eigen::Dynamic, 1>> ysHat(dataMatrix.col(targetIndex).data(), dataMatrix.rows());
//    Turingforge::Dataset::Matrix XsHat(dataMatrix.rows(), dataMatrix.cols() - 1);
//    XsHat << dataMatrix.leftCols(targetIndex), dataMatrix.rightCols(dataMatrix.cols() - targetIndex - 1);
//
//    for (int expSize = minSize; expSize <= maxSize; ++expSize) {
//        for (int i = 0; i < numIndividualsPerExpSize; ++i) {
//            auto newInd = (*creator)(*generator, expSize, expolim);
//
////            Turingforge::Range range{0, 1};
////            auto v1 = Turingforge::Interpreter<Turingforge::Scalar, Turingforge::DispatchTable<Turingforge::Scalar>>(dtable, problem.GetDataset(), newInd).Evaluate(newInd.Coefficients, Turingforge::Range(0, 1))[0];
//
//            newInd.sanitize();
//            newInd.fit(XsHat, ysHat);
//            pop.push_back(newInd);
//        }
//    }
//
//    auto best = *std::max_element(pop.begin(), pop.end(), [](const Turingforge::Individual& a, const Turingforge::Individual& b) {
//        return a.Fitness < b.Fitness;
//    });
//
//    for (int gen = 0; gen < generations; ++gen) {
//        // Selection
//        std::vector<Turingforge::Individual> parents;
//        parents.reserve(selectedSize);
//        for (size_t i = 0; i < selectedSize; ++i) {
//            const Turingforge::Individual& exp1 = *Turingforge::Random::Sample(random, pop.begin(), pop.end());
//            const Turingforge::Individual& exp2 = *Turingforge::Random::Sample(random, pop.begin(), pop.end());
//            const Turingforge::Individual& selected = (exp1.Fitness > exp2.Fitness) ? exp1 : exp2;
//            parents.emplace_back(Turingforge::Individual::copyIndividual(selected));
//        }
//
//        // Mutation
//        for (auto& parent : parents) {
//            Turingforge::Individual mutatedParent = Turingforge::Random::RandomProbability(random) > 0.5 ? parent.mutateOp() : parent.mutateTerms(nvars, expolim);
//            parent.sanitize();
//            parent.fit(XsHat, ysHat);
//        }
//
//        // Offspring
//
//        best = *std::max_element(pop.begin(), pop.end(), [](const Turingforge::Individual& a, const Turingforge::Individual& b) {
//            return a.Fitness < b.Fitness;
//        });
//    }
//
//    // Simplify and return best
//}