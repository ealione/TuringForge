#pragma once

#include <bit>
#include <bitset>
#include <cstddef>
#include <type_traits>

namespace Turingforge {

enum class FunctionType : uint32_t {
  // unary symbols
  Abs = 1U << 0U,
  Acos = 1U << 1U,
  Asin = 1U << 2U,
  Atan = 1U << 3U,
  Cbrt = 1U << 4U,
  Ceil = 1U << 5U,
  Cos = 1U << 6U,
  Cosh = 1U << 7U,
  Exp = 1U << 8U,
  Floor = 1U << 9U,
  Log = 1U << 10U,
  Logabs = 1U << 11U,
  Log1p = 1U << 12U,
  Sin = 1U << 13U,
  Sinh = 1U << 14U,
  Sqrt = 1U << 15U,
  Sqrtabs = 1U << 16U,
  Tan = 1U << 17U,
  Tanh = 1U << 18U,
  Square = 1U << 19U,

  // nullary symbols (dynamic can be anything)
  Dynamic = 1U << 20U,
  Constant = 1U << 21U,
  Variable = 1U << 22U
};

using PrimitiveSetConfig = FunctionType;
using UnderlyingFunctionType = std::underlying_type_t<FunctionType>;

struct FunctionTypes {
  static constexpr auto Count =
      std::countr_zero(static_cast<uint64_t>(FunctionType::Variable)) + 1UL;

  static auto GetIndex(FunctionType type) -> size_t {
    return std::countr_zero(static_cast<uint32_t>(type));
  }
};

inline constexpr auto operator&(FunctionType lhs, FunctionType rhs)
    -> FunctionType {
  return static_cast<FunctionType>(static_cast<UnderlyingFunctionType>(lhs) &
                                   static_cast<UnderlyingFunctionType>(rhs));
}

inline constexpr auto operator|(FunctionType lhs, FunctionType rhs)
    -> FunctionType {
  return static_cast<FunctionType>(static_cast<UnderlyingFunctionType>(lhs) |
                                   static_cast<UnderlyingFunctionType>(rhs));
}

inline constexpr auto operator^(FunctionType lhs, FunctionType rhs)
    -> FunctionType {
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

} // namespace Turingforge

