#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <catch2/matchers/catch_matchers.hpp>

#include <algorithm>
#include <functional>
#include <ranges>
#include <random>
#include <thread>
#include <fmt/core.h>
#include <fmt/color.h>
#include <fmt/ranges.h>

#include "turing_forge/core/dataset.hpp"
#include "turing_forge/core/primitive_set.hpp"
#include "turing_forge/hash/hash.hpp"
#include "turing_forge/interpreter/interpreter.hpp"
#include "turing_forge/operators/creator.hpp"
#include "turing_forge/operators/evaluator.hpp"
#include "turing_forge/operators/non_dominated_sorter.hpp"

TEST_CASE( "Non-dominated Sort" ) {
    Turingforge::RandomGenerator rd(1234);

    auto initializePop = [](Turingforge::RandomGenerator& random, auto& dist, size_t n, size_t m) {
        std::vector<Turingforge::Individual> individuals(n);

        for (auto & individual : individuals) {
            individual.Fitness.resize(m);
            for (size_t j = 0; j < m; ++j) {
                individual[j] = dist(random);
            }
        }

        std::stable_sort(individuals.begin(), individuals.end(), [](auto const& a, auto const& b){ return std::ranges::lexicographical_compare(a.Fitness, b.Fitness); });

        for(auto i = individuals.begin(); i < individuals.end(); ) {
            i->Rank = 0;
            auto j = i + 1;
            for (; j < individuals.end() && i->Fitness == j->Fitness; ++j) {
                j->Rank = 1;
            }
            i = j;
        }
        auto r = std::stable_partition(individuals.begin(), individuals.end(), [](auto const& ind) { return !ind.Rank; });
        std::vector<Turingforge::Individual> pop(individuals.begin(), r);
        return pop;
    };

    auto print = [&](auto& fr) {
        for (size_t i = 0; i < fr.size(); ++i) {
            fmt::print("{}: ", i);
            std::sort(fr[i].begin(), fr[i].end());
            //for (auto f : fronts[i]) { std::cout << f << ":(" << eigenMap(pop[f].Fitness).transpose() << ") "; }
            for (auto f : fr[i]) { std::cout << f << " "; }
            fmt::print("\n");
        }
        fmt::print("\n");
    };

    auto test = [&](int n, int m, auto&& sorter)
    {
        std::uniform_real_distribution<Turingforge::Scalar> dist(0, 1);
        auto pop = initializePop(rd, dist, n, m);

        Turingforge::Less less;
        Turingforge::Equal eq;

        Turingforge::Scalar const eps{0};
        std::stable_sort(pop.begin(), pop.end(), [&](auto const& lhs, auto const& rhs) { return less(lhs.Fitness, rhs.Fitness, eps); });
        for(auto i = pop.begin(); i < pop.end(); ) {
            i->Rank = 0;
            auto j = i + 1;
            for (; j < pop.end() && eq(i->Fitness, j->Fitness, eps); ++j) {
                j->Rank = 1;
            }
            i = j;
        }
        auto r = std::stable_partition(pop.begin(), pop.end(), [](auto const& ind) { return !ind.Rank; });
        Turingforge::Span<Turingforge::Individual const> s(pop.begin(), r);

        return sorter(s);
    };

    auto compareSorters = [&](auto const& s1, auto const& s2, auto const& ns, auto const& ms) {
        std::uniform_real_distribution<Turingforge::Scalar> dist(0, 1);
        for (auto n : ns) {
            for (auto m : ms) {
                auto pop = initializePop(rd, dist, n, m);

                auto f1 = s1.Sort(pop, 0);
                auto f2 = s2.Sort(pop, 0);

                if (f1.size() != f2.size()) {
                    return false;
                }
                for (auto i = 0; i < std::ssize(f1); ++i) {
                    std::ranges::stable_sort(f1[i]);
                    std::ranges::stable_sort(f2[i]);
                    if (f1[i] != f2[i]) {
                        return false;
                    }
                }
            }
        }
        return true;
    };

    SECTION("Compare Sorters") {
        std::array ns { 100, 1000, 10000, 50000, 100000 };
        std::array ms { 2, 3, 4, 5, 6, 7, 8, 9, 10, 13, 17, 20, 23, 40 };
        Turingforge::RankIntersectSorter rs;
        Turingforge::RankOrdinalSorter ro;
        Turingforge::MergeSorter mnds;
        Turingforge::BestOrderSorter bos;
        Turingforge::HierarchicalSorter hnds;
        Turingforge::DeductiveSorter ds;
        Turingforge::EfficientBinarySorter ebs;
        Turingforge::EfficientSequentialSorter ess;
        std::vector<std::reference_wrapper<Turingforge::NondominatedSorterBase const>> sorters{ ro, mnds, bos, hnds, ds, ebs, ess };
        std::vector<std::string> names{ "ro", "ms", "bos", "hs", "ds", "ebs", "ess" };
        fmt::print("rs -- ");
        for (auto i = 0; i < std::ssize(sorters); ++i) {
            auto const& sorter = sorters[i].get();
            auto res = compareSorters(rs, sorter, ns, ms);
            fmt::print(fmt::fg(res ? fmt::color::green : fmt::color::red), "{} ", names[i]);
        }
        fmt::print("\n");
    }

    SECTION("test 1")
    {
        std::vector<std::vector<Turingforge::Scalar>> points = {{0, 7}, {1, 5}, {2, 3}, {4, 2}, {7, 1}, {10, 0}, {2, 6}, {4, 4}, {10, 2}, {6, 6}, {9, 5}};
        std::vector<Turingforge::Individual> pop(points.size());
        for (size_t i = 0; i < points.size(); ++i) {
            pop[i].Fitness = points[i];
        }
        fmt::print("DS\n");
        Turingforge::DeductiveSorter ds;
        auto fronts = ds(pop);
        print(fronts);

        fmt::print("HS\n");
        Turingforge::HierarchicalSorter hs;
        fronts = hs(pop);
        print(fronts);

        fmt::print("ENS-SS\n");
        Turingforge::EfficientSequentialSorter es;
        fronts = es(pop);
        print(fronts);

        fmt::print("ENS-BS\n");
        Turingforge::EfficientBinarySorter eb;
        fronts = eb(pop);
        print(fronts);

        fmt::print("RO\n");
        Turingforge::RankOrdinalSorter ro;
        fronts = ro(pop);
        print(fronts);

        fmt::print("RS\n");
        Turingforge::RankIntersectSorter rs;
        fronts = rs(pop);
        print(fronts);

        fmt::print("MNDS\n");
        Turingforge::MergeSorter ms;
        fronts = ms(pop);
        print(fronts);

        fmt::print("BOS\n");
        Turingforge::BestOrderSorter bos;
        fronts = bos(pop);
        print(fronts);
    }

    SECTION("test 2")
    {
        std::vector<std::vector<Turingforge::Scalar>> points = {{1, 2, 3}, {-2, 3, 7}, {-1, -2, -3}, {0, 0, 0}};
        std::vector<Turingforge::Individual> pop(points.size());
        for (size_t i = 0; i < points.size(); ++i) {
            pop[i].Fitness = points[i];
        }
        std::stable_sort(pop.begin(), pop.end(), Turingforge::LexicographicalComparison{});
        fmt::print("DS\n");
        Turingforge::DeductiveSorter ds;
        auto fronts = ds(pop);
        print(fronts);

        fmt::print("HS\n");
        Turingforge::HierarchicalSorter hs;
        fronts = hs(pop);
        print(fronts);

        fmt::print("ENS-SS\n");
        Turingforge::EfficientSequentialSorter es;
        fronts = es(pop);
        print(fronts);

        fmt::print("ENS-BS\n");
        Turingforge::EfficientBinarySorter eb;
        fronts = eb(pop);
        print(fronts);

        fmt::print("RO\n");
        Turingforge::RankOrdinalSorter ro;
        fronts = ro(pop);
        print(fronts);

        fmt::print("RS\n");
        Turingforge::RankIntersectSorter rs;
        fronts = rs(pop);
        print(fronts);

        fmt::print("MNDS\n");
        Turingforge::MergeSorter ms;
        fronts = ms(pop);
        print(fronts);
    }

    SECTION("test 3")
    {
        std::vector<std::vector<Turingforge::Scalar>> points = {
                { 0.79, 0.35 },
                { 0.40, 0.71 },
                { 0.15, 0.014 },
                { 0.46, 0.82 },
                { 0.28, 0.98 },
                { 0.31, 0.74 },
                { 0.82, 0.52 },
                { 0.84, 0.19 },
                { 0.85, 0.78 },
                { 0.96, 0.83 }
        };

        std::vector<Turingforge::Individual> pop(points.size());
        for (size_t i = 0; i < points.size(); ++i) {
            pop[i].Fitness = points[i];
        }
        std::vector<int> indices(points.size());
        std::iota(indices.begin(), indices.end(), 0);
        std::stable_sort(indices.begin(), indices.end(), [&](auto i, auto j) { return Turingforge::LexicographicalComparison{}(pop[i], pop[j]); });
        fmt::print("indices: {}\n", indices);

        std::stable_sort(pop.begin(), pop.end(), Turingforge::LexicographicalComparison{});

        fmt::print("RS\n");
        auto fronts = Turingforge::RankOrdinalSorter{}(pop);
        // replace indices with original values
        for (auto &f : fronts) {
            for (auto &v : f) {
                v = indices[v] + 1;
            }
        }

        print(fronts);
    }

    SECTION("rank sort")
    {
        std::uniform_real_distribution<Turingforge::Scalar> dist(0, 1);
        auto pop = initializePop(rd, dist, 100, 2);
        Turingforge::RankIntersectSorter rs;
        std::vector<std::vector<size_t>> fronts;

        fronts = rs(pop);
        fmt::print("RS comparisons: {} {} {} {}\n", rs.Stats.LexicographicalComparisons, rs.Stats.SingleValueComparisons, rs.Stats.RankComparisons, rs.Stats.InnerOps);
        print(fronts);
        rs.Reset();
    }

    SECTION("MNDS")
    {
        size_t n = 100;
        size_t m = 2;

        auto fronts = test(n, m, Turingforge::MergeSorter{});
        fmt::print("mnds\n");
        print(fronts);
    }

    SECTION("RS")
    {
        size_t n = 20;
        size_t m = 2;

        auto fronts = test(n, m, Turingforge::RankIntersectSorter{});
        fmt::print("rs\n");
        print(fronts);
    }

    SECTION("basic")
    {
        size_t n = 20;
        size_t m = 3;

        std::uniform_real_distribution<Turingforge::Scalar> dist(-1, 1);
        auto pop = initializePop(rd, dist, n, m);

        Turingforge::Less less;
        Turingforge::Equal eq;

        Turingforge::Scalar const eps{0};
        std::stable_sort(pop.begin(), pop.end(), [&](auto const& lhs, auto const& rhs) { return less(lhs.Fitness, rhs.Fitness, eps); });
        for(auto i = pop.begin(); i < pop.end(); ) {
            i->Rank = 0;
            auto j = i + 1;
            for (; j < pop.end() && eq(i->Fitness, j->Fitness, eps); ++j) {
                j->Rank = 1;
            }
            i = j;
        }
        auto r = std::stable_partition(pop.begin(), pop.end(), [](auto const& ind) { return !ind.Rank; });
        Turingforge::Span<Turingforge::Individual const> s(pop.begin(), r);

        auto hash = [](auto const& vec) {
            auto pt = reinterpret_cast<uint8_t const*>(vec.data());
            auto sz = vec.size() * sizeof(uint64_t) / sizeof(uint8_t);
            return Turingforge::Hasher{}(pt, sz);
        };

        auto computeHash = [&hash](auto const& fronts) {
            std::vector<uint64_t> hashes;
            std::transform(fronts.begin(), fronts.end(), std::back_inserter(hashes),
                           [&hash](auto const& f) { return hash(f); });
            return hash(hashes);
        };

        auto fronts = Turingforge::RankOrdinalSorter{}(s, eps);
        std::cout << "rank ordinal " << computeHash(fronts) << "\n";
        print(fronts);

        fronts = Turingforge::RankIntersectSorter{}(s, eps);
        std::cout << "rank intersect " << computeHash(fronts) << "\n";
        print(fronts);

         fronts = Turingforge::DeductiveSorter{}(s, eps);
         std::cout << "deductive sort\n";
         print(fronts);

         fronts = Turingforge::HierarchicalSorter{}(s, eps);
         std::cout << "hnds " << computeHash(fronts) << "\n";
         print(fronts);

         fronts = Turingforge::DominanceDegreeSorter{}(s, eps);
         std::cout << "dominance degree sort\n";
         print(fronts);

         fronts = Turingforge::EfficientSequentialSorter{}(s, eps);
         std::cout << "ens-ss\n";
         print(fronts);

         fronts = Turingforge::EfficientBinarySorter{}(s, eps);
         std::cout << "ens-bs\n";
         print(fronts);

        fronts = Turingforge::MergeSorter{}(s, eps);
        std::cout << "mnds " << computeHash(fronts) << "\n";
        print(fronts);

         fronts = Turingforge::BestOrderSorter{}(s, eps);
         std::cout << "bos " << computeHash(fronts) << "\n";
         print(fronts);
    }

    SECTION("bit density")
    {
        size_t reps = 1000;

        // TODO: make <= 10000
        for (size_t nn = 1000; nn <= 1000; nn += 1000) {
            for (size_t mm = 2; mm <= 5; ++mm) {
                for (auto i = 0UL; i < reps; ++i) {
                    std::uniform_real_distribution<Turingforge::Scalar> dist(0, 1);
                    auto pop = initializePop(rd, dist, nn, mm);
                    auto fronts = Turingforge::RankIntersectSorter{}(pop);
                }
            }
        }
    }

    auto testComparisons = [&](std::string const& name, Turingforge::NondominatedSorterBase& sorter)
    {
        size_t reps = 1000;
        std::uniform_real_distribution<Turingforge::Scalar> dist(0, 1);

        fmt::print("name,n,m,lc,sv,dc,rc,ops,mean_rk,mean_nd\n");
        // TODO: make <= 2000
        for (size_t n = 100; n <= 200; n += 100) {
            for (size_t m = 2; m <= 2; ++m) {
                double lc{0};
                double dc{0};
                double rc{0};
                double sv{0};
                double ops{0};
                double mean_rank{0};
                double mean_front_size{0};
                double mean_nd{0};
                for (size_t r = 0; r < reps; ++r) {
                    auto pop = initializePop(rd, dist, n, m);
                    auto fronts = sorter(pop);

                    double rk = 0;
                    for (size_t i = 0UL; i < fronts.size(); ++i) {
                        rk += i * fronts[i].size();
                    }
                    rk /= n;
                    mean_rank = mean_rank + rk;
                    mean_front_size = mean_front_size + static_cast<double>(n) / static_cast<double>(fronts.size());

                    auto [lc_, sv_, dc_, rc_, ops_, rk_, nd_, el_] = sorter.Stats;
                    lc = lc + static_cast<double>(lc_);
                    sv = sv + static_cast<double>(sv_);
                    dc = dc + static_cast<double>(dc_);
                    rc = rc + static_cast<double>(rc_);
                    ops = ops + static_cast<double>(ops_);
                    mean_nd = mean_nd + nd_;
                    sorter.Reset();
                }
                auto r = static_cast<double>(reps);
                fmt::print("{},{},{},{},{},{},{},{},{},{}\n", name, n, m, lc/r, sv/r, dc/r, rc/r, ops/r, mean_rank/r, mean_front_size/r);
            }
        }
    };

    SECTION("comparisons RS")
    {
        Turingforge::RankIntersectSorter sorter;
        testComparisons("RS", sorter);
    }

    SECTION("comparisons DS")
    {
        Turingforge::DeductiveSorter sorter;
        testComparisons("DS", sorter);
    }

    SECTION("comparisons HS")
    {
        Turingforge::HierarchicalSorter sorter;
        testComparisons("HS", sorter);
    }

    SECTION("comparisons ENS-SS")
    {
        Turingforge::EfficientSequentialSorter sorter;
        testComparisons("ENS-SS", sorter);
    }

    SECTION("comparisons ENS-BS")
    {
        Turingforge::EfficientBinarySorter sorter;
        testComparisons("ENS-BS", sorter);
    }
}