#pragma once

#include <cstddef>                              // for size_t
#include <memory>                               // for unique_ptr, make_unique
#include <string>                               // for operator==, string
#include <utility>                              // for addressof
#include <vector>                               // for vector

#include "turing_forge/core/types.hpp"                  // for Span
#include "turing_forge/core/individual.hpp"             // for Comparison
#include "turing_forge/interpreter/dispatch_table.hpp"
#include "turing_forge/interpreter/interpreter.hpp"     // for Interpreter
#include "turing_forge/optimizer/optimizer.hpp"
#include "util.hpp"                                     // for Split

namespace Turingforge { struct EvaluatorBase; }
namespace Turingforge { class KeepBestReinserter; }
namespace Turingforge { class OffspringGeneratorBase; }
namespace Turingforge { class PrimitiveSet; }
namespace Turingforge { class Problem; }
namespace Turingforge { class ReinserterBase; }
namespace Turingforge { class ReplaceWorstReinserter; }
namespace Turingforge { class SelectorBase; }
namespace Turingforge { struct CreatorBase; }
namespace Turingforge { struct CrossoverBase; }
namespace Turingforge { struct ErrorMetric; }
namespace Turingforge { class CoefficientOptimizer; }
namespace Turingforge { struct MutatorBase; }
namespace Turingforge { struct Variable; }

namespace Turingforge {

    auto ParseReinserter(std::string const& str, ComparisonCallback&& comp) -> std::unique_ptr<ReinserterBase>;

    auto ParseSelector(std::string const& str, ComparisonCallback&& comp) -> std::unique_ptr<SelectorBase>;

    auto ParseCreator(std::string const& str, PrimitiveSet const& pset, std::vector<Turingforge::Hash> const& inputs) -> std::unique_ptr<CreatorBase>;

    auto ParseEvaluator(std::string const& str, Problem& problem, DefaultDispatch& dtable, bool scale = true) -> std::unique_ptr<EvaluatorBase>;

    auto ParseErrorMetric(std::string const& str) -> std::tuple<std::unique_ptr<Turingforge::ErrorMetric>, bool>;

    auto ParseGenerator(std::string const& str, EvaluatorBase& eval, CrossoverBase& cx, MutatorBase& mut, SelectorBase& femSel, SelectorBase& maleSel, CoefficientOptimizer const* cOpt) -> std::unique_ptr<OffspringGeneratorBase>;

    auto ParseOptimizer(std::string const& str, Problem const& problem, DefaultDispatch const& dtable) -> std::unique_ptr<OptimizerBase>;

} // namespace Turingforge