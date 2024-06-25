#pragma once

#include <vector>

#include "turing_forge/core/operator.hpp"
#include "turing_forge/core/individual.hpp"

namespace Turingforge {
// crossover takes two parent trees and returns a child
struct CrossoverBase : public OperatorBase<Individual, const Individual&, const Individual&> {
};

class IndividualCrossover : public CrossoverBase {
public:
    IndividualCrossover(double p, size_t l)
        : internalProbability_(p)
        , maxLength_(l)
    {
    }
    auto operator()(Turingforge::RandomGenerator& random, const Individual& lhs, const Individual& rhs) const -> Individual override;
    auto FindCompatibleSwapLocations(Turingforge::RandomGenerator& random, const Individual& lhs, const Individual& rhs) const -> std::pair<size_t, size_t>;

    static inline auto Cross(const Individual& lhs, const Individual& rhs, /* index of subtree 1 */ size_t i, /* index of subtree 2 */ size_t j) -> Individual
    {
        auto const& left = lhs;
        auto const& right = rhs;
        using signed_t = std::make_signed<size_t>::type; // NOLINT

        // Calculate the total sizes needed
        size_t coeffSize = (i - left.Length) + (right.Length + 1) + (left.Coefficients.size() - (i + 1));
        size_t funcSize = (i - left.Length) + (right.Length + 1) + (left.Functions.size() - (i + 1));
        size_t polySize = (i - left.Length) + (right.Length + 1) + (left.Polynomials.size() - (i + 1));

        // Resize the vectors
        Turingforge::Vector<Scalar> coefficients(coeffSize);
        Turingforge::Vector<Function> functions(funcSize);
        Turingforge::Vector<Turingforge::Vector<Scalar>> polynomials(polySize);

        //size_t totalSize = (i - left.Length) + (right.Length + 1) + (left.Coefficients.size() - (i + 1));
        //coefficients.reserve(right.Length - left.Length + sizeof(Scalar));
        //coefficients.resize(totalSize);

        auto coeffIt = coefficients.begin();
        auto funcIt = functions.begin();
        auto polyIt = polynomials.begin();

        // Copy Coefficients
        //std::copy_n(left.Coefficients.begin(), i - left.Length, back_inserter(coefficients));
        //std::copy_n(right.Coefficients.begin() + static_cast<signed_t>(j) - right.Length, right.Length + 1, back_inserter(coefficients));
        //std::copy_n(left.Coefficients.begin() + static_cast<signed_t>(i) + 1, left.Coefficients.size() - (i + 1), back_inserter(coefficients));
        coeffIt = std::copy_n(left.Coefficients.begin(), i - left.Length, coeffIt);
        coeffIt = std::copy_n(right.Coefficients.begin() + static_cast<signed_t>(j) - right.Length, right.Length + 1, coeffIt);
        std::copy_n(left.Coefficients.begin() + static_cast<signed_t>(i) + 1, left.Coefficients.size() - (i + 1), coeffIt);

        // Copy Functions
        funcIt = std::copy_n(left.Functions.begin(), i - left.Length, funcIt);
        funcIt = std::copy_n(right.Functions.begin() + static_cast<signed_t>(j) - right.Length, right.Length + 1, funcIt);
        std::copy_n(left.Functions.begin() + static_cast<signed_t>(i) + 1, left.Functions.size() - (i + 1), funcIt);

        // Copy Polynomials
        polyIt = std::copy_n(left.Polynomials.begin(), i - left.Length, polyIt);
        polyIt = std::copy_n(right.Polynomials.begin() + static_cast<signed_t>(j) - right.Length, right.Length + 1, polyIt);
        std::copy_n(left.Polynomials.begin() + static_cast<signed_t>(i) + 1, left.Polynomials.size() - (i + 1), polyIt);

        auto child = Individual();
        child.Coefficients = std::move(coefficients);
        child.Functions = std::move(functions);
        child.Polynomials = std::move(polynomials);

        return child;
    }

    [[nodiscard]] auto InternalProbability() const -> double { return internalProbability_; }
    [[nodiscard]] auto MaxLength() const -> size_t { return maxLength_; }

private:
    double internalProbability_;
    size_t maxLength_;
};
} // namespace Turingforge