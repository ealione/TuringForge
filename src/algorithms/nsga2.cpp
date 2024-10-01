#include <algorithm>                                 // for stable_sort, copy_n, max
#include <atomic>                                    // for atomic_bool
#include <chrono>                                    // for steady_clock
#include <cmath>                                     // for isfinite
#include <iterator>                                  // for move_iterator, back_inse...
#include <limits>                                    // for numeric_limits
#include <memory>                                    // for allocator, allocator_tra...
#include <optional>                                  // for optional
#include <ranges>                                    // for ranges
#include <taskflow/taskflow.hpp>                     // for taskflow, subflow
#include <taskflow/algorithm/for_each.hpp>           // for taskflow.for_each_index
#include <vector>                                    // for vector, vector::size_type
#include <fmt/ranges.h>

#include "turing_forge/algorithms/nsga2.hpp"
#include "turing_forge/core/contracts.hpp"                 // for ENSURE
#include "turing_forge/core/operator.hpp"                  // for OperatorBase
#include "turing_forge/core/problem.hpp"                   // for Problem
#include "turing_forge/core/range.hpp"                     // for Range
#include "turing_forge/core/individual.hpp"                // for Individual
#include "turing_forge/operators/initializer.hpp"          // for CoefficientInitializerBase
#include "turing_forge/operators/non_dominated_sorter.hpp" // for RankSorter
#include "turing_forge/operators/reinserter.hpp"           // for ReinserterBase

namespace Turingforge {

    auto NSGA2::Sort(Turingforge::Span<Individual> pop) -> void
    {
        auto eps = static_cast<Turingforge::Scalar>(GetConfig().Epsilon);
        auto eq = [eps](auto const& lhs, auto const& rhs) { return Turingforge::Equal{}(lhs.Fitness, rhs.Fitness, eps); };
        // sort the population lexicographically
        std::stable_sort(pop.begin(), pop.end(), [](auto const& a, auto const& b){ return std::ranges::lexicographical_compare(a.Fitness, b.Fitness); });
        // mark the duplicates for stable_partition
        for(auto i = pop.begin(); i < pop.end(); ) {
            i->Rank = 0;
            auto j = i + 1;
            for (; j < pop.end() && eq(*i, *j); ++j) {
                j->Rank = 1;
            }
            i = j;
        }
        auto r = std::stable_partition(pop.begin(), pop.end(), [](auto const& ind) { return !ind.Rank; });
        Turingforge::Span<Turingforge::Individual const> uniq(pop.begin(), r);
        // do the sorting
        fronts_ = sorter_(uniq, eps);
        // sort the fronts for consistency between sorting algos
        for (auto& f : fronts_) {
            std::stable_sort(f.begin(), f.end());
        }
        // banish the duplicates into the last front
        if (r < pop.end()) {
            std::vector<size_t> last(pop.size() - uniq.size());
            std::iota(last.begin(), last.end(), uniq.size());
            fronts_.push_back(last);
        }
        // calculate crowding distance
        //UpdateDistance(pop);
        // update best front
        best_.clear();
        std::transform(fronts_.front().begin(), fronts_.front().end(), std::back_inserter(best_), [&](auto i) { return pop[i]; });
    }

    auto NSGA2::Run(tf::Executor& executor, RandomGenerator& random, std::function<void()> report) -> void
    {
        const auto& config = GetConfig();
        const auto& individualInit = GetIndividualInitializer();
        const auto& coeffInit = GetCoefficientInitializer();
        const auto& generator = GetGenerator();
        const auto& reinserter = GetReinserter();
        const auto& problem = GetProblem();

        auto t0 = std::chrono::steady_clock::now();
        auto elapsed = [t0]() {
            auto t1 = std::chrono::steady_clock::now();
            constexpr double ms{1e3};
            return static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count()) / ms;
        };

        // random seeds for each thread
        size_t s = std::max(config.PopulationSize, config.PoolSize);
        std::vector<Turingforge::RandomGenerator> rngs;
        for (size_t i = 0; i < s; ++i) {
            rngs.emplace_back(random());
        }

        auto const& evaluator = generator.Evaluator();

        // we want to allocate all the memory that will be necessary for evaluation (e.g. for storing model responses)
        // in one go and use it throughout the generations in order to minimize the memory pressure
        auto trainSize = problem.TrainingRange().Size();

        ENSURE(executor.num_workers() > 0);
        std::vector<Turingforge::Vector<Turingforge::Scalar>> slots(executor.num_workers());

        tf::Taskflow taskflow;

        auto stop = [&]() {
            return generator.Terminate() || Generation() == config.Generations || elapsed() > static_cast<double>(config.TimeLimit);
        };

        auto& individuals = Individuals();
        auto parents      = Parents();
        auto offspring    = Offspring();

        // while loop control flow
        auto [init, cond, body, back, done] = taskflow.emplace(
            [&](tf::Subflow& subflow) {
                auto init_i = subflow.for_each_index(size_t{0}, parents.size(), size_t{1}, [&](size_t i) {
                    parents[i] = individualInit(rngs[i]);
                    coeffInit(rngs[i], parents[i]);
                }).name("initialize population");
                auto prepareEval = subflow.emplace([&]() { evaluator.Prepare(parents); }).name("prepare evaluator");
                auto eval = subflow.for_each_index(size_t{0}, parents.size(), size_t{1}, [&](size_t i) {
                    auto id = executor.this_worker_id();
                    // make sure the worker has a large enough buffer
                    if (slots[id].size() < trainSize) {
                        slots[id].resize(trainSize);
                    }
                    // TODO: `Fitness` is a 2x1 here!
                    parents[i].Fitness = evaluator(rngs[i], parents[i], slots[id]);
                }).name("evaluate population");
                auto nonDominatedSort = subflow.emplace([&]() { Sort(parents); }).name("non-dominated sort");
                auto reportProgress = subflow.emplace([&]() { if (report) { std::invoke(report); } }).name("report progress");
                init_i.precede(prepareEval);
                prepareEval.precede(eval);
                eval.precede(nonDominatedSort);
                nonDominatedSort.precede(reportProgress);
            }, // init
            stop, // loop condition
            [&](tf::Subflow& subflow) {
                auto prepareGenerator = subflow.emplace([&]() { generator.Prepare(parents); }).name("prepare generator");
                auto generateOffspring = subflow.for_each_index(size_t{0}, offspring.size(), size_t{1}, [&](size_t i) {
                    auto buf = Turingforge::Span<Turingforge::Scalar>(slots[executor.this_worker_id()]);
                    while (!stop()) {
                        auto result = generator(rngs[i], config.CrossoverProbability, config.MutationProbability, config.LocalSearchProbability, buf);
                        if (result) {
                            offspring[i] = std::move(*result);
                            ENSURE(offspring[i].Length > 0);
                            return;
                        }
                    }
                }).name("generate offspring");
                auto nonDominatedSort = subflow.emplace([&]() { Sort(individuals); }).name("non-dominated sort");
                auto reinsert = subflow.emplace([&]() { reinserter.Sort(individuals); }).name("reinsert");
                auto incrementGeneration = subflow.emplace([&]() { ++Generation(); }).name("increment generation");
                auto reportProgress = subflow.emplace([&]() { if (report) { std::invoke(report); } }).name("report progress");

                // set-up subflow graph
                prepareGenerator.precede(generateOffspring);
                generateOffspring.precede(nonDominatedSort);
                nonDominatedSort.precede(reinsert);
                reinsert.precede(incrementGeneration);
                incrementGeneration.precede(reportProgress);
            }, // loop body (evolutionary main loop)
            [&]() { return 0; }, // jump back to the next iteration
            [&]() { /* done nothing to do */ } // work done, report last gen and stop
        ); // evolutionary loop

        init.name("init");
        cond.name("termination");
        body.name("main loop");
        back.name("back");
        done.name("done");
        taskflow.name("NSGA2");

        init.precede(cond);
        cond.precede(body, done);
        body.precede(back);
        back.precede(cond);

        executor.run(taskflow);
        executor.wait_for_all();
    }

    auto NSGA2::Run(RandomGenerator& random, std::function<void()> report, size_t threads) -> void
    {
        if (threads == 0U) {
            threads = std::thread::hardware_concurrency();
        }
        tf::Executor executor(threads);
        Run(executor, random, std::move(report));
    }
} // namespace Turingforge