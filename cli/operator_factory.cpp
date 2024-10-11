#include "operator_factory.hpp"
#include <stdexcept>                            // for runtime_error
#include <fmt/format.h>                         // for format
#include <scn/scan.h>

#include "turing_forge/interpreter/dispatch_table.hpp"
#include "turing_forge/operators/creator.hpp"    // for CreatorBase, BalancedTreeC...
#include "turing_forge/operators/evaluator.hpp"  // for Evaluator, EvaluatorBase
#include "turing_forge/operators/generator.hpp"  // for OffspringGeneratorBase
#include "turing_forge/operators/reinserter.hpp"  // for OffspringGeneratorBase
#include "turing_forge/operators/selector.hpp"
#include "turing_forge/operators/local_search.hpp"
#include "turing_forge/optimizer/optimizer.hpp"

#include <cxxopts.hpp>


namespace Turingforge { class PrimitiveSet; }
namespace Turingforge { class Problem; }
namespace Turingforge { struct CrossoverBase; }
namespace Turingforge { struct MutatorBase; }
namespace Turingforge { struct Variable; }

namespace Turingforge {

    namespace detail {
        auto GetErrorString(std::string const& name, std::string const& arg) {
            return fmt::format("unable to parse {} argument '{}'", name, arg);
        }
    } // namespace detail

    auto ParseReinserter(std::string const& str, ComparisonCallback&& comp) -> std::unique_ptr<ReinserterBase>
    {
        std::unique_ptr<ReinserterBase> reinserter;
        if (str == "keep-best") {
            reinserter = std::make_unique<KeepBestReinserter>(std::move(comp));
        } else if (str == "replace-worst") {
            reinserter = std::make_unique<ReplaceWorstReinserter>(std::move(comp));
        } else {
            throw std::invalid_argument(detail::GetErrorString("reinserter", str));
        }
        return reinserter;
    }

    auto ParseSelector(std::string const& str, ComparisonCallback&& comp) -> std::unique_ptr<Turingforge::SelectorBase>
    {
        auto tok = Split(str, ':');
        auto name = tok[0];
        std::unique_ptr<Turingforge::SelectorBase> selector;
        constexpr size_t defaultTournamentSize{5};
        if (name == "tournament") {
            selector = std::make_unique<Turingforge::TournamentSelector>(std::move(comp));
            size_t tournamentSize{defaultTournamentSize};
            if (tok.size() > 1) {
                auto result = scn::scan<std::size_t>(tok[1], "{}");
                        ENSURE(result);
                tournamentSize = result->value();
            }
            dynamic_cast<Turingforge::TournamentSelector*>(selector.get())->SetTournamentSize(tournamentSize);
        } else if (name == "proportional") {
            selector = std::make_unique<Turingforge::ProportionalSelector>(std::move(comp));
            dynamic_cast<Turingforge::ProportionalSelector*>(selector.get())->SetObjIndex(0);
        } else if (name == "rank") {
            selector = std::make_unique<Turingforge::RankTournamentSelector>(std::move(comp));
            size_t tournamentSize{defaultTournamentSize};
            if (tok.size() > 1) {
                auto result = scn::scan<std::size_t>(tok[1], "{}");
                        ENSURE(result);
                tournamentSize = result->value();
            }
            dynamic_cast<Turingforge::RankTournamentSelector*>(selector.get())->SetTournamentSize(tournamentSize);
        } else if (name == "random") {
            selector = std::make_unique<Turingforge::RandomSelector>();
        } else {
            throw std::invalid_argument(detail::GetErrorString("selector", str));
        }

        return selector;
    }

    auto ParseCreator(std::string const& str, PrimitiveSet const& pset, std::vector<Turingforge::Hash> const& inputs) -> std::unique_ptr<CreatorBase>
    {
        std::unique_ptr<CreatorBase> creator;

        auto tok = Split(str, ':');
        auto name = tok[0];

        double bias{0}; // irregularity bias (used by btc and ptc20)
        if(tok.size() > 1) {
            auto res = scn::scan<double>(tok[1], "{}");
                    ENSURE(res);
            bias = res->value();
        }

        if (str == "btc") {
            creator = std::make_unique<BalancedCreator>(pset, inputs, bias);
        } else {
            throw std::invalid_argument(detail::GetErrorString("creator", str));
        }
        return creator;
    }

    auto ParseEvaluator(std::string const& str, Problem& problem, DefaultDispatch& dtable, bool scale) -> std::unique_ptr<EvaluatorBase>
    {
        using T = DefaultDispatch;

        std::unique_ptr<EvaluatorBase> evaluator;
        if (str == "r2") {
            evaluator = std::make_unique<Turingforge::Evaluator<T>>(problem, dtable, Turingforge::R2{}, scale);
        } else if (str == "c2") {
            evaluator = std::make_unique<Turingforge::Evaluator<T>>(problem, dtable, Turingforge::C2{}, scale);
        } else if (str == "nmse") {
            evaluator = std::make_unique<Turingforge::Evaluator<T>>(problem, dtable, Turingforge::NMSE{}, scale);
        } else if (str == "mse") {
            evaluator = std::make_unique<Turingforge::Evaluator<T>>(problem, dtable, Turingforge::MSE{}, scale);
        } else if (str == "rmse") {
            evaluator = std::make_unique<Turingforge::Evaluator<T>>(problem, dtable, Turingforge::RMSE{}, scale);
        } else if (str == "mae") {
            evaluator = std::make_unique<Turingforge::Evaluator<T>>(problem, dtable, Turingforge::MAE{}, scale);
        } else if (str == "gauss") {
            evaluator = std::make_unique<Turingforge::GaussianLikelihoodEvaluator<T>>(problem, dtable);
        } else {
            throw std::runtime_error(fmt::format("unable to parse evaluator metric '{}'\n", str));
        }
        return evaluator;
    }

    auto ParseGenerator(std::string const& str, EvaluatorBase& eval, CrossoverBase& cx, MutatorBase& mut, SelectorBase& femSel, SelectorBase& maleSel, CoefficientOptimizer const* coeffOptimizer = nullptr) -> std::unique_ptr<OffspringGeneratorBase>
    {
        std::unique_ptr<OffspringGeneratorBase> generator;
        auto tok = Split(str, ':');
        auto name = tok[0];
        if (name == "basic") {
            generator = std::make_unique<BasicOffspringGenerator>(eval, cx, mut, femSel, maleSel, coeffOptimizer);
        } else if (name == "os") {
            size_t maxSelectionPressure{100};
            double comparisonFactor{0};
            if (tok.size() > 1) {
                maxSelectionPressure = scn::scan<size_t>(tok[1], "{}")->value();
            }
            if (tok.size() > 2) {
                comparisonFactor = scn::scan<double>(tok[2], "{}")->value();
            }
            generator = std::make_unique<OffspringSelectionGenerator>(eval, cx, mut, femSel, maleSel, coeffOptimizer);
            dynamic_cast<OffspringSelectionGenerator*>(generator.get())->MaxSelectionPressure(maxSelectionPressure);
            dynamic_cast<OffspringSelectionGenerator*>(generator.get())->ComparisonFactor(comparisonFactor);
        } else if (name == "brood") {
            generator = std::make_unique<BroodOffspringGenerator>(eval, cx, mut, femSel, maleSel, coeffOptimizer);
            size_t broodSize{BroodOffspringGenerator::DefaultBroodSize};
            if (tok.size() > 1) { broodSize = scn::scan<size_t>(tok[1], "{}")->value(); }
            dynamic_cast<BroodOffspringGenerator*>(generator.get())->BroodSize(broodSize);
        } else if (name == "poly") {
            generator = std::make_unique<PolygenicOffspringGenerator>(eval, cx, mut, femSel, maleSel, coeffOptimizer);
            size_t polygenicSize{PolygenicOffspringGenerator::DefaultBroodSize};
            if (tok.size() > 1) { polygenicSize = scn::scan<size_t>(tok[1], "{}")->value(); }
            dynamic_cast<PolygenicOffspringGenerator*>(generator.get())->PolygenicSize(polygenicSize);
        } else {
            throw std::invalid_argument(detail::GetErrorString("generator", str));
        }
        return generator;
    }

    auto ParseOptimizer(std::string const& /*str*/, Problem const& /*problem*/, DefaultDispatch const& /*dtable*/) -> std::unique_ptr<OptimizerBase> {
        throw std::runtime_error("not implemented");
    }

} // namespace Turingforge