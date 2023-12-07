#pragma once

#include <cstddef>
#include <type_traits>
#include <bitset>
#include <bit>

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
        static constexpr auto Count = std::countr_zero(static_cast<uint64_t>(FunctionType::Variable)) + 1UL;

        static auto GetIndex(FunctionType type) -> size_t {
            return std::countr_zero(static_cast<uint32_t>(type));
        }
    };

    inline constexpr auto
    operator&(FunctionType lhs, FunctionType rhs) -> FunctionType {
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

    inline constexpr auto operator~(
            FunctionType lhs) -> FunctionType { return static_cast<FunctionType>(~static_cast<UnderlyingFunctionType>(lhs)); }

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

}