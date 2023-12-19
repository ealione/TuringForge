#pragma once

#include <cstddef>
#include <tuple>

#include "functions.hpp"
#include "random.hpp"
#include "types.hpp"

namespace Turingforge {

struct Expression {
  Turingforge::Vector<Turingforge::Scalar>
      coefficients; // vector of free parameters of the affine combination of
                    // the functions in `Expression`.
  Turingforge::Vector<int>
      functions; // vector of transformation functions by index.
  Turingforge::Vector<Turingforge::Vector<Turingforge::Scalar>>
      polynomials; // vector of interaction functions for each transformation.

  inline auto operator[](size_t const i) const noexcept
      -> std::tuple<Turingforge::Scalar, int,
                    Turingforge::Vector<Turingforge::Scalar>> {
    return std::make_tuple(coefficients[i], functions[i], polynomials[i]);
  };

  [[nodiscard]] inline auto Size() const noexcept -> size_t {
    return coefficients.size();
  }

  Expression() = default;

  Expression(size_t terms, size_t vars, int expoLimit) {
    coefficients.resize(terms, 1.0);
    functions.resize(terms);
    std::generate(functions.begin(), functions.end(), );
    polynomials.resize(terms);
    for (auto &poly : polynomials) {
      poly.resize(terms);
      std::generate(poly.begin(), poly.end(),
                    Turingforge::RandomScalar(0.0F, +1.0F) * 2 *
                            (expoLimit + 1) -
                        1 - expoLimit);
    }
  }
};

} // namespace Turingforge
