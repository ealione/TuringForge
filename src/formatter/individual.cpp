#include "turing_forge//core/dataset.hpp"
#include "turing_forge//formatter/formatter.hpp"

namespace Turingforge {

    // TODO: this is not done! Must refactor/optimise
    auto IndividualFormatter::FormatNode(Individual const& individual, Turingforge::Map<Turingforge::Hash, std::string> const& variableNames, size_t i, fmt::memory_buffer& current, int decimalPrecision) -> void
    {
        if (i == std::numeric_limits<size_t>::max()) return;
        const auto& tp = individual.GetAllDataAt(i);
        const auto& coef = std::get<0>(tp);
        const auto& func = std::get<1>(tp);
        const auto& poly = std::get<2>(tp);
        if (func.IsConstant()) {
            auto formatString = fmt::format(fmt::runtime(func.Value < 0 ? "({{:.{}f}})" : "{{:.{}f}}"), decimalPrecision);
            fmt::format_to(std::back_inserter(current), fmt::runtime(formatString), func.Value);
        } else if (func.IsVariable()) {
            auto formatString = fmt::format(fmt::runtime(func.Value < 0 ? "(({{:.{}f}}) * {{}})" : "({{:.{}f}} * {{}})"), decimalPrecision);
            if (auto it = variableNames.find(func.HashValue); it != variableNames.end()) {
                fmt::format_to(std::back_inserter(current), fmt::runtime(formatString), func.Value, it->second);
            } else {
                throw std::runtime_error(fmt::format("A key with hash value {} could not be found in the variable map.\n", func.HashValue));
            }
        } else {
            if (func.Type < FunctionType::Abs) // add, sub, mul, div, aq, fmax, fmin, pow
            {
                fmt::format_to(std::back_inserter(current), "(");
                if (func.Arity == 1) {
                    if (func.Type == FunctionType::Sub) {
                        // subtraction with a single argument is a negation -x
                        fmt::format_to(std::back_inserter(current), "-");
                    } else if (func.Type == FunctionType::Div) {
                        // division with a single argument is an inversion 1/x
                        fmt::format_to(std::back_inserter(current), "1 / ");
                    }
                    FormatNode(individual, variableNames, i-1, current, decimalPrecision);
                } else if (func.Type == FunctionType::Pow) {
                    // format pow(a,b) as a^b
                    auto j = i - 1;
                    auto k = j - individual.Length - 1;
                    FormatNode(individual, variableNames, j, current, decimalPrecision);
                    fmt::format_to(std::back_inserter(current), " ^ ");
                    FormatNode(individual, variableNames, k, current, decimalPrecision);
                } else if (func.Type == FunctionType::Aq) {
                    // format aq(a,b) as a / (1 + b^2)
                    auto j = i - 1;
                    auto k = j - individual.Length - 1;
                    FormatNode(individual, variableNames, j, current, decimalPrecision);
                    fmt::format_to(std::back_inserter(current), " / (sqrt(1 + ");
                    FormatNode(individual, variableNames, k, current, decimalPrecision);
                    fmt::format_to(std::back_inserter(current), " ^ 2))");
                } else if (func.Type == FunctionType::Fmin) {
                    auto j = i - 1;
                    auto k = j - individual.Length - 1;
                    fmt::format_to(std::back_inserter(current), "min(");
                    FormatNode(individual, variableNames, j, current, decimalPrecision);
                    fmt::format_to(std::back_inserter(current), ", ");
                    FormatNode(individual, variableNames, k, current, decimalPrecision);
                    fmt::format_to(std::back_inserter(current), ")");
                } else if (func.Type == FunctionType::Fmax) {
                    auto j = i - 1;
                    auto k = j - individual.Length - 1;
                    fmt::format_to(std::back_inserter(current), "max(");
                    FormatNode(individual, variableNames, j, current, decimalPrecision);
                    fmt::format_to(std::back_inserter(current), ", ");
                    FormatNode(individual, variableNames, k, current, decimalPrecision);
                    fmt::format_to(std::back_inserter(current), ")");
                }
                fmt::format_to(std::back_inserter(current), ")");
            } else { // unary operators abs, asin, ... log, exp, sin, etc.
                if (func.Type == FunctionType::Square) {
                    // format square(a) as a ^ 2
                    fmt::format_to(std::back_inserter(current), "(");
                    FormatNode(individual, variableNames, i - 1, current, decimalPrecision);
                    fmt::format_to(std::back_inserter(current), " ^ 2)");
                } else if (func.Type == FunctionType::Logabs) {
                    // format logabs(a) as log(abs(a))
                    fmt::format_to(std::back_inserter(current), "log(abs(");
                    FormatNode(individual, variableNames, i - 1, current, decimalPrecision);
                    fmt::format_to(std::back_inserter(current), "))");
                } else if (func.Type == FunctionType::Log1p) {
                    // format log1p(a) as log(a+1)
                    fmt::format_to(std::back_inserter(current), "log(");
                    FormatNode(individual, variableNames, i - 1, current, decimalPrecision);
                    fmt::format_to(std::back_inserter(current), "+1)");
                } else if (func.Type == FunctionType::Sqrtabs) {
                    // format sqrtabs(a) as sqrt(abs(a))
                    fmt::format_to(std::back_inserter(current), "sqrt(abs(");
                    FormatNode(individual, variableNames, i - 1, current, decimalPrecision);
                    fmt::format_to(std::back_inserter(current), "))");
                } else {
                    auto formatString = fmt::format(fmt::runtime(coef < 0 ? "-({})" : "+({})"), std::abs(coef));
                    fmt::format_to(std::back_inserter(current), fmt::runtime(formatString), coef);
                    fmt::format_to(std::back_inserter(current), "{}", func.Name());
                    fmt::format_to(std::back_inserter(current), "(");
                    formatString = fmt::format("('{{}}'^{{}})", decimalPrecision);
                    // I'd use a `zip` here, but I've come to the conclusion that C and C++ actually suck.
                    auto varIt = variableNames.begin();
                    auto expIt = poly.begin();
                    while(varIt != variableNames.end() && expIt != poly.end()) {
                        std::string varName = varIt->second;
                        double exp = *expIt;
                        fmt::format_to(std::back_inserter(current), fmt::runtime(formatString), varName, exp);
                        ++varIt;
                        ++expIt;
                    }
                    fmt::format_to(std::back_inserter(current), ")");
                    FormatNode(individual, variableNames, i - 1, current, decimalPrecision);
                }
            }
        }
    }

    auto IndividualFormatter::Format(Individual const& individual, Dataset const& dataset, int decimalPrecision) -> std::string
    {
        Turingforge::Map<Turingforge::Hash, std::string> variableNames;
        for (auto const& var : dataset.GetVariables()) {
            variableNames.insert({ var.Hash, var.Name });
        }
        fmt::memory_buffer result;
        FormatNode(individual, variableNames, individual.Length - 1, result, decimalPrecision);
        return { result.begin(), result.end() };
    }

    auto IndividualFormatter::Format(Individual const& individual, Turingforge::Map<Turingforge::Hash, std::string> const& variableNames, int decimalPrecision) -> std::string
    {
        fmt::memory_buffer result;
        FormatNode(individual, variableNames, individual.Length - 1, result, decimalPrecision);
        return { result.begin(), result.end() };
    }

} // namespace Turingforge