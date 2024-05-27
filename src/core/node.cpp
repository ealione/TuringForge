#include <iterator>
#include <unordered_map>
#include <utility>
#include <string>

#include <ankerl/unordered_dense.h>
#include "turing_forge/core/function.hpp"


using std::pair;
using std::string;
using std::unordered_map;

namespace Turingforge {

    static const Turingforge::Map<FunctionType, pair < string, string>> NodeDesc = {
            {FunctionType::Add,      std::make_pair("+", "n-ary addition f(a,b,c,...) = a + b + c + ...") },
            {FunctionType::Mul,      std::make_pair("*", "n-ary multiplication f(a,b,c,...) = a * b * c * ..." ) },
            {FunctionType::Sub,      std::make_pair("-", "n-ary subtraction f(a,b,c,...) = a - (b + c + ...)" ) },
            {FunctionType::Div,      std::make_pair("/", "n-ary division f(a,b,c,..) = a / (b * c * ...)" ) },
            {FunctionType::Fmin,     std::make_pair("fmin", "minimum function f(a,b) = min(a,b)" ) },
            {FunctionType::Fmax,     std::make_pair("fmax", "maximum function f(a,b) = max(a,b)" ) },
            {FunctionType::Aq,       std::make_pair("aq", "analytical quotient f(a,b) = a / sqrt(1 + b^2)" ) },
            {FunctionType::Pow,      std::make_pair("pow", "raise to power f(a,b) = a^b" ) },
            {FunctionType::Abs,      std::make_pair("abs", "absolute value function f(a) = abs(a)" ) },
            {FunctionType::Acos,     std::make_pair("acos", "inverse cosine function f(a) = acos(a)" ) },
            {FunctionType::Asin,     std::make_pair("asin", "inverse sine function f(a) = asin(a)" ) },
            {FunctionType::Atan,     std::make_pair("atan", "inverse tangent function f(a) = atan(a)" ) },
            {FunctionType::Cbrt,     std::make_pair("cbrt", "cube root function f(a) = cbrt(a)" ) },
            {FunctionType::Ceil,     std::make_pair("ceil", "ceiling function f(a) = ceil(a)" ) },
            {FunctionType::Cos,      std::make_pair("cos", "cosine function f(a) = cos(a)" ) },
            {FunctionType::Cosh,     std::make_pair("cosh", "hyperbolic cosine function f(a) = cosh(a)" ) },
            {FunctionType::Exp,      std::make_pair("exp", "e raised to the given power f(a) = e^a" ) },
            {FunctionType::Floor,    std::make_pair("floor", "floor function f(a) = floor(a)" ) },
            {FunctionType::Log,      std::make_pair("log", "natural (base e) logarithm f(a) = ln(a)" ) },
            {FunctionType::Logabs,   std::make_pair("logabs", "natural (base e) logarithm of absolute value f(a) = ln(|a|)" ) },
            {FunctionType::Log1p,    std::make_pair("log1p", "f(a) = ln(a + 1), accurate even when a is close to zero" ) },
            {FunctionType::Sin,      std::make_pair("sin", "sine function f(a) = sin(a)" ) },
            {FunctionType::Sinh,     std::make_pair("sinh", "hyperbolic sine function f(a) = sinh(a)" ) },
            {FunctionType::Sqrt,     std::make_pair("sqrt", "square root function f(a) = sqrt(a)" ) },
            {FunctionType::Sqrtabs,  std::make_pair("sqrtabs", "square root of absolute value function f(a) = sqrt(|a|)" ) },
            {FunctionType::Tan,      std::make_pair("tan", "tangent function f(a) = tan(a)" ) },
            {FunctionType::Tanh,     std::make_pair("tanh", "hyperbolic tangent function f(a) = tanh(a)" ) },
            {FunctionType::Square,   std::make_pair("square", "square function f(a) = a^2" ) },
            {FunctionType::Dynamic,  std::make_pair("dyn", "user-defined function" ) },
            {FunctionType::Constant, std::make_pair("constant", "a constant value" ) },
            {FunctionType::Variable, std::make_pair("variable", "a dataset input with an associated weight" ) }
    };

    auto Function::Name() const noexcept -> std::string const& { return NodeDesc.at(Type).first; }
    auto Function::Desc() const noexcept -> std::string const& { return NodeDesc.at(Type).second; }
}