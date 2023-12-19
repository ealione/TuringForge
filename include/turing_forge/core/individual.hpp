#pragma once

#include <cstddef>
#include <tuple>

#include "node.hpp"
#include "turing_forge/random/random.hpp"
#include "types.hpp"

namespace Turingforge {

struct Individual {
    // vector of free parameters of the affine combination of
    // the functions in `Individual`.
    Turingforge::Vector<Turingforge::Scalar> coefficients;

    // vector of transformation functions by index.
    Turingforge::Vector<Turingforge::NodeType> functions;

    // vector of interaction functions for each transformation.
    Turingforge::Vector<Turingforge::Vector<Turingforge::Scalar>> polynomials;

  inline auto operator[](size_t const i) const noexcept -> std::tuple<Turingforge::Scalar, Turingforge::NodeType, Turingforge::Vector<Turingforge::Scalar>> {
    return std::make_tuple(coefficients[i], functions[i], polynomials[i]);
  };

  [[nodiscard]] inline auto Size() const noexcept -> size_t {
    return coefficients.size();
  }

  Individual() = default;

  Individual(size_t terms, size_t vars, Turingforge::Scalar expoLimit) {
    coefficients.resize(terms, 1.0);
    functions.resize(terms);
    Turingforge::RandomGenerator rng{0};
    std::generate(functions.begin(), functions.end(), [&]() { return Turingforge::Random::RandomSymbol(rng); });
    polynomials.resize(terms);
    for (auto &poly : polynomials) {
      poly.resize(vars);
      std::generate(poly.begin(), poly.end(), [&]() { return Turingforge::Random::Uniform(rng, 0.0F, +1.0F) * 2 * (expoLimit + 1) - 1 - expoLimit; });
    }
  }
};

} // namespace Turingforge