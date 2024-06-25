#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "turing_forge/core/dataset.hpp"
#include "turing_forge/formatter/formatter.hpp"
#include "turing_forge/core/primitive_set.hpp"
#include "turing_forge/operators/creator.hpp"
//#include "turing_forge/operators/crossover.hpp"
//#include "turing_forge/operators/initializer.hpp"

TEST_CASE("Sample nodes")
{
    Turingforge::PrimitiveSet grammar;
    grammar.SetConfig(static_cast<Turingforge::FunctionType>(~uint32_t{0}));
    Turingforge::RandomGenerator rd(std::random_device {}());

    std::vector<double> observed(Turingforge::FunctionTypes::Count, 0);
    size_t r = grammar.EnabledPrimitives().size() + 1;

    const size_t nTrials = 1'000'000;
    for (auto i = 0U; i < nTrials; ++i) {
        auto node = grammar.SampleRandomSymbol(rd, 0, 2);
        ++observed[Turingforge::FunctionTypes::GetIndex(node.Type)];
    }
    std::transform(observed.begin(), observed.end(), observed.begin(), [&](double v) { return v / nTrials; });
    std::vector<double> actual(Turingforge::FunctionTypes::Count, 0);
    for (size_t i = 0; i < observed.size(); ++i) {
        auto type = static_cast<Turingforge::FunctionType>(1U << i);
        auto node = Turingforge::Function(type);
        actual[Turingforge::FunctionTypes::GetIndex(type)] = static_cast<double>(grammar.Frequency(node.HashValue));
    }
    auto freqSum = std::reduce(actual.begin(), actual.end(), 0.0, std::plus {});
    std::transform(actual.begin(), actual.end(), actual.begin(), [&](double v) { return v / freqSum; });
    auto chi = 0.0;
    for (auto i = 0U; i < observed.size(); ++i) {
        Turingforge::Function node(static_cast<Turingforge::FunctionType>(1U << i));
        if (!grammar.IsEnabled(node.HashValue)) {
            continue;
        }
        auto x = observed[i];
        auto y = actual[i];
        //fmt::print("{:>8} observed {:.4f}, expected {:.4f}\n", node.Name(), x, y);
        REQUIRE_THAT(x, Catch::Matchers::WithinRel(y, 0.1));
        chi += (x - y) * (x - y) / y;
    }
    chi *= nTrials;
    auto criticalValue = static_cast<double>(r) + 2 * std::sqrt(r);
    //fmt::print("chi = {}, critical value = {}\n", chi, criticalValue);
    REQUIRE(chi <= criticalValue);
}

TEST_CASE("Create Individuals")
{
    const auto *target = "Y";
    auto ds = Turingforge::Dataset("../../data/Poly-10.csv", /*hasHeader=*/true);
    auto variables = ds.GetVariables();

    auto inputs = ds.VariableHashes();
    std::erase(inputs, ds.GetVariable(target)->Hash);
    size_t const maxLength = 100;

    size_t const n = 10000;
    auto sizeDistribution = std::uniform_int_distribution<size_t>(1, maxLength);

    Turingforge::PrimitiveSet grammar;
    grammar.SetConfig(Turingforge::PrimitiveSet::Arithmetic | Turingforge::FunctionType::Log | Turingforge::FunctionType::Exp);
    grammar.SetMaximumArity(Turingforge::Function(Turingforge::FunctionType::Add), 2);
    grammar.SetMaximumArity(Turingforge::Function(Turingforge::FunctionType::Mul), 2);
    grammar.SetMaximumArity(Turingforge::Function(Turingforge::FunctionType::Sub), 2);
    grammar.SetMaximumArity(Turingforge::Function(Turingforge::FunctionType::Div), 2);

    grammar.SetFrequency(Turingforge::Function(Turingforge::FunctionType::Add), 4);
    grammar.SetFrequency(Turingforge::Function(Turingforge::FunctionType::Mul), 1);
    grammar.SetFrequency(Turingforge::Function(Turingforge::FunctionType::Sub), 1);
    grammar.SetFrequency(Turingforge::Function(Turingforge::FunctionType::Div), 1);
    grammar.SetFrequency(Turingforge::Function(Turingforge::FunctionType::Exp), 1);
    grammar.SetFrequency(Turingforge::Function(Turingforge::FunctionType::Log), 1);

    Turingforge::BalancedCreator btc{grammar, inputs, /* bias= */ 0.0};

    Turingforge::RandomGenerator random(std::random_device{}());

    std::vector<size_t> lengths(n);

    SECTION("Simple Individual") {
        auto individual = btc(random, 2, 3, 1);
    }

    SECTION("Simple Evaluation") {
        using Data = Turingforge::Vector<Turingforge::Scalar>;
        Data X {0.95, 3.75};
        Eigen::VectorXd vecX = Eigen::Map<const Eigen::VectorXd>(X.data(), X.size());

        auto individual = Turingforge::Individual(
                Turingforge::Vector<Turingforge::Scalar>{17.36912419791657},
                Turingforge::Vector<Turingforge::Function>{Turingforge::Function(Turingforge::FunctionType::Exp)},
                Turingforge::Vector<Turingforge::Vector<Turingforge::Scalar>>{{3, 0}}
                );
         fmt::print("{}\n", Turingforge::IndividualFormatter::Format(individual, ds));
        REQUIRE(individual.eval(vecX) == 40.93842704843804);
    }
}