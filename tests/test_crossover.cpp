#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <catch2/matchers/catch_matchers.hpp>

#include <random>
#include <vstat/vstat.hpp>
#include <fmt/core.h>

#include "turing_forge/core/dataset.hpp"
#include "turing_forge/formatter/formatter.hpp"
#include "turing_forge/core/primitive_set.hpp"
#include "turing_forge/core/variable.hpp"
#include "turing_forge/operators/creator.hpp"
#include "turing_forge/operators/crossover.hpp"
#include "turing_forge/core/types.hpp"

TEST_CASE("Crossover", "Basic operations") {
    std::string target = "ΔP";
    Turingforge::Dataset ds("../../data/simple.csv", /*hasHeader=*/true);
    auto variables = ds.GetVariables();
    std::vector<Turingforge::Hash> inputs;
    for (auto const& v : variables) {
        if (v.Name != target) { inputs.push_back(v.Hash); }
    }
    auto const nrow { ds.Rows<std::size_t>() };
    Turingforge::Range range { 0, nrow };

    Turingforge::PrimitiveSet grammar;
    grammar.SetConfig(Turingforge::PrimitiveSet::Unary);

    Turingforge::BalancedCreator btc { grammar, inputs, /* bias= */ 0.0 };

    Turingforge::RandomGenerator random(1234);

    SECTION("Simple swap") {
        Turingforge::RandomGenerator rng(std::random_device{}());
        constexpr size_t objectives{1000};
        constexpr size_t maxLength{100};
        constexpr double internalNodeProbability{0.9};
        Turingforge::IndividualCrossover cx(internalNodeProbability, maxLength);
        auto p1 = btc(rng, 7, 1, objectives); // NOLINT
        auto p2 = btc(rng, 5, 1, objectives); // NOLINT
        auto child = cx(rng, p1, p2);

        fmt::print("parent 1\n{}\n", Turingforge::IndividualFormatter::Format(p1, ds, 2));
        fmt::print("parent 2\n{}\n", Turingforge::IndividualFormatter::Format(p2, ds, 2));
        fmt::print("child\n{}\n", Turingforge::IndividualFormatter::Format(child, ds, 2));
    }

    SECTION("Distribution of swap locations") {
        Turingforge::RandomGenerator rng(std::random_device{}());

        size_t objectives{1000}, maxLength{20};
        Turingforge::IndividualCrossover cx(1.0, maxLength);

        std::vector<double> c1(maxLength);
        std::vector<double> c2(maxLength);

        uint64_t p1_term{0}, p1_func{0};
        uint64_t p2_term{0}, p2_func{0};

        for (int n = 0; n < 100000; ++n) {
            auto p1 = btc(rng, maxLength, 1, objectives);
            //auto p2 = btc(rng, maxLength, 1, maxDepth);
            auto p2 = p1;

            auto [i, j] = cx.FindCompatibleSwapLocations(rng, p1.Length, p2.Length);
            c1[i]++;
            c2[j]++;
        }

        fmt::print("parents swap location sampling counts:\n");
        for (size_t i = 0; i < maxLength; ++i) {
            fmt::print("{} {} {}\n", i, c1[i], c2[i]);
        }
    }

    SECTION("Child size") {
        const int n = 100000;
        std::vector<Turingforge::Individual> individuals;
        constexpr size_t objectives{1000};
        constexpr size_t maxLength{100};
        auto sizeDistribution = std::uniform_int_distribution<size_t>(1, maxLength);
        for (int i = 0; i < n; ++i) {
            individuals.push_back(btc(random, sizeDistribution(random), 1UL, objectives));
        }
        std::vector<std::array<size_t, 3>> sizes;

        std::uniform_int_distribution<size_t> dist(0, n-1);
        for (auto p : { 0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0 }) {
            Turingforge::IndividualCrossover cx(p, maxLength);
            for (int i = 0; i < n; ++i) {
                auto p1 = dist(random);
                auto p2 = dist(random);
                auto c = cx(random, individuals[p1], individuals[p1]);
                sizes.push_back({individuals[p1].Length, individuals[p2].Length, c.Length});
            }
            double m1 = vstat::univariate::accumulate<double>(sizes.begin(), sizes.end(), [](auto const& arr) { return arr[0]; }).mean;
            double m2 = vstat::univariate::accumulate<double>(sizes.begin(), sizes.end(), [](auto const& arr) { return arr[1]; }).mean;
            double m3 = vstat::univariate::accumulate<double>(sizes.begin(), sizes.end(), [](auto const& arr) { return arr[2]; }).mean;
            fmt::print("p: {:.1f}, parent1: {:.2f}, parent2: {:.2f}, child: {:.2f}\n", p, m1, m2, m3);
        }
    }
}