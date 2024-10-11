#include <string>


#include "turing_forge/core/dataset.hpp"
#include "turing_forge/core/types.hpp"
#include "turing_forge/formatter/formatter.hpp"
#include "turing_forge/interpreter/dispatch_table.hpp"
#include "turing_forge/optimizer/likelihood/gaussian_likelihood.hpp"
#include "turing_forge/interpreter/interpreter.hpp"
#include "turing_forge/operators/evaluator.hpp"
#include "util.hpp"

#include <cxxopts.hpp>
#include <fmt/core.h>
#include <scn/scan.h>

auto main(int argc, char** argv) -> int
{
    cxxopts::Options opts("operon_parse_model", "Parse and evaluate a model in infix form");

    opts.add_options()
            ("dataset", "Dataset file name (csv) (required)", cxxopts::value<std::string>())
            ("target", "Name of the target variable (if none provided, model output will be printed)", cxxopts::value<std::string>())
            ("range", "Data range [A:B)", cxxopts::value<std::string>())
            ("scale", "Linear scaling slope:intercept", cxxopts::value<std::string>())
            ("debug", "Show some debugging information", cxxopts::value<bool>()->default_value("false"))
            ("format", "Format string (see https://fmt.dev/latest/syntax.html)", cxxopts::value<std::string>()->default_value(":>#8.4g"))
            ("help", "Print help");

    opts.allow_unrecognised_options();

    cxxopts::ParseResult result;
    try {
        result = opts.parse(argc, argv);
    } catch (cxxopts::exceptions::parsing const& ex) {
        fmt::print(stderr, "error: {}. rerun with --help to see available options.\n", ex.what());
        return EXIT_FAILURE;
    };

    if (result.arguments().empty() || result.count("help") > 0) {
        fmt::print("{}\n", opts.help());
        return EXIT_SUCCESS;
    }

    if (result.count("dataset") == 0) {
        fmt::print(stderr, "error: no dataset was specified.\n");
        return EXIT_FAILURE;
    }

    if (result.unmatched().empty()) {
        fmt::print(stderr, "error: no infix string was provided.\n");
        return EXIT_FAILURE;
    }

    Turingforge::Dataset ds(result["dataset"].as<std::string>(), /*hasHeader=*/true);
    auto infix = result.unmatched().front();
    Turingforge::Map<std::string, Turingforge::Hash> vars;
    for (auto const& v : ds.GetVariables()) {
        vars.insert({ v.Name, v.Hash });
    }
    auto model = Turingforge::InfixParser::Parse(infix, vars);

    Turingforge::DefaultDispatch dtable;
    Turingforge::Range range{0, ds.Rows<std::size_t>()};
    if (result["range"].count() > 0) {
        auto res = scn::scan<std::size_t, std::size_t>(result["range"].as<std::string>(), "{}:{}");
        ENSURE(res);
        auto [a, b] = res->values();
        range = Turingforge::Range{a, b};
    }

    int constexpr defaultPrecision{6};
    if (result["debug"].as<bool>()) {
        fmt::print("\nInput string:\n{}\n", infix);
        fmt::print("Parsed tree:\n{}\n", Turingforge::InfixFormatter::Format(model, ds, defaultPrecision));
        fmt::print("Data range: {}:{}\n", range.Start(), range.End());
        fmt::print("Scale: {}\n", result["scale"].count() > 0 ? result["scale"].as<std::string>() : std::string("auto"));
    }

    auto est = Turingforge::Interpreter<Turingforge::Scalar, decltype(dtable)>::Evaluate(model, ds, range);

    std::string format = result["format"].as<std::string>();
    if (result["target"].count() > 0) {
        auto tgt = ds.GetValues(result["target"].as<std::string>()).subspan(range.Start(), range.Size());

        Turingforge::Scalar a{0};
        Turingforge::Scalar b{0};
        if (result["scale"].count() > 0) {
            auto res = scn::scan<Turingforge::Scalar, Turingforge::Scalar>(result["scale"].as<std::string>(), "{}:{}");
            ENSURE(res);
            a = std::get<0>(res->values());
            b = std::get<1>(res->values());
        } else {
            auto [a_, b_] = Turingforge::FitLeastSquares(est, tgt);
            a = static_cast<Turingforge::Scalar>(a_);
            b = static_cast<Turingforge::Scalar>(b_);
        }

        std::transform(est.begin(), est.end(), est.begin(), [&](auto v) { return v * a + b; });
        auto r2 = -Turingforge::R2{}(Turingforge::Span<Turingforge::Scalar>{est}, tgt);
        auto rs = -Turingforge::C2{}(Turingforge::Span<Turingforge::Scalar>{est}, tgt);
        auto mae = Turingforge::MAE{}(Turingforge::Span<Turingforge::Scalar>{est}, tgt);
        auto mse = Turingforge::MSE{}(Turingforge::Span<Turingforge::Scalar>{est}, tgt);
        auto rmse = Turingforge::RMSE{}(Turingforge::Span<Turingforge::Scalar>{est}, tgt);
        auto nmse = Turingforge::NMSE{}(Turingforge::Span<Turingforge::Scalar>{est}, tgt);

        Turingforge::Problem problem(ds, range, range);
        Turingforge::RandomGenerator rng{0};
        Turingforge::Individual ind;
        ind.Genotype = model;

        Turingforge::Interpreter<Turingforge::Scalar, Turingforge::DefaultDispatch> interpreter{dtable, ds, ind.Genotype};
        auto target = problem.TargetValues(range);
        Turingforge::GaussianLikelihood lik{rng, interpreter, target, range};
        // Turingforge::MinimumDescriptionLengthEvaluator<Turingforge::DefaultDispatch> mdlEval{problem, dtable, lik};
        // auto mdl = mdlEval(rng, ind, {}).front();

        auto mdl = 0;

        std::vector<std::tuple<std::string, double, std::string>> stats{
                {"slope", a, format},
                {"intercept", b, format},
                {"r2", r2, format},
                {"rs", rs, format},
                {"mae", mae, format},
                {"mse", mse, format},
                {"rmse", rmse, format},
                {"nmse", nmse, format},
                {"mdl", mdl, format}
        };
        Turingforge::PrintStats(stats);
    } else {
        std::string out{};
        for (auto v : est) {
            fmt::format_to(std::back_inserter(out), fmt::runtime(fmt::format("{{{}}}\n", format)), v);
        }
        fmt::print("{}", out);
    }

    return EXIT_SUCCESS;
}