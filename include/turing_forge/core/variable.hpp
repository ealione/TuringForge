#pragma once

#include "types.hpp"

namespace Turingforge {
    struct Variable {
        std::string Name;
        int64_t Index{0};

        constexpr auto operator==(Variable const& rhs) const noexcept -> bool {
            return std::tie(Name, Index) == std::tie(rhs.Name, rhs.Index);
        }

        constexpr auto operator!=(Variable const& rhs) const noexcept -> bool {
            return !(*this == rhs);
        }
    };
} // namespace Turingforge