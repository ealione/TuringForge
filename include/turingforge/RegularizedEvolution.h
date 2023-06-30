#include <algorithm>
#include <array>
#include <cmath>
#include <functional>
#include <numeric>
#include <random>
#include <tuple>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <cassert>

// Forward declarations
template<typename T, typename L>
class Dataset;

template<typename T, typename L>
class Population;

template<typename T, typename L>
class PopMember;

class RunningSearchStatistics;

struct Options;

template<typename T, typename L>
class RecordType;

enum class DATA_TYPE;
enum class LOSS_TYPE;

// Helper function to shuffle vector elements
template<typename T>
void shuffle_vector(std::vector<T>& vec)
{
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(vec.begin(), vec.end(), g);
}

template<typename T>
T argmin_fast(const std::vector<T>& vec)
{
    return std::distance(vec.begin(), std::min_element(vec.begin(), vec.end()));
}

template<typename T, typename L>
std::tuple<Population<T, L>, double> reg_evol_cycle(
        const Dataset<T, L>& dataset,
        Population<T, L> pop,
        double temperature,
        int curmaxsize,
        const RunningSearchStatistics& running_search_statistics,
        const Options& options,
        const RecordType<T, L>& record
)
{
    if (options.crossover_probability > 0.0)
    {
        throw std::runtime_error("You cannot have the recorder on when using crossover");
    }

    double num_evals = 0.0;
    int n_evol_cycles = std::ceil(pop.n / options.tournament_selection_n);

    if (options.fast_cycle)
    {
        throw std::runtime_error(
                "You cannot have the recorder and fast_cycle set to true at the same time!"
        );
        assert(options.prob_pick_first == 1.0);
        assert(options.crossover_probability == 0.0);

        shuffle_vector(pop.members);
        std::vector<PopMember<T, L>> babies(n_evol_cycles);
        std::vector<bool> accepted(n_evol_cycles);
        std::vector<double> array_num_evals(n_evol_cycles);

#pragma omp parallel for
        for (int i = 0; i < n_evol_cycles; ++i)
        {
            double best_score = std::numeric_limits<double>::infinity();
            int best_idx = 1 + (i - 1) * options.tournament_selection_n;

            // Calculate best member of the subsample
            for (int sub_i = (1 + (i - 1) * options.tournament_selection_n); sub_i <= (i * options.tournament_selection_n); ++sub_i)
            {
                if (pop.members[sub_i].score < best_score)
                {
                    best_score = pop.members[sub_i].score;
                    best_idx = sub_i;
                }
            }

            const PopMember<T, L>& allstar = pop.members[best_idx];
            RecordType<T, L> mutation_recorder;
            babies[i], accepted[i], array_num_evals[i] = next_generation(
                    dataset,
                    allstar,
                    temperature,
                    curmaxsize,
                    running_search_statistics,
                    options,
                    mutation_recorder
            );
        }

        num_evals = std::accumulate(array_num_evals.begin(), array_num_evals.end(), 0.0);

        // Replace the n_evol_cycles-oldest members of each population
        for (int i = 0; i < n_evol_cycles; ++i)
        {
            int oldest = argmin_fast(std::vector<double>(pop.n, [&](int member) { return pop.members[member].birth; }));
            if (accepted[i] || !options.skip_mutation_failures)
            {
                pop.members[oldest] = babies[i];
            }
        }
    }
    else
    {
        for (int i = 0; i < n_evol_cycles; ++i)
        {
            if (rand() > options.crossover_probability)
            {
                const PopMember<T, L>& allstar = best_of_sample(pop, running_search_statistics, options);
                RecordType<T, L> mutation_recorder;
                PopMember<T, L> baby;
                bool mutation_accepted;
                double tmp_num_evals;
                std::tie(baby, mutation_accepted, tmp_num_evals) = next_generation(
                        dataset,
                        allstar,
                        temperature,
                        curmaxsize,
                        running_search_statistics,
                        options,
                        mutation_recorder
                );
                num_evals += tmp_num_evals;

                if (!mutation_accepted && options.skip_mutation_failures)
                {
                    continue;
                }

                int oldest = argmin_fast(std::vector<double>(pop.n, [&](int member) { return pop.members[member].birth; }));

                // Recorder
                if (!record.mutations.count(allstar.ref))
                {
                    record.mutations[allstar.ref] = RecordType<T, L>();
                }
                if (!record.mutations.count(baby.ref))
                {
                    record.mutations[baby.ref] = RecordType<T, L>();
                }
                if (!record.mutations.count(pop.members[oldest].ref))
                {
                    record.mutations[pop.members[oldest].ref] = RecordType<T, L>();
                }

                RecordType<T, L> mutate_event;
                mutate_event["type"] = "mutate";
                mutate_event["time"] = std::time(nullptr);
                mutate_event["child"] = baby.ref;
                mutate_event["mutation"] = mutation_recorder;

                RecordType<T, L> death_event;
                death_event["type"] = "death";
                death_event["time"] = std::time(nullptr);

                record.mutations[allstar.ref]["events"].push_back(mutate_event);
                record.mutations[pop.members[oldest].ref]["events"].push_back(death_event);

                pop.members[oldest] = baby;
            }
            else // Crossover
            {
                const PopMember<T, L>& allstar1 = best_of_sample(pop, running_search_statistics, options);
                const PopMember<T, L>& allstar2 = best_of_sample(pop, running_search_statistics, options);

                PopMember<T, L> baby1, baby2;
                bool crossover_accepted;
                double tmp_num_evals;
                std::tie(baby1, baby2, crossover_accepted, tmp_num_evals) = crossover_generation(
                        allstar1, allstar2, dataset, curmaxsize, options
                );
                num_evals += tmp_num_evals;

                if (!crossover_accepted && options.skip_mutation_failures)
                {
                    continue;
                }

                // Replace old members with new ones
                int oldest = argmin_fast(std::vector<double>(pop.n, [&](int member) { return pop.members[member].birth; }));
                pop.members[oldest] = baby1;
                oldest = argmin_fast(std::vector<double>(pop.n, [&](int member) { return pop.members[member].birth; }));
                pop.members[oldest] = baby2;
            }
        }
    }

    return std::make_tuple(pop, num_evals);
}