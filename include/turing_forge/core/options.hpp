#include <cxxopts.hpp>

#include "version.hpp"


namespace Turingforge {

    auto InitOptions(std::string const &name, std::string const &desc, int width = 200) -> cxxopts::Options {
        cxxopts::Options opts(name, desc);
        opts.set_width(width);

        std::string const symbols = "add, sub, mul, div, exp, log, square, sqrt, cbrt, sin, cos, tan, asin, acos, atan, sinh, cosh, tanh, abs, aq, ceil, floor, fmin, fmax, log1p, logabs, sqrtabs";

        opts.add_options()
                ("dataset", "Dataset file name (csv) (required)", cxxopts::value<std::string>())
                ("shuffle", "Shuffle the input data", cxxopts::value<bool>()->default_value("false"))
                ("standardize", "Standardize the training partition (zero mean, unit variance)",
                 cxxopts::value<bool>()->default_value("false"))
                ("train", "Training range specified as start:end (required)", cxxopts::value<std::string>())
                ("test", "Test range specified as start:end", cxxopts::value<std::string>())
                ("target", "Name of the target variable (required)", cxxopts::value<std::string>())
                ("inputs", "Comma-separated list of input variables", cxxopts::value<std::string>())
                ("epsilon", "Tolerance for fitness comparison (needed e.g. for eps-dominance)",
                 cxxopts::value<Turingforge::Scalar>()->default_value("1e-6"))
                ("error-metric", "The error metric used for calculating fitness",
                 cxxopts::value<std::string>()->default_value("r2"))
                ("population-size", "Population size", cxxopts::value<size_t>()->default_value("1000"))
                ("pool-size", "Recombination pool size (how many generated offspring per generation)",
                 cxxopts::value<size_t>()->default_value("1000"))
                ("seed", "Random number seed",
                 cxxopts::value<Turingforge::RandomGenerator::result_type>()->default_value("0"))
                ("generations", "Number of generations", cxxopts::value<size_t>()->default_value("1000"))
                ("iterations", "Local optimization iterations", cxxopts::value<size_t>()->default_value("0"))
                ("selection-pressure", "Selection pressure", cxxopts::value<size_t>()->default_value("100"))
                ("maxlength", "Maximum length", cxxopts::value<size_t>()->default_value("50"))
                ("maxdepth", "Maximum depth", cxxopts::value<size_t>()->default_value("10"))
                ("crossover-probability", "The probability to apply crossover",
                 cxxopts::value<Turingforge::Scalar>()->default_value("1.0"))
                ("crossover-internal-probability", "Crossover bias towards swapping function nodes",
                 cxxopts::value<Turingforge::Scalar>()->default_value("0.9"))
                ("mutation-probability", "The probability to apply mutation",
                 cxxopts::value<Turingforge::Scalar>()->default_value("0.25"))
                ("reinserter",
                 "Reinsertion operator merging offspring in the recombination pool back into the population",
                 cxxopts::value<std::string>()->default_value("keep-best"))
                ("enable-symbols", "Comma-separated list of enabled symbols (" + symbols + ")",
                 cxxopts::value<std::string>())
                ("disable-symbols", "Comma-separated list of disabled symbols (" + symbols + ")",
                 cxxopts::value<std::string>())
                ("symbolic", "Operate in symbolic mode - no coefficient tuning or coefficient mutation",
                 cxxopts::value<bool>()->default_value("false"))
                ("timelimit", "Time limit after which the algorithm will terminate",
                 cxxopts::value<size_t>()->default_value(std::to_string(std::numeric_limits<size_t>::max())))
                ("debug", "Debug mode (more information displayed)")
                ("help", "Print help")
                ("version", "Print version and program information");
        return opts;
    }

    auto ParseOptions(cxxopts::Options &&opts, int argc, char **argv) -> cxxopts::ParseResult {
        cxxopts::ParseResult result;
        try {
            result = opts.parse(argc, argv);
        } catch (cxxopts::exceptions::parsing const &ex) {
            fmt::print(stderr, "error: {}. rerun with --help to see available options.\n", ex.what());
            std::exit(EXIT_FAILURE);
        }
        if (result.arguments().empty() || result.count("help") > 0) {
            fmt::print("{}\n", opts.help());
            std::exit(EXIT_SUCCESS);
        }
        if (result.count("version") > 0) {
            fmt::print("{}\n", Turingforge::Version());
            std::exit(EXIT_SUCCESS);
        }
        if (result.count("target") == 0) {
            fmt::print(stderr, "error: no target variable was specified.\n");
            std::exit(EXIT_FAILURE);
        }
        if (result.count("dataset") == 0) {
            fmt::print(stderr, "error: no dataset was specified.\n");
            std::exit(EXIT_FAILURE);
        }
        return result;
    }

}