#pragma once

#include <bit>
#include <bitset>
#include <cstddef>
#include <type_traits>

#include "types.hpp"


namespace Turingforge {

    enum class FunctionType : uint32_t {
        // n-ary symbols
        Add = 1U << 0U,
        Mul = 1U << 1U,
        Sub = 1U << 2U,
        Div = 1U << 3U,
        Fmin = 1U << 4U,
        Fmax = 1U << 5U,

        // binary symbols
        Aq = 1U << 6U,
        Pow = 1U << 7U,

        // unary symbols
        Abs = 1U << 8U,
        Acos = 1U << 9U,
        Asin = 1U << 10U,
        Atan = 1U << 11U,
        Cbrt = 1U << 12U,
        Ceil = 1U << 13U,
        Cos = 1U << 14U,
        Cosh = 1U << 15U,
        Exp = 1U << 16U,
        Floor = 1U << 17U,
        Log = 1U << 18U,
        Logabs = 1U << 19U,
        Log1p = 1U << 20U,
        Sin = 1U << 21U,
        Sinh = 1U << 22U,
        Sqrt = 1U << 23U,
        Sqrtabs = 1U << 24U,
        Tan = 1U << 25U,
        Tanh = 1U << 26U,
        Square = 1U << 27U,

        // nullary symbols (dynamic can be anything)
        Dynamic = 1U << 28U,
        Constant = 1U << 29U,
        Variable = 1U << 30U
    };

    using PrimitiveSetConfig = FunctionType;
    using UnderlyingFunctionType = std::underlying_type_t<FunctionType>;

    struct FunctionTypes {
        static constexpr auto Count =
                std::countr_zero(static_cast<uint64_t>(FunctionType::Variable)) + 1UL;

        static auto GetIndex(FunctionType type) -> size_t {
            return std::countr_zero(static_cast<uint32_t>(type));
        }

        static auto constexpr NoType{FunctionType{123456}};

        static auto GetAllTypes() -> std::vector<FunctionType> {
            std::vector<FunctionType> types;
            for (size_t i = 0; i <= static_cast<size_t>(FunctionType::Variable); ++i) {
                types.push_back(static_cast<FunctionType>(1U << i));
            }
            return types;
        }

        static auto toString(FunctionType type) -> std::string {
            switch (type) {
                // n-ary symbols
                case FunctionType::Add:
                    return "add";
                case FunctionType::Mul:
                    return "mul";
                case FunctionType::Sub:
                    return "sub";
                case FunctionType::Div:
                    return "div";
                case FunctionType::Fmin:
                    return "fmin";
                case FunctionType::Fmax:
                    return "fmax";

                    // binary symbols
                case FunctionType::Aq:
                    return "aq";
                case FunctionType::Pow:
                    return "pow";

                    // unary symbols
                case FunctionType::Abs:
                    return "abs";
                case FunctionType::Acos:
                    return "acos";
                case FunctionType::Asin:
                    return "asin";
                case FunctionType::Atan:
                    return "atan";
                case FunctionType::Cbrt:
                    return "cbrt";
                case FunctionType::Ceil:
                    return "ceil";
                case FunctionType::Cos:
                    return "cos";
                case FunctionType::Cosh:
                    return "cosh";
                case FunctionType::Exp:
                    return "exp";
                case FunctionType::Floor:
                    return "floor";
                case FunctionType::Log:
                    return "ln";
                case FunctionType::Logabs:
                    return "logabs";
                case FunctionType::Log1p:
                    return "log1p";
                case FunctionType::Sin:
                    return "sin";
                case FunctionType::Sinh:
                    return "sinh";
                case FunctionType::Sqrt:
                    return "sqrt";
                case FunctionType::Sqrtabs:
                    return "sqrtabs";
                case FunctionType::Tan:
                    return "tan";
                case FunctionType::Tanh:
                    return "tanh";
                case FunctionType::Square:
                    return "square";

                    // nullary symbols (dynamic can be anything)
                case FunctionType::Dynamic:
                    return "dynamic";
                case FunctionType::Constant:
                    return "constant";
                case FunctionType::Variable:
                    return "variable";

                default:
                    return "unknown";
            }
        }

        static auto GetAllSymbols() -> std::vector<std::string> {
            std::vector<std::string> symbols;
            for (size_t i = 0; i <= static_cast<size_t>(FunctionType::Variable); ++i) {
                symbols.push_back(toString(static_cast<FunctionType>(1U << i)));
            }
            return symbols;
        }

        // TODO: Not used, consider removing
        [[maybe_unused]] static auto RandomSymbol(auto &random) -> FunctionType {
            auto all_types = Turingforge::FunctionTypes::GetAllTypes();
            return *Sample(random, all_types.begin(), all_types.end());
        };
    };

    inline constexpr auto operator&(FunctionType lhs, FunctionType rhs) -> FunctionType {
        return static_cast<FunctionType>(static_cast<UnderlyingFunctionType>(lhs) &
                                         static_cast<UnderlyingFunctionType>(rhs));
    }

    inline constexpr auto operator|(FunctionType lhs, FunctionType rhs) -> FunctionType {
        return static_cast<FunctionType>(static_cast<UnderlyingFunctionType>(lhs) |
                                         static_cast<UnderlyingFunctionType>(rhs));
    }

    inline constexpr auto operator^(FunctionType lhs, FunctionType rhs) -> FunctionType {
        return static_cast<FunctionType>(static_cast<UnderlyingFunctionType>(lhs) ^
                                         static_cast<UnderlyingFunctionType>(rhs));
    }

    inline constexpr auto operator~(FunctionType lhs) -> FunctionType {
        return static_cast<FunctionType>(~static_cast<UnderlyingFunctionType>(lhs));
    }

    inline auto operator&=(FunctionType &lhs, FunctionType rhs) -> FunctionType & {
        lhs = lhs & rhs;
        return lhs;
    }

    inline auto operator|=(FunctionType &lhs, FunctionType rhs) -> FunctionType & {
        lhs = lhs | rhs;
        return lhs;
    }

    inline auto operator^=(FunctionType &lhs, FunctionType rhs) -> FunctionType & {
        lhs = lhs ^ rhs;
        return lhs;
    }

    struct Function {
        Turingforge::Hash HashValue;    // needs to be unique for each node type
        Turingforge::Scalar Value;      // value for constants or weighting factor for variables
        uint16_t Arity;                 // 0-65535
        uint16_t Length;                // 0-65535
        FunctionType Type;
        bool IsEnabled;

        Function() = default;
        Function(const Function& other) = default;

        explicit Function(FunctionType type) noexcept
                : Function(type, static_cast<Turingforge::Hash>(type))
        {
        }

        explicit Function(FunctionType type, Turingforge::Hash hashValue) noexcept
                : HashValue(hashValue), Arity(0UL), Length(0UL), Type(type) {
            if (Type < FunctionType::Abs) // Add, Mul, Sub, Div, Aq, Pow
                Arity = 2;
            else if (Type < FunctionType::Dynamic) // Log, Exp, Sin, Cos, Tan, Tanh, Sqrt, Cbrt, Square
                Arity = 1;

            Length = Arity;
            IsEnabled = true;
            Value = 1.;
        }

        // TODO: Not used, consider removing.
        static auto RandomFunction() -> Function {
            Turingforge::RandomGenerator rng{0};
            Function node(Turingforge::FunctionTypes::RandomSymbol(rng));
            return node;
        }

        // TODO: Possibly not needed.
        static auto Constant(double value) {
            Function node(FunctionType::Constant);
            node.Value = static_cast<Turingforge::Scalar>(value);
            return node;
        }

        [[nodiscard]] auto Name() const noexcept -> std::string const &;

        [[nodiscard]] auto Desc() const noexcept -> std::string const &;

        // comparison operators
        inline auto operator==(const Function &rhs) const noexcept -> bool {
            return HashValue == rhs.HashValue;
        }

        inline auto operator!=(const Function &rhs) const noexcept -> bool {
            return !((*this) == rhs);
        }

        inline auto operator<(const Function &rhs) const noexcept -> bool {
            return std::tie(HashValue) < std::tie(rhs.HashValue);
        }

        inline auto operator<=(const Function &rhs) const noexcept -> bool {
            return ((*this) == rhs || (*this) < rhs);
        }

        inline auto operator>(const Function &rhs) const noexcept -> bool {
            return !((*this) <= rhs);
        }

        inline auto operator>=(const Function &rhs) const noexcept -> bool {
            return !((*this) < rhs);
        }

        Function& operator=(const Function& other) = default;

        [[nodiscard]] inline auto
        IsCommutative() const noexcept -> bool { return Is<FunctionType::Add, FunctionType::Mul, FunctionType::Fmin, FunctionType::Fmax>(); }

        template<FunctionType... T>
        [[nodiscard]] inline auto Is() const -> bool { return ((Type == T) || ...); }

        [[nodiscard]] inline auto Is(FunctionType T) const -> bool { return (Type == T); }

        [[nodiscard]] inline auto IsConstant() const -> bool { return Is(FunctionType::Constant); }

        [[nodiscard]] inline auto IsVariable() const -> bool { return Is(FunctionType::Variable); }

        [[nodiscard]] inline auto IsAddition() const -> bool { return Is(FunctionType::Add); }

        [[nodiscard]] inline auto IsSubtraction() const -> bool { return Is(FunctionType::Sub); }

        [[nodiscard]] inline auto IsMultiplication() const -> bool { return Is(FunctionType::Mul); }

        [[nodiscard]] inline auto IsDivision() const -> bool { return Is(FunctionType::Div); }

        [[nodiscard]] inline auto IsCeiling() const -> bool { return Is(FunctionType::Ceil); }

        [[nodiscard]] inline auto IsFloor() const -> bool { return Is(FunctionType::Floor); }

        [[nodiscard]] inline auto IsAbsolute() const -> bool { return Is(FunctionType::Abs); }

        [[nodiscard]] inline auto IsAq() const -> bool { return Is(FunctionType::Aq); }

        [[nodiscard]] inline auto IsPow() const -> bool { return Is(FunctionType::Pow); }

        [[nodiscard]] inline auto IsExp() const -> bool { return Is(FunctionType::Exp); }

        [[nodiscard]] inline auto IsLog() const -> bool { return Is(FunctionType::Log); }

        [[nodiscard]] inline auto IsLog1p() const -> bool { return Is(FunctionType::Log1p); }

        [[nodiscard]] inline auto IsLogAbs() const -> bool { return Is(FunctionType::Logabs); }

        [[nodiscard]] inline auto IsSin() const -> bool { return Is(FunctionType::Sin); }

        [[nodiscard]] inline auto IsAsin() const -> bool { return Is(FunctionType::Asin); }

        [[nodiscard]] inline auto IsCos() const -> bool { return Is(FunctionType::Cos); }

        [[nodiscard]] inline auto IsAcos() const -> bool { return Is(FunctionType::Acos); }

        [[nodiscard]] inline auto IsSinh() const -> bool { return Is(FunctionType::Sinh); }

        [[nodiscard]] inline auto IsCosh() const -> bool { return Is(FunctionType::Cosh); }

        [[nodiscard]] inline auto IsTan() const -> bool { return Is(FunctionType::Tan); }

        [[nodiscard]] inline auto IsAtan() const -> bool { return Is(FunctionType::Atan); }

        [[nodiscard]] inline auto IsTanh() const -> bool { return Is(FunctionType::Tanh); }

        [[nodiscard]] inline auto IsSquareRoot() const -> bool { return Is(FunctionType::Sqrt); }

        [[nodiscard]] inline auto IsSquareRootAbs() const -> bool { return Is(FunctionType::Sqrtabs); }

        [[nodiscard]] inline auto IsCubeRoot() const -> bool { return Is(FunctionType::Cbrt); }

        [[nodiscard]] inline auto IsSquare() const -> bool { return Is(FunctionType::Square); }

        [[nodiscard]] inline auto IsDynamic() const -> bool { return Is(FunctionType::Dynamic); }

        template<FunctionType Type>
        static auto constexpr IsNary = Type < FunctionType::Aq;

        template<FunctionType Type>
        static auto constexpr IsBinary = Type > FunctionType::Fmax && Type < FunctionType::Abs;

        template<FunctionType Type>
        static auto constexpr IsUnary = Type > FunctionType::Pow && Type < FunctionType::Dynamic;

        template<FunctionType Type>
        static auto constexpr IsNullary = Type > FunctionType::Square;

        template<typename... Args>
        auto operator()(Args&&... args) {
            // TODO: Only unary for now.
            return EvaluateUnary(std::forward<Args>(args)...);
        }

    private:
        template<typename T>
        auto EvaluateUnary(T&& x) {
            if (IsSin()) {
                return Turingforge::Scalar(std::sin(x));
            } else if (IsCos()) {
                return Turingforge::Scalar(std::cos(x));
            } else if (IsTan()) {
                return Turingforge::Scalar(std::tan(x));
            } else if (IsAsin()) {
                return Turingforge::Scalar(std::asin(x));
            } else if (IsAcos()) {
                return Turingforge::Scalar(std::acos(x));
            } else if (IsAtan()) {
                return Turingforge::Scalar(std::atan(x));
            } else if (IsSinh()) {
                return Turingforge::Scalar(std::sinh(x));
            } else if (IsCosh()) {
                return Turingforge::Scalar(std::cosh(x));
            } else if (IsTanh()) {
                return Turingforge::Scalar(std::tanh(x));
            } else if (IsExp()) {
                return Turingforge::Scalar(std::exp(x));
            } else if (IsLog()) {
                return Turingforge::Scalar(std::log(x));
            } else if (IsLog1p()) {
                return Turingforge::Scalar(std::log1p(x));
            } else if (IsLogAbs()) {
                return Turingforge::Scalar(std::log(std::abs(x)));
            } else if (IsSquare()) {
                return Turingforge::Scalar(x * x);
            } else if (IsSquareRoot()) {
                return Turingforge::Scalar(std::sqrt(x));
            } else if (IsSquareRootAbs()) {
                return Turingforge::Scalar(std::sqrt(std::abs(x)));
            } else if (IsCubeRoot()) {
                return Turingforge::Scalar(std::cbrt(x));
            } else if (IsCeiling()) {
                return Turingforge::Scalar(std::ceil(x));
            } else if (IsFloor()) {
                return Turingforge::Scalar(std::floor(x));
            } else if (IsAbsolute()) {
                return Turingforge::Scalar(std::abs(x));
            }
        }

        template<typename T>
        auto EvaluateBinary(T&& x, T&& y) {
            if (IsAddition()) {
                return Turingforge::Scalar(x + y);
            } else if (IsMultiplication()) {
                return Turingforge::Scalar(x * y);
            } else if (IsSubtraction()) {
                return Turingforge::Scalar(x - y);
            } else if (IsDivision()) {
                return Turingforge::Scalar(x / y);
            } else if (IsAq()) {
                return Turingforge::Scalar(x * std::sqrt(1 + y * y));
            } else if (IsPow()) {
                return Turingforge::Scalar(std::pow(x, y));
            }
        }

        template<typename... Ts>
        auto EvaluateNary(Ts&&... xs) {
            if (IsAddition()) {
                return Turingforge::Scalar((xs + ...));
            } else if (IsMultiplication()) {
                return Turingforge::Scalar((xs * ...));
            } else if (IsSubtraction()) {
                return Turingforge::Scalar((xs - ...));
            } else if (IsDivision()) {
                return Turingforge::Scalar((xs / ...));
            }
        }
    };

} // namespace Turingforge