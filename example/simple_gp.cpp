#include "turing_forge/core/dataset.hpp"
#include "turing_forge/core/problem.hpp"
#include "turing_forge/operators/mutation.hpp"
#include "turing_forge/operators/crossover.hpp"
#include "turing_forge/operators/selector.hpp"
#include "turing_forge/operators/evaluator.hpp"
#include "turing_forge/operators/reinserter.hpp"
#include "turing_forge/operators/generator.hpp"
#include "turing_forge/operators/creator.hpp"
#include "turing_forge/operators/initializer.hpp"
#include "turing_forge/algorithms/config.hpp"
#include "turing_forge/algorithms/gp.hpp"

/*
 * This example shows how to do symbolic regression using GP.
 * We solve a synthetic benchmark problem, namely the `Poly-10`:
 *      F(x) - x1x2 + x3x4 + x5x6 + x1x7x9 + x3x6x10
 */
auto main() -> int
{
    // This problem consists of 500 datapoints which we’ll split equally between our training and test data.
    // By convention, the program will use all the dataset columns (except for the target column) as features.
    // The user is responsible for preprocessing the data prior to the modeling step.
    Turingforge::Dataset ds("../../data/Poly-10.csv", /* csv has header */ true);
    Turingforge::Range trainingRange { 0, static_cast<size_t>(ds.Rows() / 2) };
    Turingforge::Range testRange     { static_cast<size_t>(ds.Rows() / 2), static_cast<size_t>(ds.Rows()) };

    // Next, we define the optimization target and create a `Problem`. We configure the problem to use an arithmetic
    // grammar, consisting of unary functions. The Grammar class keeps track of the allowed symbols and their
    // initial frequencies (taken into account when the population is initialized).
    const std::string target = "Y";
    Turingforge::Problem problem(ds, trainingRange, testRange);
    problem.SetTarget(target);
    problem.ConfigurePrimitiveSet(Turingforge::PrimitiveSet::Unary);

    // set up crossover and mutation
    using Dist = std::uniform_int_distribution<int>;
    double internalNodeBias = 0.9;
    size_t maxIndividualLength = 50;
    Turingforge::IndividualCrossover crossover { internalNodeBias, maxIndividualLength };
    Turingforge::MultiMutation mutation;
    Turingforge::OnePointMutation<Dist> onePoint;
    Turingforge::ChangeFunctionMutation changeFunc { problem.GetPrimitiveSet() };
    mutation.Add(onePoint, 1.0);
    mutation.Add(changeFunc, 1.0);

    // The selection operator samples the distribution of fitness values in the population and picks parent individuals
    // for taking part in recombination. Turingforge supports specifying different selection methods for the two parents
    // (typically called male and female or root and non-root parents). We tell the selector how to compare individuals
    // by providing a lambda function to its constructor:

    // our lambda function simply compares the fitness of the individuals
    auto comp = [](Turingforge::Individual const& lhs, Turingforge::Individual const& rhs) {
        return lhs[0] < rhs[0];
    };
    // set up the selector
    Turingforge::TournamentSelector selector(comp);
    selector.SetTournamentSize(5);

    // The evaluator operator is responsible for calculating fitness and is allotted a fixed evaluation budget at the
    // beginning of the run. The evaluator is also capable of performing nonlinear least-squares fitting of model
    // parameters if the local optimization iterations parameter is set to a value greater than zero.

    // set up the evaluator
    constexpr auto maxEvaluations{1'000'000};
    Turingforge::DefaultDispatch dt;
    Turingforge::Evaluator<Turingforge::DefaultDispatch> evaluator{problem, dt, Turingforge::RMSE{}, true};
    evaluator.SetBudget(maxEvaluations);

    // The reinsertion operator merges the pool of recombinants (new offspring) back into the population. This can be a
    // simple replacement or a more sophisticated strategy (eg., keep the best individuals among the parents and
    // offspring). Like the selector, the reinserter requires a lambda to specify how it should compare individuals.
    Turingforge::ReplaceWorstReinserter reinserter{comp};

    // Offspring generator implements a strategy for producing new offspring. This can be plain recombination
    // (eg., crossover + mutation) or more elaborate logic like acceptance criteria for offspring or brood selection.
    // In general, this operation may fail (returning a maybe type) and should be handled by the algorithm designer.

    // the generator makes use of the other operators to generate offspring and assign fitness the selector is passed
    // twice, once for the male parent, once for the female parent.
    Turingforge::BasicOffspringGenerator generator(evaluator, crossover, mutation, selector, selector);

    // The creator initializes random `individuals` of any target length.
    // The length is sampled from a uniform distribution `U[1,maxLength]`.
    constexpr auto maxLength{30UL};
    Turingforge::BalancedCreator creator{problem.GetPrimitiveSet(), problem.GetInputs()};
    auto [minArity, maxArity] = problem.GetPrimitiveSet().FunctionArityLimits();
    Turingforge::UniformIndividualInitializer individualInitializer(creator);
    individualInitializer.SetTerms(maxLength);
    individualInitializer.ParameterizeDistribution(minArity + 1, maxLength);
    individualInitializer.SetObjectivesCount(1);
    using Uniform = std::uniform_real_distribution<Turingforge::Scalar>;
    Turingforge::CoefficientInitializer<Uniform> coeffInitializer;
    coeffInitializer.ParameterizeDistribution(-5.F, +5.F);

    // Finally, we can configure the genetic algorithm and run it. A callback function can be provided to the algorithm
    // in order to report progress at the end of each generation.
    Turingforge::AlgorithmConfig config{};
    config.Generations = 100;
    config.Evaluations = maxEvaluations;
    config.PopulationSize = 1000;
    config.PoolSize = 1000;
    config.CrossoverProbability = 1;
    config.MutationProbability = 0.25;
    config.Seed = 42;
    config.TimeLimit = std::numeric_limits<size_t>::max();

    // set up a genetic programming algorithm
    Turingforge::GeneticProgrammingAlgorithm gp(problem, config, individualInitializer, coeffInitializer, generator, reinserter);

    int generation = 0;
    auto report = [&] { fmt::print("{}\n", ++generation); };
    Turingforge::RandomGenerator rng{config.Seed};
    gp.Run(rng, report);

    return 0;
}