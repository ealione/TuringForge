#pragma once

#include <cstddef>
#include <functional>
#include <utility>
#include <execution>
#include <ranges>
#include <numeric>

#include <Eigen/Dense>
#include <iostream>

#include "function.hpp"
#include "types.hpp"
#include "turing_forge/random/random.hpp"

namespace Turingforge {

    struct Individual {
        // number of nodes (Functions to be applied)
        uint16_t Length = 0UL; // 0-65535

        Turingforge::Scalar Fitness = 0UL;

        // vector of free parameters of the affine combination of
        // the Functions in `Individual`.
        Turingforge::Vector<Turingforge::Scalar> Coefficients;

        // vector of transformation Functions by index.
        Turingforge::Vector<Turingforge::Function> Functions;

        // vector of interaction Functions for each transformation.
        Turingforge::Vector<Turingforge::Vector<Turingforge::Scalar>> Polynomials;

        inline auto operator[](size_t const i) const noexcept -> std::tuple<Turingforge::Scalar, Turingforge::Function, Turingforge::Vector<Turingforge::Scalar>> {
            return std::make_tuple(Coefficients[i], Functions[i], Polynomials[i]);
        };

        [[nodiscard]] inline auto Size() const noexcept -> size_t {
            return Coefficients.size();
        }

        Individual() = default;

        Individual(
                Turingforge::Vector<Turingforge::Scalar> C,
                Turingforge::Vector<Turingforge::Function> F,
                Turingforge::Vector<Turingforge::Vector<Turingforge::Scalar>> P
                ): Length(P.size()), Coefficients(std::move(C)), Functions(std::move(F)), Polynomials(std::move(P)) {};

        [[nodiscard]] auto GetCoefficients() const {
            return Coefficients;
        }

        [[nodiscard]] auto GetFunctions() const {
            return Functions;
        }

        [[nodiscard]] auto GetFunctions(size_t const i) const {
            return Functions[i];
        }

        [[nodiscard]] auto GetPolynomials() const {
            return Polynomials;
        }

        // TODO: should there be a backup here, just in case we want to revert?
        void sanitize() {
            std::vector<size_t> toRemove;
            for(size_t i = 0; i < Polynomials.size(); i++) {
                if (std::all_of(Polynomials[i].begin(), Polynomials[i].end(), [](auto term) { return term == 0; })) {
                    toRemove.push_back(i);
                } else {
                    for (size_t j = i + 1; j < Polynomials.size(); ++j) {
                        if (Polynomials[i] == Polynomials[j] and Functions[i] == Functions[j]) {
                            toRemove.push_back(i);
                        }
                    }
                }
            }

            for (auto& iter : std::ranges::reverse_view(toRemove)) {
                auto index = static_cast<int>(iter);
                Coefficients.erase(Coefficients.begin() + index);
                Functions.erase(Functions.begin() + index);
                Polynomials.erase(Polynomials.begin() + index);
            }

            Length = Polynomials.size();
        }

        [[nodiscard]] Turingforge::Scalar eval(Turingforge::Vector<Turingforge::Scalar> const& X) const {
            // Again this has to be so ugly because C++
            // Once I get zip, ranges and reduce I could re-write this (Or use Zig/Odin;)
            auto vpi = Polynomials.begin();
            return std::transform_reduce(
                std::execution::par,
                Coefficients.begin(), Coefficients.end(),
                Functions.begin(),
                0.,
                std::plus(),
                [&](double c, auto f)
                {
                    auto vp = *vpi++;
                    return c * f(std::transform_reduce(
                            std::execution::par,
                            X.begin(),
                            X.end(),
                            vp.begin(),
                            1.0,
                            std::multiplies(),
                            [](double x, double p) { return std::pow(x, p); }
                    ));
                }
            );
        }

        void fit(const Eigen::Matrix<Turingforge::Scalar, Eigen::Dynamic, 1>& X, const std::vector<Scalar>& y) {
            try {
                Eigen::VectorXd yHat = Eigen::Map<const Eigen::VectorXd>(y.data(), static_cast<Eigen::Index>(y.size()));
                Eigen::Matrix<Turingforge::Scalar, 1, Eigen::Dynamic> XHat(Functions.size());
                std::transform(
                    std::execution::par,
                    Polynomials.begin(), Polynomials.end(),
                    Functions.begin(),
                    XHat.data(),
                    [&](const Vector<Scalar>& p, auto f) {
                       Scalar transformedX = std::transform_reduce(
                               std::execution::par,
                               X.begin(),
                               X.end(),
                               p.begin(),
                               1.0,
                               std::multiplies(),
                               [](double x, double p) { return std::pow(x, p); }
                       );
                       return f(transformedX);
                    }
                );
                Eigen::MatrixXd XsHatT = XHat.transpose();

                Eigen::MatrixXd q1 = (XsHatT * XHat).inverse();
                Eigen::MatrixXd q2 = XsHatT * yHat;

                Eigen::VectorXd ws = q1 * q2;

                Coefficients.resize(ws.size());
                for (int i = 0; i < ws.size(); ++i) {
                    Coefficients[i] = ws[i];
                }
            } catch (const std::exception& e) {
                // Error handling if matrix inversion fails
                std::cerr << "Matrix inversion failed: " << e.what() << std::endl;
                Coefficients.assign(Functions.size(), 1.0);
            }

            Scalar MAE = std::transform_reduce(
                    X.data(), X.data() + X.size(), y.begin(), 0.0, std::plus<>(),
                    [&](Scalar X, Scalar y) {
                        Scalar yHat = eval({X});
                        return std::abs(yHat - y);
                    }
            ) / X.size();

            Scalar score = 1 / (1 + MAE);
            Fitness = std::isfinite(score) ? score : 0.0;
        }
    };

} // namespace Turingforge