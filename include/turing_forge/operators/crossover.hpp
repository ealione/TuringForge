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
    auto FindCompatibleSwapLocations(Turingforge::RandomGenerator& random, size_t lhsLength, size_t rhsLength) const -> std::pair<size_t, size_t>;

    static inline auto Cross(const Individual& lhs, const Individual& rhs, size_t i, size_t j, size_t maxLength) -> Individual {
        auto const& left = lhs;
        auto const& right = rhs;
        using signed_t = std::make_signed_t<size_t>;

        // Ensure crossover points are within bounds
        i = std::min(i, static_cast<size_t>(left.Length) - 1);
        j = std::min(j, static_cast<size_t>(right.Length) - 1);

        // Calculate the sizes for each part
        size_t leftSize = i;
        size_t rightSize = right.Length - j;

        // Create the new vectors with the correct size
        size_t childSize = std::min(i + (right.Length - j), maxLength);
        Vector<Scalar> coefficients(childSize);
        Vector<Function> functions(childSize);
        Vector<Vector<Scalar>> polynomials(childSize);

        auto copyRange = [](const auto& source, auto& dest, size_t sourceStart, size_t count, size_t destStart) {
            std::copy_n(source.begin() + static_cast<std::make_signed_t<size_t>>(sourceStart),
                        std::min(count, dest.size() - destStart),
                        dest.begin() + static_cast<std::make_signed_t<size_t>>(destStart));
        };

        // Copy left part
        copyRange(left.Coefficients, coefficients, 0, leftSize, 0);
        copyRange(left.Functions, functions, 0, leftSize, 0);
        copyRange(left.Polynomials, polynomials, 0, leftSize, 0);

        // Copy right part
        copyRange(right.Coefficients, coefficients, j, rightSize, leftSize);
        copyRange(right.Functions, functions, j, rightSize, leftSize);
        copyRange(right.Polynomials, polynomials, j, rightSize, leftSize);

        // Create and return the new Individual
        Individual child;
        child.Coefficients = std::move(coefficients);
        child.Functions = std::move(functions);
        child.Polynomials = std::move(polynomials);
        child.Length = childSize;

        return child;
    }

    [[nodiscard]] auto InternalProbability() const -> double { return internalProbability_; }
    [[nodiscard]] auto MaxLength() const -> size_t { return maxLength_; }

private:
    double internalProbability_;
    size_t maxLength_;
};

class FixedPointCrossover : public CrossoverBase {
public:
    FixedPointCrossover(double p, size_t l)
        : internalProbability_(p)
        , maxLength_(l)
    {
    }
    auto operator()(Turingforge::RandomGenerator& random, const Individual& lhs, const Individual& rhs) const -> Individual override;
    auto FindCompatibleSwapLocations(Turingforge::RandomGenerator& random, size_t lhsLength, size_t rhsLength) const -> std::pair<size_t, size_t>;

    static inline auto Cross(const Individual& lhs, const Individual& rhs, size_t i, size_t j) -> Individual {
        auto const& left = lhs;
        auto const& right = rhs;
        using signed_t = std::make_signed_t<size_t>;

        // Calculate the sizes for each part
        size_t leftPrefixSize = i;
        size_t rightSuffixSize = right.Length - j;

        // Calculate the total size needed
        size_t totalSize = leftPrefixSize + rightSuffixSize;

        // Create the new vectors with the correct size
        Vector<Scalar> coefficients(totalSize);
        Vector<Function> functions(totalSize);
        Vector<Vector<Scalar>> polynomials(totalSize);

        auto copyRange = [](const auto& source, auto& dest, size_t sourceStart, size_t count, size_t destStart) {
            std::copy_n(source.begin() + static_cast<signed_t>(sourceStart), count, dest.begin() + static_cast<signed_t>(destStart));
        };

        // Copy left prefix
        copyRange(left.Coefficients, coefficients, 0, leftPrefixSize, 0);
        copyRange(left.Functions, functions, 0, leftPrefixSize, 0);
        copyRange(left.Polynomials, polynomials, 0, leftPrefixSize, 0);

        // Copy right suffix
        copyRange(right.Coefficients, coefficients, j, rightSuffixSize, leftPrefixSize);
        copyRange(right.Functions, functions, j, rightSuffixSize, leftPrefixSize);
        copyRange(right.Polynomials, polynomials, j, rightSuffixSize, leftPrefixSize);

        // Create and return the new Individual
        Individual child;
        child.Coefficients = std::move(coefficients);
        child.Functions = std::move(functions);
        child.Polynomials = std::move(polynomials);
        child.Length = totalSize;

        return child;
    }

    [[nodiscard]] auto InternalProbability() const -> double { return internalProbability_; }
    [[nodiscard]] auto MaxLength() const -> size_t { return maxLength_; }

private:
    double internalProbability_;
    size_t maxLength_;
};

class UniformCrossover : public CrossoverBase {
public:
    UniformCrossover(double p, size_t l, double swapProb = 0.5)
        : internalProbability_(p)
        , maxLength_(l)
        , swapProbability_(swapProb)
    {
    }

    using BernoulliTrial = std::bernoulli_distribution;
    auto operator()(Turingforge::RandomGenerator& random, const Individual& lhs, const Individual& rhs) const -> Individual override {
        if (BernoulliTrial{internalProbability_}(random)) {
            return Cross(random, lhs, rhs, maxLength_, swapProbability_);
        }
        return BernoulliTrial{0.5}(random) ? lhs : rhs;
    }

    static inline auto Cross(Turingforge::RandomGenerator& random, const Individual& lhs, const Individual& rhs, size_t maxLength, double swapProbability) -> Individual {
        size_t childLength = std::min({static_cast<size_t>(lhs.Length), static_cast<size_t>(rhs.Length), maxLength});

        Vector<Scalar> coefficients(childLength);
        Vector<Function> functions(childLength);
        Vector<Vector<Scalar>> polynomials(childLength);

        for (size_t i = 0; i < childLength; ++i) {
            if (BernoulliTrial{swapProbability}(random)) {
                coefficients[i] = lhs.Coefficients[i];
                functions[i] = lhs.Functions[i];
                polynomials[i] = lhs.Polynomials[i];
            } else {
                coefficients[i] = rhs.Coefficients[i];
                functions[i] = rhs.Functions[i];
                polynomials[i] = rhs.Polynomials[i];
            }
        }

        Individual child;
        child.Coefficients = std::move(coefficients);
        child.Functions = std::move(functions);
        child.Polynomials = std::move(polynomials);
        child.Length = childLength;

        return child;
    }

    [[nodiscard]] auto InternalProbability() const -> double { return internalProbability_; }
    [[nodiscard]] auto MaxLength() const -> size_t { return maxLength_; }
    [[nodiscard]] auto SwapProbability() const -> double { return swapProbability_; }

private:
    double internalProbability_;
    size_t maxLength_;
    double swapProbability_;
};

class MultiPointCrossover : public CrossoverBase {
public:
    MultiPointCrossover(double p, size_t l, size_t numPoints = 2)
        : internalProbability_(p)
        , maxLength_(l)
        , numCrossoverPoints_(numPoints)
    {
    }

    using BernoulliTrial = std::bernoulli_distribution;
    auto operator()(Turingforge::RandomGenerator& random, const Individual& lhs, const Individual& rhs) const -> Individual override {
        if (BernoulliTrial{internalProbability_}(random)) {
            return Cross(random, lhs, rhs, maxLength_, numCrossoverPoints_);
        }
        return BernoulliTrial{0.5}(random) ? lhs : rhs;
    }

    static inline auto Cross(Turingforge::RandomGenerator& random, const Individual& lhs, const Individual& rhs, size_t maxLength, size_t numCrossoverPoints) -> Individual {
        size_t childLength = std::min({static_cast<size_t>(lhs.Length), static_cast<size_t>(rhs.Length), maxLength});

        // Generate crossover points
        std::vector<size_t> crossoverPoints;
        for (size_t i = 0; i < numCrossoverPoints; ++i) {
            crossoverPoints.push_back(std::uniform_int_distribution<size_t>(1, childLength - 1)(random));
        }
        std::ranges::sort(crossoverPoints.begin(), crossoverPoints.end());

        Vector<Scalar> coefficients(childLength);
        Vector<Function> functions(childLength);
        Vector<Vector<Scalar>> polynomials(childLength);

        bool useLeft = true;
        size_t lastPoint = 0;

        for (size_t point : crossoverPoints) {
            auto& source = useLeft ? lhs : rhs;
            std::copy(source.Coefficients.begin() + static_cast<std::ptrdiff_t>(lastPoint),
                      source.Coefficients.begin() + static_cast<std::ptrdiff_t>(point),
                      coefficients.begin() + static_cast<std::ptrdiff_t>(lastPoint));
            std::copy(source.Functions.begin() + static_cast<std::ptrdiff_t>(lastPoint),
                      source.Functions.begin() + static_cast<std::ptrdiff_t>(point),
                      functions.begin() + static_cast<std::ptrdiff_t>(lastPoint));
            std::copy(source.Polynomials.begin() + static_cast<std::ptrdiff_t>(lastPoint),
                      source.Polynomials.begin() + static_cast<std::ptrdiff_t>(point),
                      polynomials.begin() + static_cast<std::ptrdiff_t>(lastPoint));

            lastPoint = point;
            useLeft = !useLeft;
        }

        // Copy the last segment
        auto& source = useLeft ? lhs : rhs;
        std::copy(source.Coefficients.begin() + static_cast<std::ptrdiff_t>(lastPoint),
                  source.Coefficients.begin() + static_cast<std::ptrdiff_t>(childLength),
                  coefficients.begin() + static_cast<std::ptrdiff_t>(lastPoint));
        std::copy(source.Functions.begin() + static_cast<std::ptrdiff_t>(lastPoint),
                  source.Functions.begin() + static_cast<std::ptrdiff_t>(childLength),
                  functions.begin() + static_cast<std::ptrdiff_t>(lastPoint));
        std::copy(source.Polynomials.begin() + static_cast<std::ptrdiff_t>(lastPoint),
                  source.Polynomials.begin() + static_cast<std::ptrdiff_t>(childLength),
                  polynomials.begin() + static_cast<std::ptrdiff_t>(lastPoint));

        Individual child;
        child.Coefficients = std::move(coefficients);
        child.Functions = std::move(functions);
        child.Polynomials = std::move(polynomials);
        child.Length = childLength;

        return child;
    }

    [[nodiscard]] auto InternalProbability() const -> double { return internalProbability_; }
    [[nodiscard]] auto MaxLength() const -> size_t { return maxLength_; }
    [[nodiscard]] auto NumCrossoverPoints() const -> size_t { return numCrossoverPoints_; }

private:
    double internalProbability_;
    size_t maxLength_;
    size_t numCrossoverPoints_;
};
} // namespace Turingforge