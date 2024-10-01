#pragma once

#include "turing_forge/core/individual.hpp"
#include "turing_forge/operators/non_dominated_sorter.hpp"

namespace Turingforge {
    class SolutionArchive {
    public:
        auto Insert(Turingforge::Individual const& individual) -> bool;
        auto Insert(Turingforge::Span<Turingforge::Individual const> individuals) -> int64_t;

        [[nodiscard]] auto Solutions() const { return Turingforge::Span<Turingforge::Individual const> { archive_ }; }
        auto Clear() { archive_.clear(); }
    private:
        Turingforge::Scalar eps_{};
        std::vector<Turingforge::Individual> archive_;
    };
} // namespace Turingforge