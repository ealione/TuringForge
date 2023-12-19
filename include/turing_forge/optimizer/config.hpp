#pragma once

#include <cstddef>


namespace Turingforge {
    struct SimulatedAnnealingConfig {
        size_t Generations; // generation limit
        size_t Evaluations; // evaluation budget
        size_t Iterations;  // local search iterations
        size_t PopulationSize;
        size_t PoolSize;
        size_t Seed;        // random seed
        size_t TimeLimit;   // time limit
        double CrossoverProbability;
        double MutationProbability;
        double Epsilon;     // used when comparing fitness values
    };
} // namespace Turingforge