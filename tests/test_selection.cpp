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

TEST_CASE("Selection Distribution")
{
    size_t maxLength = 100;
    size_t nIndividuals = 100;
    Turingforge::Scalar expoLim = 4;

    auto random = Turingforge::RandomGenerator(1234);
    auto ds = Turingforge::Dataset("../../data/simple.csv", true);
    std::string targetName = "ΔP";
    Turingforge::Variable target = *ds.GetVariable(targetName);
    auto inputs = ds.VariableHashes();
    std::erase(inputs, target.Hash);

    std::uniform_int_distribution<size_t> sizeDistribution(1, maxLength);

    Turingforge::PrimitiveSet grammar;
    grammar.SetConfig(Turingforge::PrimitiveSet::Arithmetic | Turingforge::FunctionType::Log | Turingforge::FunctionType::Exp);
    Turingforge::BalancedCreator creator = Turingforge::BalancedCreator{grammar, inputs, 0.0};

    std::vector<Turingforge::Individual> individuals;
    individuals.reserve(nIndividuals);
    for (size_t i = 0; i < nIndividuals; ++i) {
        individuals.push_back(creator(random, sizeDistribution(random), expoLim, 1));
        individuals[i].Fitness[0] = std::uniform_real_distribution(0.0, 1.0)(random);
    }

    auto comp = [](auto const& lhs, auto const& rhs) { return lhs.Fitness < rhs.Fitness; };

    Turingforge::ProportionalSelector proportionalSelector(comp);
    proportionalSelector.SetObjIndex(0);
    proportionalSelector.Prepare(individuals);

    Turingforge::TournamentSelector tournamentSelector(comp);
    tournamentSelector.SetTournamentSize(2);
    tournamentSelector.Prepare(individuals);

    Turingforge::RankTournamentSelector rankedSelector(comp);
    rankedSelector.SetTournamentSize(2);
    rankedSelector.Prepare(individuals);

    auto plotHist = [&](Turingforge::SelectorBase& selector)
    {
        std::vector<size_t> hist(individuals.size());

        for (size_t i = 0; i < 100 * nIndividuals; ++i)
        {
            hist[selector(random)]++;
        }
        std::sort(hist.begin(), hist.end(), std::greater<>{});
        for (size_t i = 0; i < nIndividuals; ++i)
        {
            auto qty = hist[i];
            fmt::print("{:>5}\t{}\n", i, static_cast<double>(qty) / 100.0);
        }
    };

    SECTION("Proportional Selector") {
        fmt::print("proportional selector\n");
        plotHist(proportionalSelector);
    }

    SECTION("Size 2 Tournament Selector") {
        SECTION("Tournament Selector") {
            fmt::print("\ntournament 2\n");
            plotHist(tournamentSelector);
        }

        SECTION("Ranked Selector") {
            fmt::print("\nranked 2\n");
            plotHist(rankedSelector);
        }
    }

    SECTION("Size 3 Tournament Selector") {
        SECTION("Tournament Selector") {
            fmt::print("\ntournament 3\n");
            tournamentSelector.SetTournamentSize(3);
            plotHist(tournamentSelector);
        }

        SECTION("Ranked Selector") {
            fmt::print("\nranked 3\n");
            rankedSelector.SetTournamentSize(3);
            plotHist(rankedSelector);
        }
    }
}