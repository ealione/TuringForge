#pragma once

#include "types.hpp"

namespace Turingforge {
    struct Variable {
        std::string Name;
        Turingforge::Hash Hash{0};
        int64_t Index{0};

        constexpr auto operator==(Variable const& rhs) const noexcept -> bool {
            return std::tie(Name, Hash, Index) == std::tie(rhs.Name, rhs.Hash, rhs.Index);
        }

        constexpr auto operator!=(Variable const& rhs) const noexcept -> bool {
            return !(*this == rhs);
        }
    };
} // namespace Turingforge