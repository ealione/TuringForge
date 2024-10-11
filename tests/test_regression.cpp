#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>

#include "turing_forge/algorithms/nsga2.hpp"
#include "turing_forge/algorithms/gp.hpp"
#include "turing_forge/core/dataset.hpp"
#include "turing_forge/core/primitive_set.hpp"
#include "turing_forge/formatter/formatter.hpp"
#include "turing_forge/operators/creator.hpp"
#include "turing_forge/operators/selector.hpp"
#include "turing_forge/random/random.hpp"
#include "turing_forge/operators/initializer.hpp"
#include "turing_forge/operators/evaluator.hpp"
#include "turing_forge/operators/reinserter.hpp"
#include "turing_forge/optimizer/optimizer.hpp"
#include "turing_forge/operators/non_dominated_sorter.hpp"
#include "turing_forge/operators/mutation.hpp"
#include "turing_forge/operators/crossover.hpp"
#include "turing_forge/operators/generator.hpp"
#include "turing_forge/algorithms/config.hpp"

TEST_CASE("Poisson Regression") {
    constexpr auto nrows = 30;
    constexpr auto ncols = 2;

    Turingforge::RandomGenerator rng{1234};
    using Uniform = std::uniform_real_distribution<Turingforge::Scalar>;
    using Normal  = std::normal_distribution<Turingforge::Scalar>;
    using Poisson = std::poisson_distribution<>;

    Eigen::Array<Turingforge::Scalar, nrows, ncols> data;
    auto x = data.col(0);
    auto y = data.col(1);

    std::generate_n(x.data(), nrows, [&](){ return Uniform(0.1, 5)(rng); });
    Eigen::Array<Turingforge::Scalar, nrows, 1> lam = 2 * x.square();

    std::transform(lam.begin(), lam.end(), y.data(), [&](auto v) { return Poisson(v)(rng); });

    Turingforge::Dataset ds{data};
    Turingforge::Range rg(0, ds.Rows());

    Turingforge::Problem problem{ds, {0UL, ds.Rows<std::size_t>()}, {0UL, 1UL}};
    problem.ConfigurePrimitiveSet(Turingforge::PrimitiveSet::Unary);

    // parameters
    constexpr auto pc{.0};
    constexpr auto pm{0.25};

    constexpr auto maxLength{30UL};

    Turingforge::BalancedCreator creator{problem.GetPrimitiveSet(), problem.GetInputs()};
    auto [minArity, maxArity] = problem.GetPrimitiveSet().FunctionArityLimits();

    Turingforge::UniformIndividualInitializer individualInitializer(creator);
    individualInitializer.ParameterizeDistribution(minArity + 1, maxLength);
    individualInitializer.SetObjectivesCount(1);

    Turingforge::CoefficientInitializer<Uniform> coeffInitializer;
    coeffInitializer.ParameterizeDistribution(-5.F, +5.F);

    Turingforge::IndividualCrossover crossover{pc, maxLength};
    Turingforge::MultiMutation mutator{};

    Turingforge::OnePointMutation<Normal> onePoint{};
    onePoint.ParameterizeDistribution(0.F, 1.F);
    Turingforge::ChangeFunctionMutation changeFunc { problem.GetPrimitiveSet() };
    Turingforge::ReplaceInteractionMutation replaceInteraction {creator, coeffInitializer, maxLength };
    Turingforge::InsertInteractionMutation insertInteraction {creator, coeffInitializer, maxLength };
    Turingforge::RemoveInteractionMutation removeInteraction {problem.GetPrimitiveSet() };

    mutator.Add(onePoint, 1.0);
    mutator.Add(changeFunc, 1.0);
    mutator.Add(replaceInteraction, 1.0);
    mutator.Add(insertInteraction, 1.0);
    mutator.Add(removeInteraction, 1.0);

    constexpr auto maxEvaluations{1'000'000};
    constexpr auto maxGenerations{1'000};

    Turingforge::LengthEvaluator lengthEvaluator{problem, maxLength};

    Turingforge::DefaultDispatch dt;
    using Likelihood = Turingforge::PoissonLikelihood<Turingforge::Scalar, /*LogInput*/ true>;
    Turingforge::LikelihoodEvaluator<decltype(dt), Likelihood> poissonEvaluator{problem, dt};
    poissonEvaluator.SetBudget(maxEvaluations);

    Turingforge::MultiEvaluator evaluator{problem};
    evaluator.SetBudget(maxEvaluations);
    evaluator.Add(poissonEvaluator);
    evaluator.Add(lengthEvaluator);

    // Turingforge::LevenbergMarquardtOptimizer<decltype(dt)> optimizer{dt, problem};
    Turingforge::SGDOptimizer<decltype(dt), Likelihood> optimizer{dt, problem};
    // Turingforge::LBFGSOptimizer<decltype(dt), Turingforge::PoissonLikelihood<>> optimizer{dt, problem};
    optimizer.SetIterations(100);

    Turingforge::LexicographicalComparison lc;
    Turingforge::TournamentSelector selector{lc};
    Turingforge::CoefficientOptimizer co{optimizer};

    Turingforge::BasicOffspringGenerator gen{evaluator, crossover, mutator, selector, selector, &co};
    Turingforge::RankIntersectSorter rankSorter;
    Turingforge::KeepBestReinserter reinserter{lc};

    Turingforge::AlgorithmConfig config{};
    config.Generations = maxGenerations;
    config.Evaluations = maxEvaluations;
    config.PopulationSize = 10;
    config.PoolSize = 10;
    config.CrossoverProbability = pc;
    config.MutationProbability = pm;
    config.Seed = 1234;
    config.TimeLimit = std::numeric_limits<size_t>::max();

    Turingforge::NSGA2 algorithm{problem, config, individualInitializer, coeffInitializer, gen, reinserter, rankSorter};
    auto report = [&](){
        fmt::print("{} {}\n", algorithm.Generation(), std::size_t{poissonEvaluator.CallCount});
    };

    algorithm.Run(rng, report);
    fmt::print("{}\n", poissonEvaluator.TotalEvaluations());

    // validate results
     Turingforge::AkaikeInformationCriterionEvaluator<decltype(dt)> aicEvaluator{problem, dt};

     for (auto ind : algorithm.Best()) {
         auto a = poissonEvaluator(rng, ind, {});

         Turingforge::Interpreter<Turingforge::Scalar, decltype(dt)> interpreter(dt, ds, ind);
         auto est = interpreter.Evaluate(problem.TrainingRange());
         auto tgt = problem.TargetValues(problem.TrainingRange());

         auto b = Likelihood::ComputeLikelihood(est, tgt, poissonEvaluator.Sigma());
         auto c = aicEvaluator(rng, ind, {});

         fmt::print("{}: {} {} {}\n", Turingforge::IndividualFormatter::Format(ind, ds), a, b, c);
     }
}