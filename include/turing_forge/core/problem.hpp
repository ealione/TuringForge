#pragma once

#include <string>
#include <utility>
#include <vector>

#include "fmt/format.h"

#include "variable.hpp"
#include "range.hpp"
#include "primitive_set.hpp"
#include "dataset.hpp"


namespace Turingforge {

    class Problem {
        [[nodiscard]] auto GetVariable(auto t) const -> Turingforge::Variable {
            if (auto v = dataset_.GetVariable(t); v.has_value()) { return *v; }
            throw std::runtime_error(fmt::format("a variable identified by {} {} does not exist in the dataset", typeid(t).name(), t));
        }

        auto HasVariable(auto t) const -> bool {
            return dataset_.GetVariable(t).has_value();
        }

        auto ValidateInputs(auto const& inputs) const {
            using T = typename std::remove_cvref_t<decltype(inputs)>::value_type;
            static_assert(std::is_same_v<T, std::string> || std::is_same_v<T, Turingforge::Hash>, "the inputs must be strings or hashes");
            for (auto const& x : inputs) { (void) GetVariable(x); }
        }

        Dataset dataset_;
        Range training_;
        Range test_;
        Range validation_;

        PrimitiveSet pset_;
        Turingforge::Variable target_;
        Turingforge::Set<Turingforge::Hash> inputs_;

    public:
        Problem(Dataset ds, Range trainingRange, Range testRange, Range validationRange = { 0, 0 }) // NOLINT(bugprone-easily-swappable-parameters)
                : dataset_(std::move(ds))
                , training_(std::move(trainingRange))
                , test_(std::move(testRange))
                , validation_(std::move(validationRange))
        {
            target_ = dataset_.GetVariables().back();
            SetDefaultInputs();
        }

        template<typename T>
        auto SetTarget(T t) {
            target_ = GetVariable<std::remove_cvref_t<T>>(t);
        }

        auto SetTrainingRange(Turingforge::Range range) { training_ = range; }
        auto SetTrainingRange(int begin, int end) { training_ = Turingforge::Range(begin, end); }

        auto SetTestRange(Turingforge::Range range) { test_ = range; }
        auto SetTestRange(int begin, int end) { test_ = Turingforge::Range(begin, end); }

        auto SetValidationRange(Turingforge::Range range) { validation_ = range; }
        auto SetValidationRange(int begin, int end) { validation_ = Turingforge::Range(begin, end); }

        auto SetInputs(auto const& inputs) {
            ValidateInputs(inputs);
            inputs_.clear();
            for (auto const& x : inputs) {
                inputs_.insert(GetVariable(x).Hash);
            }
        }

        [[nodiscard]] auto GetInputs() const -> std::vector<Turingforge::Hash> const& {
            return inputs_.values();
        }

        // set all variables except the target as inputs
        auto SetDefaultInputs() -> void {
            inputs_.clear();
            for (auto const& v : dataset_.GetVariables()) {
                if (v.Hash != target_.Hash) { inputs_.insert(v.Hash); }
            }
        }

        [[nodiscard]] auto TrainingRange() const -> Range { return training_; }
        [[nodiscard]] auto TestRange() const -> Range { return test_; }
        [[nodiscard]] auto ValidationRange() const -> Range { return validation_; }

        [[nodiscard]] auto TargetVariable() const -> Variable const& { return target_; }
        [[nodiscard]] auto InputVariables() const -> std::vector<Variable>
        {
            std::vector<Variable> variables; variables.reserve(inputs_.size());
            std::transform(inputs_.values().begin(), inputs_.values().end(), std::back_inserter(variables),
                           [&](auto h) { return GetVariable<Turingforge::Hash>(h); });
            return variables;
        }

        [[nodiscard]] auto GetPrimitiveSet() const -> PrimitiveSet const& { return pset_; }
        auto GetPrimitiveSet() -> PrimitiveSet& { return pset_; }
        auto ConfigurePrimitiveSet(Turingforge::PrimitiveSetConfig config) { pset_.SetConfig(config); }

        [[nodiscard]] auto GetDataset() const -> Dataset const& { return dataset_; }
        auto GetDataset() -> Dataset& { return dataset_; }

        [[nodiscard]] auto TargetValues() const -> Turingforge::Span<Turingforge::Scalar const> { return dataset_.GetValues(target_.Index); }
        [[nodiscard]] auto TargetValues(Turingforge::Range range) const -> Turingforge::Span<Turingforge::Scalar const> {
            return dataset_.GetValues(target_.Index).subspan(range.Start(), range.Size());
        }

        void StandardizeData(Range range)
        {
            for (auto const& v : inputs_) {
                dataset_.Standardize(GetVariable<Turingforge::Hash>(v).Index, range);
            }
        }

        void NormalizeData(Range range)
        {
            for (auto const& v : inputs_) {
                dataset_.Normalize(GetVariable<Turingforge::Hash>(v).Index, range);
            }
        }
    };

}