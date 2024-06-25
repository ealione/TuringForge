#pragma once

#include "turing_forge/core/individual.hpp"
#include "turing_forge/core/types.hpp"

// aggregate performance statistics such as sort duration
#include <chrono>

namespace Turingforge {

    enum EfficientSortStrategy : int {
        Binary, Sequential
    };

    class NondominatedSorterBase {
    public:
        using Result = std::vector<std::vector<size_t>>;

        mutable struct {
            size_t LexicographicalComparisons{0}; // both lexicographical and single-objective
            size_t SingleValueComparisons{0};
            size_t DominanceComparisons{0};
            size_t RankComparisons{0};
            size_t InnerOps{0};
            double MeanRank{0};
            double MeanND{0};
            std::chrono::duration<size_t> Duration{0};
        } Stats;

        void Reset() { Stats = {0, 0, 0, 0, 0, 0., 0.}; }

        // this method needs to be implemented by all deriving classes
        virtual auto Sort(Turingforge::Span<Turingforge::Individual const>, Turingforge::Scalar) const -> Result = 0;

        auto
        operator()(Turingforge::Span<Turingforge::Individual const> pop, Turingforge::Scalar eps = 0) const -> Result {
            return Sort(pop, eps);
        }
    };

    struct DeductiveSorter : public NondominatedSorterBase {
        auto Sort(Turingforge::Span<Turingforge::Individual const> pop,
                  Turingforge::Scalar eps) const -> NondominatedSorterBase::Result override;
    };

    struct DominanceDegreeSorter : public NondominatedSorterBase {
        auto Sort(Turingforge::Span<Turingforge::Individual const> pop,
                  Turingforge::Scalar eps) const -> NondominatedSorterBase::Result override;
    };

    struct HierarchicalSorter : public NondominatedSorterBase {
        auto Sort(Turingforge::Span<Turingforge::Individual const> pop,
                  Turingforge::Scalar eps) const -> NondominatedSorterBase::Result override;
    };

    struct EfficientBinarySorter : public NondominatedSorterBase {
        auto Sort(Turingforge::Span<Turingforge::Individual const> pop,
                  Turingforge::Scalar eps) const -> NondominatedSorterBase::Result override;
    };

    struct EfficientSequentialSorter : public NondominatedSorterBase {
        auto Sort(Turingforge::Span<Turingforge::Individual const> pop,
                  Turingforge::Scalar eps) const -> NondominatedSorterBase::Result override;
    };

    struct MergeSorter : public NondominatedSorterBase {
        auto Sort(Turingforge::Span<Turingforge::Individual const> pop,
                  Turingforge::Scalar eps) const -> NondominatedSorterBase::Result override;
    };

    struct RankOrdinalSorter : public NondominatedSorterBase {
        auto Sort(Turingforge::Span<Turingforge::Individual const> pop,
                  Turingforge::Scalar eps) const -> NondominatedSorterBase::Result override;
    };

    struct RankIntersectSorter : public NondominatedSorterBase {
        auto Sort(Turingforge::Span<Turingforge::Individual const> pop,
                  Turingforge::Scalar eps) const -> NondominatedSorterBase::Result override;
    };

    struct BestOrderSorter : public NondominatedSorterBase {
        auto Sort(Turingforge::Span<Turingforge::Individual const> pop,
                  Turingforge::Scalar eps) const -> NondominatedSorterBase::Result override;
    };

} // namespace Turingforge