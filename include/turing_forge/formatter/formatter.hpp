#pragma once

#include <unordered_map>

#include "turing_forge/core/individual.hpp"
#include "fmt/format.h"

namespace Turingforge {

    class Dataset;
    
    class IndividualFormatter {
            static auto FormatNode(Individual const& individual, Turingforge::Map<Turingforge::Hash, std::string> const& variableNames, size_t i, fmt::memory_buffer& current, int decimalPrecision) -> void;

            public:
            static auto Format(Individual const& individual, Dataset const& dataset, int decimalPrecision = 2) -> std::string;
            static auto Format(Individual const& individual, Turingforge::Map<Turingforge::Hash, std::string> const& variableNames, int decimalPrecision = 2) -> std::string;
    };
}