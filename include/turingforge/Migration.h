#pragma once

#include <algorithm>
#include <random>
#include <vector>

template <typename T, typename L>
size_t poisson_sample(double mean) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::poisson_distribution<size_t> dist(mean);
    return dist(gen);
}

template <typename T, typename L>
void migrate(
        std::pair<std::vector<PopMember<T, L>>, Population<T, L>>& migration,
        const Options& options,
        double frac
) {
    auto base_pop = migration.second;
    auto npop = base_pop.members.size();
    auto mean_number_replaced = npop * frac;
    auto num_replace = poisson_sample(mean_number_replaced);

    auto migrant_candidates = migration.first;

    num_replace = std::min(num_replace, migrant_candidates.size());
    num_replace = std::min(num_replace, npop);

    std::vector<size_t> locations(num_replace);
    std::vector<PopMember<T, L>> migrants(num_replace);

    std::sample(
            migrant_candidates.begin(), migrant_candidates.end(),
            migrants.begin(), num_replace,
            std::mt19937{std::random_device{}()}
    );

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dist(0, npop - 1);

    for (size_t i = 0; i < num_replace; i++) {
    size_t loc = dist(gen);
    base_pop.members[loc] = copy_pop_member_reset_birth(
            migrants[i], options.deterministic);
    }
}