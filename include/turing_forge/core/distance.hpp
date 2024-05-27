#pragma once

#include "turing_forge/core/types.hpp"

namespace Turingforge::Distance {
    auto Jaccard(Turingforge::Vector<Turingforge::Hash> const& lhs, Turingforge::Vector<Turingforge::Hash> const& rhs) noexcept -> double;
    auto SorensenDice(Turingforge::Vector<Turingforge::Hash> const& lhs, Turingforge::Vector<Turingforge::Hash> const& rhs) noexcept -> double;
} // namespace Turingforge::Distance