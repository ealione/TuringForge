#include <vector>
#include <random>
#include <algorithm>
#include <functional>
#include <cmath>

#include "StatsBase.h"  // assuming StatsBase library is included
#include "DynamicExpressions.h"  // assuming DynamicExpressions library is included
#include "../CoreModule/Options.h"  // assuming Options class is defined
#include "../CoreModule/Dataset.h"  // assuming Dataset class is defined
#include "../CoreModule/RecordType.h"  // assuming RecordType class is defined
#include "../CoreModule/DataTypes.h"  // assuming DATA_TYPE and LOSS_TYPE are defined
#include "../ComplexityModule/ComplexityFunctions.h"  // assuming compute_complexity function is defined
#include "../LossFunctionsModule/LossFunctions.h"  // assuming score_func and update_baseline_loss! functions are defined
#include "../AdaptiveParsimonyModule/RunningSearchStatistics.h"  // assuming RunningSearchStatistics class is defined
#include "../MutationFunctionsModule/MutationFunctions.h"  // assuming gen_random_tree function is defined
#include "../PopMemberModule/PopMember.h"  // assuming PopMember class is defined
#include "../UtilsModule/Utils.h"  // assuming bottomk_fast and argmin_fast functions are defined

template <typename T, typename L>
class Population {
public:
    Population(const std::vector<PopMember<T, L>>& members)
            : members(members), n(members.size()) {}

    Population(const Dataset<T, L>& dataset, int npop, int nlength = 3, const Options& options, int nfeatures)
            : n(npop) {
        members.reserve(npop);
        for (int i = 0; i < npop; ++i) {
            members.emplace_back(
                    dataset,
                    gen_random_tree(nlength, options, nfeatures),
                    options,
                    -1,
                    options.deterministic
            );
        }
    }

    Population(const Matrix<T>& X, const std::vector<T>& y, int npop, int nlength = 3, const Options& options, int nfeatures, std::optional<LOSS_TYPE> loss_type = std::nullopt)
            : Population(Dataset<T, L>(X, y, loss_type), npop, nlength, options, nfeatures) {}

    Population<T, L> copy_population() const {
        std::vector<PopMember<T, L>> copied_members;
        copied_members.reserve(members.size());
        for (const auto& member : members) {
            copied_members.push_back(member.copy_pop_member());
        }
        return Population<T, L>(copied_members);
    }

    Population<T, L> sample_pop(const Options& options) const {
        std::vector<int> indices(members.size());
        std::iota(indices.begin(), indices.end(), 0);
        std::shuffle(indices.begin(), indices.end(), std::mt19937(std::random_device{}()));
        indices.resize(options.tournament_selection_n);
        std::vector<PopMember<T, L>> sampled_members;
        sampled_members.reserve(indices.size());
        for (int index : indices) {
            sampled_members.push_back(members[index]);
        }
        return Population<T, L>(sampled_members);
    }

    PopMember<T, L> best_of_sample(const RunningSearchStatistics& running_search_statistics, const Options& options) const {
        Population<T, L> sample = sample_pop(options);
        return _best_of_sample(sample.members, running_search_statistics, options);
    }

    PopMember<T, L> _best_of_sample(const std::vector<PopMember<T, L>>& members, const RunningSearchStatistics& running_search_statistics, const Options& options) const {
        double p = options.tournament_selection_p;
        int n = members.size();
        std::vector<L> scores(n);

        if (options.use_frequency_in_tournament) {
            L adaptive_parsimony_scaling = L(options.adaptive_parsimony_scaling);
            for (int i = 0; i < n; ++i) {
                const auto& member = members[i];
                int size = compute_complexity(member, options);
                L frequency = (0 < size && size <= options.maxsize) ? L(running_search_statistics.normalized_frequencies[size]) : L(0);
                scores[i] = member.score * std::exp(adaptive_parsimony_scaling * frequency);
            }
        } else {
            std::transform(members.begin(), members.end(), scores.begin(), [](const auto& member) {
                return member.score;
            });
        }

        int chosen_idx;
        if (p == 1.0) {
            chosen_idx = argmin_fast(scores);
        } else {
            int tournament_winner = StatsBase::sample(options.tournament_selection_weights);
            if (tournament_winner == 1) {
                chosen_idx = argmin_fast(scores);
            } else {
                auto bottomk = bottomk_fast(scores, tournament_winner);
                chosen_idx = bottomk[1][n - 1];
            }
        }

        return members[chosen_idx];
    }

    std::tuple<Population<T, L>, double> finalize_scores(const Dataset<T, L>& dataset, const Options& options) const {
        bool need_recalculate = options.batching;
        double num_evals = 0.0;

        if (need_recalculate) {
            for (auto& member : members) {
                auto [score, loss] = score_func(dataset, member, options);
                member.score = score;
                member.loss = loss;
            }
            num_evals += n;
        }

        return { *this, num_evals };
    }

    Population<T, L> best_sub_pop(int topn = 10) const {
        std::vector<L> scores(n);
        std::transform(members.begin(), members.end(), scores.begin(), [](const auto& member) {
            return member.score;
        });
        std::vector<int> best_idx(n);
        std::iota(best_idx.begin(), best_idx.end(), 0);
        std::partial_sort(best_idx.begin(), best_idx.begin() + topn, best_idx.end(), [&scores](int a, int b) {
            return scores[a] < scores[b];
        });
        std::vector<PopMember<T, L>> best_members(topn);
        for (int i = 0; i < topn; ++i) {
            best_members[i] = members[best_idx[i]];
        }
        return Population<T, L>(best_members);
    }

    RecordType record_population(const Options& options) const {
        std::vector<RecordType> pop_records;
        pop_records.reserve(n);
        for (const auto& member : members) {
            pop_records.push_back({
                                          { "tree", string_tree(member.tree, options.operators) },
                                          { "loss", member.loss },
                                          { "score", member.score },
                                          { "complexity", compute_complexity(member, options) },
                                          { "birth", member.birth },
                                          { "ref", member.ref },
                                          { "parent", member.parent }
                                  });
        }
        return { { "population", pop_records }, { "time", time() } };
    }

private:
    std::vector<PopMember<T, L>> members;
    int n;
};