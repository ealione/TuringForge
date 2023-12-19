#pragma once

#include "contracts.hpp"
#include "node.hpp"


namespace Turingforge {

    class PrimitiveSet {
        using Primitive = std::tuple<
                Node,
                size_t, // 1: frequency
                size_t, // 2: min arity
                size_t  // 3: max arity
        >;
        enum { NODE = 0, FREQUENCY = 1, MINARITY = 2, MAXARITY = 3}; // for accessing tuple elements more easily

        Turingforge::Map<Turingforge::Hash, Primitive> pset_;

        [[nodiscard]] auto GetPrimitive(Turingforge::Hash hash) const -> Primitive const& {
            auto it = pset_.find(hash);
            if (it == pset_.end()) {
                throw std::runtime_error(fmt::format("Unknown node hash {}\n", hash));
            }
            return it->second;
        }

        auto GetPrimitive(Turingforge::Hash hash) -> Primitive& {
            return const_cast<Primitive&>(const_cast<PrimitiveSet const*>(this)->GetPrimitive(hash)); // NOLINT
        }

    public:
        static constexpr PrimitiveSetConfig Arithmetic = NodeType::Constant | NodeType::Variable | NodeType::Add | NodeType::Sub | NodeType::Mul | NodeType::Div;
        static constexpr PrimitiveSetConfig TypeCoherent = Arithmetic | NodeType::Pow | NodeType::Exp | NodeType::Log | NodeType::Sin | NodeType::Cos | NodeType::Square;
        static constexpr PrimitiveSetConfig Full = TypeCoherent | NodeType::Aq | NodeType::Tan | NodeType::Tanh | NodeType::Sqrt | NodeType::Cbrt;

        PrimitiveSet() = default;

        explicit PrimitiveSet(PrimitiveSetConfig config)
        {
            SetConfig(config);
        }

        [[nodiscard]] auto Primitives() const -> decltype(pset_) const& { return pset_; }

        auto AddPrimitive(Turingforge::Node node, size_t frequency, size_t minArity, size_t maxArity) -> bool
        {
            auto [_, ok] = pset_.insert({ node.HashValue, Primitive { node, frequency, minArity, maxArity } });
            return ok;
        }
        void RemovePrimitive(Turingforge::Node node) { pset_.erase(node.HashValue); }

        void RemovePrimitive(Turingforge::Hash hash) { pset_.erase(hash); }

        void SetConfig(PrimitiveSetConfig config)
        {
            pset_.clear();
            for (size_t i = 0; i < Turingforge::NodeTypes::Count; ++i) {
                auto t = static_cast<Turingforge::NodeType>(1U << i);
                Turingforge::Node n(t);

                if (((1U << i) & static_cast<uint32_t>(config)) != 0U) {
                    pset_[n.HashValue] = { n, 1, n.Arity, n.Arity };
                }
            }
        }

        [[nodiscard]] auto EnabledPrimitives() const -> std::vector<Node> {
            std::vector<Node> nodes;
            for (auto const& [k, v] : pset_) {
                auto [node, freq, min_arity, max_arity] = v;
                if (node.IsEnabled && freq > 0) {
                    nodes.push_back(node);
                }
            }
            return nodes;
        }

        [[nodiscard]] auto Config() const -> PrimitiveSetConfig
        {
            PrimitiveSetConfig conf { static_cast<PrimitiveSetConfig>(0) };
            for (auto [k, v] : pset_) {
                auto const& [node, freq, min_arity, max_arity] = v;
                if (node.IsEnabled && freq > 0) {
                    conf |= node.Type;
                }
            }
            return conf;
        }

        [[nodiscard]] auto Frequency(Turingforge::Hash hash) const -> size_t
        {
            auto const& p = GetPrimitive(hash);
            return std::get<FREQUENCY>(p);
        }

        void SetFrequency(Turingforge::Hash hash, size_t frequency)
        {
            auto& p = GetPrimitive(hash);
            std::get<FREQUENCY>(p) = frequency;
        }

        [[nodiscard]] auto Contains(Turingforge::Hash hash) const -> bool { return pset_.contains(hash); }

        [[nodiscard]] auto IsEnabled(Turingforge::Hash hash) const -> bool
        {
            auto const& p = GetPrimitive(hash);
            return std::get<NODE>(p).IsEnabled;
        }

        void SetEnabled(Turingforge::Hash hash, bool enabled)
        {
            auto& p = GetPrimitive(hash);
            std::get<NODE>(p).IsEnabled = enabled;
        }

        void Enable(Turingforge::Hash hash)
        {
            SetEnabled(hash, /*enabled=*/true);
        }

        void Disable(Turingforge::Hash hash)
        {
            SetEnabled(hash, /*enabled=*/false);
        }

        [[nodiscard]] auto FunctionArityLimits() const -> std::pair<size_t, size_t>
        {
            auto minArity = std::numeric_limits<size_t>::max();
            auto maxArity = std::numeric_limits<size_t>::min();
            for (auto const& [key, val] : pset_) {
                if (std::get<NODE>(val).IsLeaf()) { continue; }
                minArity = std::min(minArity, std::get<MINARITY>(val));
                maxArity = std::max(maxArity, std::get<MAXARITY>(val));
            }
            return { minArity, maxArity };
        }

        OPERON_EXPORT auto SampleRandomSymbol(Turingforge::RandomGenerator& random, size_t minArity, size_t maxArity) const -> Turingforge::Node;

        void SetMinimumArity(Turingforge::Hash hash, size_t minArity)
        {
            EXPECT(minArity <= MaximumArity(hash));
            auto& p = GetPrimitive(hash);
            std::get<MINARITY>(p) = minArity;
        }

        [[nodiscard]] auto MinimumArity(Turingforge::Hash hash) const -> size_t
        {
            auto const& p = GetPrimitive(hash);
            return std::get<MINARITY>(p);
        }

        void SetMaximumArity(Turingforge::Hash hash, size_t maxArity)
        {
            EXPECT(maxArity >= MinimumArity(hash));
            auto& p = GetPrimitive(hash);
            std::get<MAXARITY>(p) = maxArity;
        }

        [[nodiscard]] auto MaximumArity(Turingforge::Hash hash) const -> size_t
        {
            auto const& p = GetPrimitive(hash);
            return std::get<MAXARITY>(p);
        }

        [[nodiscard]] auto MinMaxArity(Turingforge::Hash hash) const -> std::tuple<size_t, size_t>
        {
            auto const& p = GetPrimitive(hash);
            return { std::get<MINARITY>(p), std::get<MAXARITY>(p) };
        }

        void SetMinMaxArity(Turingforge::Hash hash, size_t minArity, size_t maxArity)
        {
            EXPECT(maxArity >= minArity);
            auto& p = GetPrimitive(hash);
            std::get<MINARITY>(p) = minArity;
            std::get<MAXARITY>(p) = maxArity;
        }

        // convenience overloads
        void SetFrequency(Turingforge::Node node, size_t frequency) { SetFrequency(node.HashValue, frequency); }
        [[nodiscard]] auto Frequency(Turingforge::Node node) const -> size_t { return Frequency(node.HashValue); }

        [[nodiscard]] auto Contains(Turingforge::Node node) const -> bool { return Contains(node.HashValue); }
        [[nodiscard]] auto IsEnabled(Turingforge::Node node) const -> bool { return IsEnabled(node.HashValue); }

        void SetEnabled(Turingforge::Node node, bool enabled) { SetEnabled(node.HashValue, enabled); }
        void Enable(Turingforge::Node node) { SetEnabled(node, /*enabled=*/true); }
        void Disable(Turingforge::Node node) { SetEnabled(node, /*enabled=*/false); }

        void SetMinimumArity(Turingforge::Node node, size_t minArity)
        {
            SetMinimumArity(node.HashValue, minArity);
        }
        [[nodiscard]] auto MinimumArity(Turingforge::Node node) const -> size_t { return MinimumArity(node.HashValue); }

        void SetMaximumArity(Turingforge::Node node, size_t maxArity)
        {
            SetMaximumArity(node.HashValue, maxArity);
        }
        [[nodiscard]] auto MaximumArity(Turingforge::Node node) const -> size_t { return MaximumArity(node.HashValue); }

        [[nodiscard]] auto MinMaxArity(Turingforge::Node node) const -> std::tuple<size_t, size_t>
        {
            return MinMaxArity(node.HashValue);
        }
        void SetMinMaxArity(Turingforge::Node node, size_t minArity, size_t maxArity)
        {
            SetMinMaxArity(node.HashValue, minArity, maxArity);
        }
    };
    
}