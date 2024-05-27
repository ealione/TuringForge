#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

#include "turing_forge/core/constants.hpp"

namespace Turingforge {
    struct Hasher {
            using is_transparent = void; // enable transparent lookup NOLINT

            auto operator()(uint8_t const* key, size_t len) const noexcept -> uint64_t;
            auto operator()(std::string_view key) const noexcept -> uint64_t;
            auto operator()(std::string const& key) const noexcept -> uint64_t;
            auto operator()(char const* key) const noexcept -> uint64_t;
    };
} // namespace Turingforge