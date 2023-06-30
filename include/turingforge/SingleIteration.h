#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <random>
#include <chrono>
#include <map>
#include <string>
#include <cassert>

// Import necessary modules and types
#include "DynamicExpressions.hpp"
#include "CoreModule.hpp"
#include "ComplexityModule.hpp"
#include "UtilsModule.hpp"
#include "PopMemberModule.hpp"
#include "PopulationModule.hpp"
#include "HallOfFameModule.hpp"
#include "AdaptiveParsimonyModule.hpp"
#include "RegularizedEvolutionModule.hpp"
#include "ConstantOptimizationModule.hpp"
#include "RecorderModule.hpp"

using namespace std;

// Cycle through regularized evolution many times,
// printing the fittest equation every 10% through
template <typename T, typename L>
tuple<Population<T, L>, HallOfFame<T, L>, double> s_r_cycle(
        const Dataset<T, L>& dataset,
        Population<T, L>& pop,
        int ncycles,
        int curmaxsize,
        RunningSearchStatistics& running_search_statistics,
        int verbosity,
        const Options& options,
        RecordType& record
) {
    double max_temp = 1.0;
    double min_temp = 0.0;
    if (!options.annealing) {
        min_temp = max_temp;
    }
    vector<double> all_temperatures(ncycles);
    linspace(max_temp, min_temp, ncycles, all_temperatures.begin());

    HallOfFame<T, L> best_examples_seen(options);
    double num_evals = 0.0;

    for (double temperature : all_temperatures) {
        auto [tmp_pop, tmp_num_evals] = reg_evol_cycle<T, L>(
                dataset,
                pop,
                temperature,
                curmaxsize,
                running_search_statistics,
                options,
                record
        );
        pop = move(tmp_pop);
        num_evals += tmp_num_evals;

        for (auto& member : pop.members) {
            int size = compute_complexity<T, L>(member, options);
            double score = member.score;
            if (0 < size && size <= options.maxsize && (!best_examples_seen.exists[size] || score < best_examples_seen.members[size].score)) {
                best_examples_seen.exists[size] = true;
                best_examples_seen.members[size] = copy_pop_member<T, L>(member);
            }
        }
    }

    return make_tuple(pop, best_examples_seen, num_evals);
}

template <typename T, typename L>
tuple<Population<T, L>, double> optimize_and_simplify_population(
        const Dataset<T, L>& dataset,
        Population<T, L>& pop,
        const Options& options,
        int curmaxsize,
        RecordType& record
) {
    vector<double> array_num_evals(pop.n);
    vector<bool> do_optimization(pop.n);
    random_device rd;
    mt19937 gen(rd());
    bernoulli_distribution dist(options.optimizer_probability);
    generate(do_optimization.begin(), do_optimization.end(), [&]() { return dist(gen); });

    for (int j = 0; j < pop.n; ++j) {
        if (options.should_simplify) {
            auto& tree = pop.members[j].tree;
            tree = simplify_tree(tree, options.operators);
            tree = combine_operators(tree, options.operators);
            pop.members[j].tree = tree;
        }

        if (options.should_optimize_constants && do_optimization[j]) {
            auto& [tmp_member, tmp_num_evals] = optimize_constants<T, L>(dataset, pop.members[j], options);
            pop.members[j] = move(tmp_member);
            array_num_evals[j] = tmp_num_evals;
        }
    }

    double num_evals = accumulate(array_num_evals.begin(), array_num_evals.end(), 0.0);
    auto [tmp_pop, tmp_num_evals] = finalize_scores<T, L>(dataset, pop, options);
    pop = move(tmp_pop);
    num_evals += tmp_num_evals;

    for (int j = 0; j < pop.n; ++j) {
        auto& member = pop.members[j];
        auto old_ref = member.ref;
        auto new_ref = generate_reference();
        member.parent = old_ref;
        member.ref = new_ref;

        {
            // Same structure as in RegularizedEvolution.jl,
            // except we assume that the record already exists.
            assert(record.count("mutations") > 0);
            if (record["mutations"].count(to_string(member.ref)) == 0) {
                record["mutations"][to_string(member.ref)] = RecordType{
                        {"events", vector<RecordType>{}},
                        {"tree", string_tree(member.tree, options.operators)},
                        {"score", member.score},
                        {"loss", member.loss},
                        {"parent", member.parent}
                };
            }
            RecordType optimize_and_simplify_event{
                    {"type", "tuning"},
                    {"time", chrono::system_clock::now().time_since_epoch().count()},
                    {"child", new_ref},
                    {"mutation", RecordType{
                            {"type", (do_optimization[j] && options.should_optimize_constants) ? "simplification_and_optimization" : "simplification"}
                    }}
            };
            RecordType death_event{
                    {"type", "death"},
                    {"time", chrono::system_clock::now().time_since_epoch().count()}
            };

            record["mutations"][to_string(old_ref)]["events"].push_back(optimize_and_simplify_event);
            record["mutations"][to_string(old_ref)]["events"].push_back(death_event);
        }
    }

    return make_tuple(pop, num_evals);
}

int main() {
    // Your code goes here

    return 0;
}