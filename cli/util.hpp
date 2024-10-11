#pragma once

#include <chrono>
#include <cstddef>
#include <cxxopts.hpp>
#include <fmt/core.h>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "turing_forge/core/function.hpp"

namespace Turingforge {

    constexpr int optionsWidth = 200;

    auto ParseRange(std::string const& str) -> std::pair<size_t, size_t>;
    auto Split(const std::string& s, char delimiter) -> std::vector<std::string>;
    auto FormatBytes(size_t bytes) -> std::string;
    auto FormatDuration(std::chrono::duration<double> d) -> std::string;
    auto ParsePrimitiveSetConfig(const std::string& options) -> FunctionType;
    auto PrintPrimitives(PrimitiveSetConfig config) -> void;
    auto PrintStats(std::vector<std::tuple<std::string, double, std::string>> const& stats, bool printHeader = true) -> void;

    auto InitOptions(std::string const& name, std::string const& desc, int width = optionsWidth) -> cxxopts::Options;
    auto ParseOptions(cxxopts::Options&& opts, int argc, char** argv) -> cxxopts::ParseResult;

} // namespace Turingforge
