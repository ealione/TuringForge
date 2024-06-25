#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <catch2/matchers/catch_matchers.hpp>

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

TEST_CASE("Poisson Regression")
{
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
    constexpr auto pc{1.0};
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
    Turingforge::ReplaceInteractionMutation replaceSubtree { creator, coeffInitializer, maxLength };
    Turingforge::InsertInteractionMutation insertSubtree { creator, coeffInitializer, maxLength };
    Turingforge::RemoveInteractionMutation removeSubtree { problem.GetPrimitiveSet() };

    mutator.Add(onePoint, 1.0);
    mutator.Add(changeFunc, 1.0);
    mutator.Add(replaceSubtree, 1.0);
    mutator.Add(insertSubtree, 1.0);
    mutator.Add(removeSubtree, 1.0);

    constexpr auto maxEvaluations{1'000'000};
    constexpr auto maxGenerations{1'000};
}