#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <catch2/matchers/catch_matchers.hpp>

#include <fmt/core.h>

#include "turing_forge/core/dataset.hpp"
#include "turing_forge/formatter/formatter.hpp"
#include "turing_forge/core/primitive_set.hpp"
#include "turing_forge/core/variable.hpp"
#include "turing_forge/operators/creator.hpp"
#include "turing_forge/operators/crossover.hpp"
#include "turing_forge/operators/initializer.hpp"
#include "turing_forge/operators/mutation.hpp"
#include "turing_forge/core/individual.hpp"

TEST_CASE( "Insert Mutation" ) {
    size_t maxLength = 10;
    size_t nIndividuals = 100;
    Turingforge::Scalar expoLim = 4;

    auto random = Turingforge::RandomGenerator(std::random_device {}());
    auto ds = Turingforge::Dataset("../../data/simple.csv", true);
    std::string targetName = "ΔP";
    Turingforge::Variable target = *ds.GetVariable(targetName);
    auto inputs = ds.VariableHashes();
    std::erase(inputs, target.Hash);

    Turingforge::PrimitiveSet grammar;
    grammar.SetConfig(Turingforge::PrimitiveSet::Arithmetic | Turingforge::FunctionType::Log | Turingforge::FunctionType::Exp | Turingforge::FunctionType::Cos);
    Turingforge::BalancedCreator creator = Turingforge::BalancedCreator{grammar, inputs, 0.0};
    Turingforge::UniformCoefficientInitializer cfi;

    auto sizeDistribution = std::uniform_int_distribution<size_t>(1, maxLength);
    auto targetLen = sizeDistribution(random);

    auto individual = creator(random, targetLen, expoLim, 1);
    fmt::print("Original individual:\t{}\n", Turingforge::IndividualFormatter::Format(individual, ds));

    Turingforge::InsertInteractionMutation insertInteraction(creator, cfi, 2 * targetLen);
    auto child1 = insertInteraction(random, individual);
    fmt::print("Insert interactions:\t{}\n", Turingforge::IndividualFormatter::Format(child1, ds));

    Turingforge::RemoveInteractionMutation removeInteraction(grammar);
    auto child2 = removeInteraction(random, individual);
    fmt::print("Removed interaction:\t{}\n", Turingforge::IndividualFormatter::Format(child2, ds));

    Turingforge::ReplaceInteractionMutation replaceInteraction(creator, cfi, 2 * targetLen);
    auto child3 = replaceInteraction(random, individual);
    fmt::print("Replaced interactions:\t{}\n", Turingforge::IndividualFormatter::Format(child3, ds));

    Turingforge::ShuffleInteractionsMutation shuffleInteractions;
    auto child4 = shuffleInteractions(random, individual);
    fmt::print("Shuffle interactions:\t{}\n", Turingforge::IndividualFormatter::Format(child4, ds));

    Turingforge::ChangeFunctionMutation changeFunction(grammar);
    auto child5 = changeFunction(random, individual);
    fmt::print("Change interaction: \t{}\n", Turingforge::IndividualFormatter::Format(child5, ds));

    Turingforge::DiscretePointMutation discretePoint;
    auto child6 = discretePoint(random, individual);
    for (auto v : Turingforge::Math::Constants) {
        discretePoint.Add(static_cast<Turingforge::Scalar>(v), 1);
    }
    fmt::print("Discrete point mutation:{}\n", Turingforge::IndividualFormatter::Format(child6, ds));

    using Dist = std::uniform_int_distribution<int>;
    int constexpr range{5};

    Turingforge::OnePointMutation<Dist> changeCoefficient;
    changeCoefficient.ParameterizeDistribution(-range, +range);
    auto child7 = changeCoefficient(random, individual);
    fmt::print("One point mutation: \t{}\n", Turingforge::IndividualFormatter::Format(child7, ds));

    Turingforge::MultiPointMutation<Dist> changeMultipleCoefficients;
    changeMultipleCoefficients.ParameterizeDistribution(-range, +range);
    auto child8= changeMultipleCoefficients(random, individual);
    fmt::print("Multi point mutation:\t{}\n", Turingforge::IndividualFormatter::Format(child8, ds));

    Turingforge::MultiMutation mutator;
    mutator.Add(insertInteraction, 1.0);
    mutator.Add(removeInteraction, 1.0);
    mutator.Add(replaceInteraction, 1.0);
    mutator.Add(shuffleInteractions, 1.0);
    mutator.Add(changeFunction, 1.0);
    mutator.Add(discretePoint, 1.0);
    auto child9 = mutator(random, individual);
    fmt::print("Combined mutator:\t\t{}\n", Turingforge::IndividualFormatter::Format(child9, ds));
}