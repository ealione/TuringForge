#pragma once

#include <cstddef>
#include <tuple>

#include "types.hpp"
#include "functions.hpp"


namespace Turingforge
{

  struct Expression
  {
    Turingforge::Vector<Turingforge::Scalar> coefficients;                     // vector of free parameters of the affine combination of the functions in `Expression`.
    Turingforge::Vector<int> functions;                                        // vector of transformation functions by index.
    Turingforge::Vector<Turingforge::Vector<Turingforge::Scalar>> polynomials; // vector of interaction functions for each transformation.

    inline auto operator[](size_t const i) noexcept -> std::tuple<Turingforge::Vector<Turingforge::Scalar>, Turingforge::Vector<int>, Turingforge::Vector<Turingforge::Vector<Turingforge::Scalar>>> & { return std::make_tuple(coefficients[i], functions[i], polynomials[i]); };
    inline auto operator[](size_t const i) const noexcept -> std::tuple<Turingforge::Vector<Turingforge::Scalar>, Turingforge::Vector<int>, Turingforge::Vector<Turingforge::Vector<Turingforge::Scalar>>> { return std::make_tuple(coefficients[i], functions[i], polynomials[i]); };

    [[nodiscard]] inline auto Size() const noexcept -> size_t { return coefficients.size(); }

    Expression() = default;
    Expression(size_t terms, size_t vars)
    {
      coefficients.resize(terms, 1.0);
      functions.resize(terms);
      std::generate(functions, Turingforge::RandomScalar());
      polynomials.resize(terms);
      for (auto &poly : polynomials)
      {
        poly.resize(terms);
        std::generate(poly, Turingforge::RandomScalar());
      }
    }
  };

}
