#pragma once

#include <bit>
#include <bitset>
#include <cstddef>
#include <type_traits>

#include "types.hpp"


namespace Turingforge {

enum class NodeType : uint32_t {
    // n-ary symbols
    Add      = 1U << 0U,
    Mul      = 1U << 1U,
    Sub      = 1U << 2U,
    Div      = 1U << 3U,
    Fmin     = 1U << 4U,
    Fmax     = 1U << 5U,

    // binary symbols
    Aq       = 1U << 6U,
    Pow      = 1U << 7U,

    // unary symbols
    Abs      = 1U << 8U,
    Acos     = 1U << 9U,
    Asin     = 1U << 10U,
    Atan     = 1U << 11U,
    Cbrt     = 1U << 12U,
    Ceil     = 1U << 13U,
    Cos      = 1U << 14U,
    Cosh     = 1U << 15U,
    Exp      = 1U << 16U,
    Floor    = 1U << 17U,
    Log      = 1U << 18U,
    Logabs   = 1U << 19U,
    Log1p    = 1U << 20U,
    Sin      = 1U << 21U,
    Sinh     = 1U << 22U,
    Sqrt     = 1U << 23U,
    Sqrtabs  = 1U << 24U,
    Tan      = 1U << 25U,
    Tanh     = 1U << 26U,
    Square   = 1U << 27U,

    // nullary symbols (dynamic can be anything)
    Dynamic  = 1U << 28U,
    Constant = 1U << 29U,
    Variable = 1U << 30U
};

using PrimitiveSetConfig = NodeType;
using UnderlyingFunctionType = std::underlying_type_t<NodeType>;

struct NodeTypes {
  static constexpr auto Count =
          std::countr_zero(static_cast<uint64_t>(NodeType::Variable)) + 1UL;

  static auto GetIndex(NodeType type) -> size_t {
    return std::countr_zero(static_cast<uint32_t>(type));
  }

    static auto GetAllTypes() -> std::vector<NodeType> {
        std::vector<NodeType> types;
        for (size_t i = 0; i <= static_cast<size_t>(NodeType::Variable); ++i) {
            types.push_back(static_cast<NodeType>(1U << i));
        }
        return types;
    }

    static auto toString(NodeType type) -> std::string {
        switch (type) {
            // n-ary symbols
            case NodeType::Add: return "add";
            case NodeType::Mul: return "mul";
            case NodeType::Sub: return "sub";
            case NodeType::Div: return "div";
            case NodeType::Fmin: return "fmin";
            case NodeType::Fmax: return "fmax";

                // binary symbols
            case NodeType::Aq: return "aq";
            case NodeType::Pow: return "pow";

                // unary symbols
            case NodeType::Abs: return "abs";
            case NodeType::Acos: return "acos";
            case NodeType::Asin: return "asin";
            case NodeType::Atan: return "atan";
            case NodeType::Cbrt: return "cbrt";
            case NodeType::Ceil: return "ceil";
            case NodeType::Cos: return "cos";
            case NodeType::Cosh: return "cosh";
            case NodeType::Exp: return "exp";
            case NodeType::Floor: return "floor";
            case NodeType::Log: return "log";
            case NodeType::Logabs: return "logabs";
            case NodeType::Log1p: return "log1p";
            case NodeType::Sin: return "sin";
            case NodeType::Sinh: return "sinh";
            case NodeType::Sqrt: return "sqrt";
            case NodeType::Sqrtabs: return "sqrtabs";
            case NodeType::Tan: return "tan";
            case NodeType::Tanh: return "tanh";
            case NodeType::Square: return "square";

                // nullary symbols (dynamic can be anything)
            case NodeType::Dynamic: return "dynamic";
            case NodeType::Constant: return "constant";
            case NodeType::Variable: return "variable";

            default: return "unknown";
        }
    }

    static auto GetAllSymbols() -> std::vector<std::string> {
        std::vector<std::string> symbols;
        for (size_t i = 0; i <= static_cast<size_t>(NodeType::Variable); ++i) {
            symbols.push_back(toString(static_cast<NodeType>(1U << i)));
        }
        return symbols;
    }
};

inline constexpr auto operator&(NodeType lhs, NodeType rhs) -> NodeType {
  return static_cast<NodeType>(static_cast<UnderlyingFunctionType>(lhs) &
                               static_cast<UnderlyingFunctionType>(rhs));
}

inline constexpr auto operator|(NodeType lhs, NodeType rhs) -> NodeType {
  return static_cast<NodeType>(static_cast<UnderlyingFunctionType>(lhs) |
                               static_cast<UnderlyingFunctionType>(rhs));
}

inline constexpr auto operator^(NodeType lhs, NodeType rhs) -> NodeType {
  return static_cast<NodeType>(static_cast<UnderlyingFunctionType>(lhs) ^
                               static_cast<UnderlyingFunctionType>(rhs));
}

inline constexpr auto operator~(NodeType lhs) -> NodeType {
  return static_cast<NodeType>(~static_cast<UnderlyingFunctionType>(lhs));
}

inline auto operator&=(NodeType &lhs, NodeType rhs) -> NodeType & {
  lhs = lhs & rhs;
  return lhs;
}

inline auto operator|=(NodeType &lhs, NodeType rhs) -> NodeType & {
  lhs = lhs | rhs;
  return lhs;
}

inline auto operator^=(NodeType &lhs, NodeType rhs) -> NodeType & {
  lhs = lhs ^ rhs;
  return lhs;
}

struct Node {
    Turingforge::Scalar Value; // value for constants or weighting factor for variables
    uint16_t Arity; // 0-65535
    uint16_t Length; // 0-65535
    NodeType Type;
    bool IsEnabled;

    Node() = default;

    explicit Node(NodeType type) noexcept
            : Arity(0UL)
            , Length(0UL)
            , Type(type)
    {
        if (Type < NodeType::Abs) // Add, Mul, Sub, Div, Aq, Pow
            Arity = 2;
        else if (Type < NodeType::Dynamic) // Log, Exp, Sin, Cos, Tan, Tanh, Sqrt, Cbrt, Square
            Arity = 1;

        Length = Arity;
        IsEnabled = true;
        Value = 1.;
    }

    static auto Constant(double value)
    {
        Node node(NodeType::Constant);
        node.Value = static_cast<Turingforge::Scalar>(value);
        return node;
    }

    [[nodiscard]] auto Name() const noexcept -> std::string const&;
    [[nodiscard]] auto Desc() const noexcept -> std::string const&;

    // comparison operators
    inline auto operator==(const Node& rhs) const noexcept -> bool
    {
        return Arity == rhs.Arity && Type == rhs.Type && Length == rhs.Length;
    }

    inline auto operator!=(const Node& rhs) const noexcept -> bool
    {
        return !((*this) == rhs);
    }

    inline auto operator<(const Node& rhs) const noexcept -> bool
    {
        return std::tie(Type) < std::tie(rhs.Type);
    }

    inline auto operator<=(const Node& rhs) const noexcept -> bool
    {
        return ((*this) == rhs || (*this) < rhs);
    }

    inline auto operator>(const Node& rhs) const noexcept -> bool
    {
        return !((*this) <= rhs);
    }

    inline auto operator>=(const Node& rhs) const noexcept -> bool
    {
        return !((*this) < rhs);
    }

    [[nodiscard]] inline auto IsLeaf() const noexcept -> bool { return Arity == 0; }
    [[nodiscard]] inline auto IsCommutative() const noexcept -> bool { return Is<NodeType::Add, NodeType::Mul, NodeType::Fmin, NodeType::Fmax>(); }

    template <NodeType... T>
    [[nodiscard]] inline auto Is() const -> bool { return ((Type == T) || ...); }

    [[nodiscard]] inline auto IsConstant() const -> bool { return Is<NodeType::Constant>(); }
    [[nodiscard]] inline auto IsVariable() const -> bool { return Is<NodeType::Variable>(); }
    [[nodiscard]] inline auto IsAddition() const -> bool { return Is<NodeType::Add>(); }
    [[nodiscard]] inline auto IsSubtraction() const -> bool { return Is<NodeType::Sub>(); }
    [[nodiscard]] inline auto IsMultiplication() const -> bool { return Is<NodeType::Mul>(); }
    [[nodiscard]] inline auto IsDivision() const -> bool { return Is<NodeType::Div>(); }
    [[nodiscard]] inline auto IsAq() const -> bool { return Is<NodeType::Aq>(); }
    [[nodiscard]] inline auto IsPow() const -> bool { return Is<NodeType::Pow>(); }
    [[nodiscard]] inline auto IsExp() const -> bool { return Is<NodeType::Exp>(); }
    [[nodiscard]] inline auto IsLog() const -> bool { return Is<NodeType::Log>(); }
    [[nodiscard]] inline auto IsSin() const -> bool { return Is<NodeType::Sin>(); }
    [[nodiscard]] inline auto IsCos() const -> bool { return Is<NodeType::Cos>(); }
    [[nodiscard]] inline auto IsTan() const -> bool { return Is<NodeType::Tan>(); }
    [[nodiscard]] inline auto IsTanh() const -> bool { return Is<NodeType::Tanh>(); }
    [[nodiscard]] inline auto IsSquareRoot() const -> bool { return Is<NodeType::Sqrt>(); }
    [[nodiscard]] inline auto IsCubeRoot() const -> bool { return Is<NodeType::Cbrt>(); }
    [[nodiscard]] inline auto IsSquare() const -> bool { return Is<NodeType::Square>(); }
    [[nodiscard]] inline auto IsDynamic() const -> bool { return Is<NodeType::Dynamic>(); }

    template<NodeType Type>
    static auto constexpr IsNary = Type < NodeType::Aq;

    template<NodeType Type>
    static auto constexpr IsBinary = Type > NodeType::Fmax && Type < NodeType::Abs;

    template<NodeType Type>
    static auto constexpr IsUnary = Type > NodeType::Pow && Type < NodeType::Dynamic;

    template<NodeType Type>
    static auto constexpr IsNullary = Type > NodeType::Square;
};

} // namespace Turingforge