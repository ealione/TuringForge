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
#include "turing_forge/error_metrics/error_metrics.hpp"
#include "comparison.hpp"

namespace Turingforge {

    struct Individual {
        // domination rank
        size_t Rank{};

        // crowding distance;
        Turingforge::Scalar Distance{};

        // number of nodes (Functions to be applied)
        uint16_t Length = 0UL; // 0-65535

        Turingforge::Vector<Turingforge::Scalar> Fitness;

        // vector of free parameters of the affine combination of the `Functions` in `Individual`.
        Turingforge::Vector<Turingforge::Scalar> Coefficients;

        // vector of transformation Functions by index.
        Turingforge::Vector<Turingforge::Function> Functions;

        // vector of interaction Functions for each transformation.
        Turingforge::Vector<Turingforge::Vector<Turingforge::Scalar>> Polynomials;

        inline auto operator[](size_t const i) noexcept -> Turingforge::Scalar& { return Fitness[i]; }
        inline auto operator[](size_t const i) const noexcept -> Turingforge::Scalar { return Fitness[i]; }

        [[nodiscard]] inline auto Size() const noexcept -> size_t {
            return Fitness.size();
        }

        Individual() = default;

        Individual(
                Turingforge::Vector<Turingforge::Scalar> C,
                Turingforge::Vector<Turingforge::Function> F,
                Turingforge::Vector<Turingforge::Vector<Turingforge::Scalar>> P
        ) : Length(P.size()), Coefficients(std::move(C)), Functions(std::move(F)), Polynomials(std::move(P)) {};

        Individual(const Individual& other)
                : Length(other.Length), Fitness(other.Fitness), Coefficients(other.Coefficients),
                  Functions(other.Functions), Polynomials(other.Polynomials) {}

        Individual(Individual&& other) noexcept
                : Length(other.Length), Fitness(std::move(other.Fitness)), Coefficients(std::move(other.Coefficients)),
                  Functions(std::move(other.Functions)), Polynomials(std::move(other.Polynomials)) {}

        static Individual copyIndividual(const Individual& it) {
            return Individual{it};
        }

        // Copy assignment operator
        Individual& operator=(const Individual& other) {
            if (this != &other) {
                Length = other.Length;
                Fitness = other.Fitness;
                Coefficients = other.Coefficients;
                Functions = other.Functions;
                Polynomials = other.Polynomials;
            }
            return *this;
        }

        // Move assignment operator
        Individual& operator=(Individual&& other) noexcept {
            if (this != &other) {
                Rank = other.Rank;
                Length = other.Length;
                Fitness = std::move(other.Fitness);
                Coefficients = std::move(other.Coefficients);
                Functions = std::move(other.Functions);
                Polynomials = std::move(other.Polynomials);
            }
            return *this;
        }

        [[nodiscard]] inline auto GetAllDataAt(size_t const i) const noexcept -> std::tuple<Turingforge::Scalar, Turingforge::Function, Turingforge::Vector<Turingforge::Scalar>> {
            return std::make_tuple(Coefficients[i], Functions[i], Polynomials[i]);
        };

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

        [[nodiscard]] inline auto GetCoefficientsCount() const {
            return Coefficients.size();
        }

        // TODO: should there be a backup here, just in case we want to revert?
        void sanitize() {
            std::vector<size_t> toRemove;
            for (size_t i = 0; i < Polynomials.size(); i++) {
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

            for (auto &iter: std::ranges::reverse_view(toRemove)) {
                auto index = static_cast<int>(iter);
                Coefficients.erase(Coefficients.begin() + index);
                Functions.erase(Functions.begin() + index);
                Polynomials.erase(Polynomials.begin() + index);
            }

            Length = Polynomials.size();
        }

        [[nodiscard]] Turingforge::Scalar eval(const Eigen::VectorXd& X) const {
            // Again this has to be so ugly because C++
            // Once I get zip, ranges and reduce I could re-write this (Or use Zig/Odin;)
            auto vpi = Polynomials.begin();
            return std::transform_reduce(
                    std::execution::par,
                    Coefficients.begin(), Coefficients.end(),
                    Functions.begin(),
                    0.,
                    std::plus(),
                    [&](double c, auto f) {
                        auto vp = *vpi++;
                        //std::cout << "\nCoefficient " << c << '\n';
                        //std::cout << "X\n" << X << '\n';
                        //std::cout << "Function: " << FunctionTypes::toString(f.Type) << "\n";
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

        void fit(const Eigen::Matrix<Turingforge::Scalar, Eigen::Dynamic, Eigen::Dynamic> &X,
                 const Eigen::Map<const Eigen::Matrix<Turingforge::Scalar, Eigen::Dynamic, 1>> &y) {
            try {
                Eigen::MatrixXd XsHat(X.rows(), Functions.size());
                Eigen::Matrix<Scalar, Eigen::Dynamic, 1> yHat = y;

                for (Eigen::Index i = 0; i < X.rows(); ++i) {
                    for (Eigen::Index j = 0; j < static_cast<Eigen::Index>(Functions.size()); ++j) {
                        Scalar product = 1.0;
                        for (Eigen::Index k = 0; k < X.cols(); ++k) {
                            Scalar powered = std::pow(X(i, k), Polynomials[j][k]);
                            product *= powered;
                        }
                        XsHat(i, j) = Functions[j](product);
                    }
                }

                Eigen::MatrixXd XsHatT = XsHat.transpose();

                Eigen::MatrixXd q1 = (XsHatT * XsHat).inverse();
                Eigen::MatrixXd q2 = XsHatT * yHat;

                Eigen::VectorXd ws = q1 * q2;

                Coefficients.resize(ws.size());
                for (int i = 0; i < ws.size(); ++i) {
                    Coefficients[i] = ws[i];
                }
            } catch (const std::exception &e) {
                // Error handling if matrix inversion fails
                std::cerr << "Matrix inversion failed: " << e.what() << std::endl;
                Coefficients.assign(Functions.size(), 1.0);
            }

            Scalar mae = 0.0;
            for (Eigen::Index i = 0; i < X.rows(); ++i) {
                const auto& X_row = X.row(i);
                Scalar XsHat = eval(X_row);
                Scalar error = std::abs(XsHat - y[i]);
                mae += error;
            }
            mae /= static_cast<Scalar>(X.rows());

            Scalar score = 1 / (1 + mae);
            Fitness[0] = std::isfinite(score) ? score : 0.0;
        }
    };

    struct SingleObjectiveComparison {
        explicit SingleObjectiveComparison(size_t idx)
                : obj_(idx)
        {
        }
        SingleObjectiveComparison()
                : SingleObjectiveComparison(0)
        {
        }

        auto operator()(Individual const& lhs, Individual const& rhs, Turingforge::Scalar eps = 0) const -> bool
        {
            return Turingforge::Less{}(lhs[obj_], rhs[obj_], eps);
        }

        [[nodiscard]] auto GetObjectiveIndex() const -> size_t { return obj_; }
        void SetObjectiveIndex(size_t obj) { obj_ = obj; }

    private:
        size_t obj_; // objective index
    };

    struct LexicographicalComparison {
        auto operator()(Individual const& lhs, Individual const& rhs, Turingforge::Scalar eps = 0) const -> bool
        {
            EXPECT(std::size(lhs.Fitness) == std::size(rhs.Fitness));
            auto const& fit1 = lhs.Fitness;
            auto const& fit2 = rhs.Fitness;
            return Less{}(fit1.begin(), fit1.end(), fit2.begin(), fit2.end(), eps);
        }
    };

    // returns true if lhs dominates rhs
    struct ParetoComparison {
        // assumes minimization in every dimension
        auto operator()(Individual const& lhs, Individual const& rhs, Turingforge::Scalar eps = 0) const -> bool
        {
            EXPECT(std::size(lhs.Fitness) == std::size(rhs.Fitness));
            auto const& fit1 = lhs.Fitness;
            auto const& fit2 = rhs.Fitness;
            return ParetoDominance{}(fit1.begin(), fit1.end(), fit2.begin(), fit2.end(), eps) == Dominance::Left;
        }
    };

    struct CrowdedComparison {
        auto operator()(Individual const& lhs, Individual const& rhs, Turingforge::Scalar eps = 0) const -> bool
        {
            EXPECT(std::size(lhs.Fitness) == std::size(rhs.Fitness));
            if (lhs.Rank != rhs.Rank) { return lhs.Rank < rhs.Rank; }
            return Turingforge::Less{}(rhs.Distance, lhs.Distance, eps);
        }
    };

    using ComparisonCallback = std::function<bool(Individual const&, Individual const&)>;

} // namespace Turingforge