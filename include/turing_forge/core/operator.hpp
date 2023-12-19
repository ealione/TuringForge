#pragma once

#include "types.hpp"


namespace Turingforge {

    template <typename Ret, typename... Args>
    struct OperatorBase {
        using ReturnType = Ret;
        using ArgumentType = std::tuple<Args...>;
        virtual auto operator()(Turingforge::RandomGenerator& /*random*/, Args... /*args*/) const -> Ret = 0;
        virtual ~OperatorBase() = default;

        OperatorBase() = default;
        OperatorBase(OperatorBase const& other) = default;
        OperatorBase(OperatorBase&& other) noexcept = default;

        auto operator=(OperatorBase const& other) -> OperatorBase& = default;
        auto operator=(OperatorBase&& other) noexcept -> OperatorBase& = default;
    };

}