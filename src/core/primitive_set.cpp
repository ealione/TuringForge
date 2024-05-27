#include "turing_forge/core/primitive_set.hpp"

namespace Turingforge {

    auto PrimitiveSet::SampleRandomSymbol(Turingforge::RandomGenerator& random, size_t minArity, size_t maxArity) const -> Function
    {
        EXPECT(minArity <= maxArity);
        EXPECT(!pset_.empty());

        std::vector<Primitive> candidates;
        candidates.reserve(pset_.size());

        auto sum{0UL};
        for (auto const& [k, v] : pset_) {
            auto const& [node, freq, min_arity, max_arity] = v;
            if (!(node.IsEnabled && freq > 0)) { continue; }
            if (minArity > max_arity || maxArity < min_arity) { continue; }
            sum += freq;
            candidates.push_back(v);
        }

        if (candidates.empty()) {
            // arity requirements unreasonable
            throw std::runtime_error(fmt::format("PrimitiveSet::SampleRandomSymbol: unable to find suitable symbol with arity between {} and {}\n", minArity, maxArity));
        }

        Turingforge::Function result {Turingforge::FunctionType::Constant };

        auto c { std::uniform_real_distribution<Turingforge::Scalar>(0, sum)(random) };
        auto s { 0UL };
        for (auto const& [node, freq, min_arity, max_arity] : candidates) {
            s += freq;
            if (c < s) {
                auto amin = std::max(minArity, MinimumArity(node.HashValue));
                auto amax = std::min(maxArity, MaximumArity(node.HashValue));
                auto arity = std::uniform_int_distribution<size_t>(amin, amax)(random);
                result = node;
                result.Arity = static_cast<uint16_t>(arity);
                break;
            }
        }

        ENSURE(IsEnabled(result.HashValue));
        ENSURE(Frequency(result.HashValue) > 0);

        return result;
    }

} // namespace Turingforge