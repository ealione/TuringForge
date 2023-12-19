#pragma once

#include <optional>

#include "turing_forge/core/types.hpp"
#include "turing_forge/core/operator.hpp"
#include "turing_forge/core/individual.hpp"


namespace Turingforge::Operators {

    class OffspringGeneratorBase: public OperatorBase<std::optional<Turingforge::Individual>, /* crossover prob. */ double, /* mutation prob. */ double, /* memory buffer */ Turingforge::Span<Turingforge::Scalar>> {
    public:
        OffspringGeneratorBase(EvaluatorBase& eval, CrossoverBase& cx, MutatorBase& mut, SelectorBase& femSel, SelectorBase& maleSel)
                : evaluator_(eval)
                , crossover_(cx)
                , mutator_(mut)
                , femaleSelector_(femSel)
                , maleSelector_(maleSel)
        {
        }
    };

}