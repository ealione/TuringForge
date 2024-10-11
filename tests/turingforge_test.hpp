#pragma once

#include <fmt/core.h>
#include <fmt/color.h>
#include <fmt/ranges.h>

#include "turing_forge/core/dataset.hpp"
#include "turing_forge/core/individual.hpp"
#include "turing_forge/interpreter/backend/backend.hpp"
#include "turing_forge/interpreter/dual.hpp"

namespace Turingforge::Test::Util {
    inline auto RandomDataset(Turingforge::RandomGenerator& rng, int rows, int cols) -> Turingforge::Dataset {
        std::uniform_real_distribution<Turingforge::Scalar> dist(-1.f, +1.f);
        Eigen::Matrix<decltype(dist)::result_type, -1, -1> data(rows, cols);
        for (auto& v : data.reshaped()) { v = dist(rng); }
        Turingforge::Dataset ds(data);
        return ds;
    }

    template<typename T, std::size_t S = Backend::BatchSize<T>>
    auto EvaluateIndividual(auto const& individual, auto const& dataset, auto const range, T const* coeff, T* out) {
        auto const nrows = range.Size();
        auto const& functions = individual.GetFunctions();
        auto const& polynomials = individual.GetPolynomials();
        auto const& coefficients = individual.GetCoefficients();

        Eigen::Array<T, S, -1> buffer(S, functions.size());
        buffer.setZero();
        Backend::View<T, S> view(buffer.data(), S, functions.size());

        for (auto row = 0UL; row < nrows; row += S) {
            auto const rem = std::min(S, nrows - row);

            for (auto i = 0UL; i < functions.size(); ++i) {
                auto const& function = functions[i];
                auto const& polynomial = polynomials[i];
                auto coeff_value = coefficients[i];

                switch(function.Type) {
                    case FunctionType::Variable: {
                        // Apply polynomial transformation to the variables
                        auto const* ptr = dataset.GetValues(function.HashValue).subspan(row, rem).data();
                        buffer.col(i).segment(0, rem) = Eigen::Map<const Eigen::Array<T, -1, 1>>(ptr, rem, 1).template cast<T>();
                        for (std::size_t j = 0; j < polynomial.size(); ++j) {
                            buffer.col(i).segment(0, rem) = buffer.col(i).segment(0, rem).pow(polynomial[j]);
                        }
                        break;
                    }
                    case FunctionType::Add: {
                        buffer.col(i).setZero();
                        for (auto j = i - function.Arity; j < i; ++j) {
                            buffer.col(i) += buffer.col(j);
                        }
                        break;
                    }
                    case FunctionType::Mul: {
                        buffer.col(i).setOnes();
                        for (auto j = i - function.Arity; j < i; ++j) {
                            buffer.col(i) *= buffer.col(j);
                        }
                        break;
                    }
                    case FunctionType::Sub: {
                        if (function.Arity == 1) {
                            buffer.col(i) = -buffer.col(i-1);
                        } else {
                            buffer.col(i) = buffer.col(i - function.Arity);
                            for (auto j = i - function.Arity + 1; j < i; ++j) {
                                buffer.col(i) -= buffer.col(j);
                            }
                        }
                        break;
                    }
                    case FunctionType::Div: {
                        if (function.Arity == 1) {
                            buffer.col(i) = buffer.col(i-1).inverse();
                        } else {
                            buffer.col(i) = buffer.col(i - function.Arity);
                            for (auto j = i - function.Arity + 1; j < i; ++j) {
                                buffer.col(i) /= buffer.col(j);
                            }
                        }
                        break;
                    }
                    case FunctionType::Fmin: {
                        buffer.col(i) = buffer.col(i - function.Arity);
                        for (auto j = i - function.Arity + 1; j < i; ++j) {
                            buffer.col(i) = buffer.col(i).min(buffer.col(j));
                        }
                        break;
                    }
                    case FunctionType::Fmax: {
                        buffer.col(i) = buffer.col(i - function.Arity);
                        for (auto j = i - function.Arity + 1; j < i; ++j) {
                            buffer.col(i) = buffer.col(i).max(buffer.col(j));
                        }
                        break;
                    }
                    case FunctionType::Aq: {
                        auto j = i - 1;
                        auto k = j - (functions[j].Length + 1);
                        buffer.col(i) = buffer.col(j) / (T{1} + buffer.col(k).square()).sqrt();
                        break;
                    }
                    case FunctionType::Pow: {
                        auto j = i - 1;
                        auto k = j - (functions[j].Length + 1);
                        buffer.col(i) = buffer.col(j).pow(buffer.col(k));
                        break;
                    }
                    case FunctionType::Abs: {
                        buffer.col(i) = buffer.col(i - 1).abs();
                        break;
                    }
                    case FunctionType::Acos: {
                        buffer.col(i) = buffer.col(i - 1).acos();
                        break;
                    }
                    case FunctionType::Asin: {
                        buffer.col(i) = buffer.col(i - 1).asin();
                        break;
                    }
                    case FunctionType::Atan: {
                        buffer.col(i) = buffer.col(i - 1).atan();
                        break;
                    }
                    case FunctionType::Cbrt: {
                        buffer.col(i) = buffer.col(i - 1).unaryExpr([](auto x) { return std::cbrt(x); });
                        break;
                    }
                    case FunctionType::Ceil: {
                        buffer.col(i) = buffer.col(i - 1).ceil();
                        break;
                    }
                    case FunctionType::Cos: {
                        buffer.col(i) = buffer.col(i - 1).cos();
                        break;
                    }
                    case FunctionType::Cosh: {
                        buffer.col(i) = buffer.col(i - 1).cosh();
                        break;
                    }
                    case FunctionType::Exp: {
                        buffer.col(i) = buffer.col(i - 1).exp();
                        break;
                    }
                    case FunctionType::Floor: {
                        buffer.col(i) = buffer.col(i - 1).floor();
                        break;
                    }
                    case FunctionType::Log: {
                        buffer.col(i) = buffer.col(i - 1).log();
                        break;
                    }
                    case FunctionType::Logabs: {
                        buffer.col(i) = buffer.col(i - 1).abs().log();
                        break;
                    }
                    case FunctionType::Log1p: {
                        buffer.col(i) = buffer.col(i - 1).log1p();
                        break;
                    }
                    case FunctionType::Sin: {
                        buffer.col(i) = buffer.col(i - 1).sin();
                        break;
                    }
                    case FunctionType::Sinh: {
                        buffer.col(i) = buffer.col(i - 1).sinh();
                        break;
                    }
                    case FunctionType::Sqrt: {
                        buffer.col(i) = buffer.col(i - 1).sqrt();
                        break;
                    }
                    case FunctionType::Sqrtabs: {
                        buffer.col(i) = buffer.col(i - 1).abs().sqrt();
                        break;
                    }
                    case FunctionType::Square: {
                        buffer.col(i) = buffer.col(i - 1).square();
                        break;
                    }
                    case FunctionType::Tan: {
                        buffer.col(i) = buffer.col(i - 1).tan();
                        break;
                    }
                    case FunctionType::Tanh: {
                        buffer.col(i) = buffer.col(i - 1).tanh();
                        break;
                    }
                    default: {
                        throw std::runtime_error(fmt::format("Unknown function type: {}", function.Name()));
                    }
                }

                // Apply the coefficient scaling
                buffer.col(i) *= coeff_value;
            }

            std::ranges::copy_n(buffer.col(functions.size() - 1).segment(0, rem).data(), rem, out + row);
        }
    }

    auto Autodiff(auto const& tree, auto const& dataset, auto const range) {
        static_assert(std::is_convertible_v<typename Dual::Scalar, Scalar>, "The chosen Jet and Scalar types are not compatible.");
        static_assert(std::is_convertible_v<Scalar, typename Dual::Scalar>, "The chosen Jet and Scalar types are not compatible.");

        auto coeff = tree.GetCoefficients();
        auto* parameters = coeff.data();

        std::vector<Turingforge::Scalar> resid(range.Size());
        auto* residuals = resid.data();

        std::vector<Turingforge::Scalar> jacob(range.Size() * coeff.size());
        auto* jacobian = jacob.data();

        if (parameters == nullptr) {
            return std::tuple{std::move(resid), std::move(jacob)};
        }

        EXPECT(parameters != nullptr);
        EXPECT(residuals != nullptr || jacobian != nullptr);

        std::vector<Dual> inputs(coeff.size());
        for (size_t i = 0; i < inputs.size(); ++i) {
            inputs[i].a = parameters[i];
            inputs[i].v.setZero();
        }
        std::vector<Dual> outputs(range.Size());

        static auto constexpr dim{Dual::DIMENSION};
        Eigen::Map<Eigen::Matrix<Scalar, -1, -1>> jmap(jacobian, outputs.size(), inputs.size());

        auto function = [&](auto const* inputs, auto* outputs) {
            EvaluateTree<Dual>(tree, dataset, range, inputs, outputs);
        };

        for (auto s = 0U; s < inputs.size(); s += dim) {
            auto r = std::min(static_cast<uint32_t>(inputs.size()), s + dim); // remaining parameters

            for (auto i = s; i < r; ++i) {
                inputs[i].v[i - s] = 1.0;
            }

            function(inputs.data(), outputs.data());

            for (auto i = s; i < r; ++i) {
                inputs[i].v[i - s] = 0.0;
            }

            for (auto i = s; i < r; ++i) {
                std::transform(outputs.cbegin(), outputs.cend(), jmap.col(i).data(), [&](auto const& jet) { return jet.v[i - s]; });
            }
        }
        if (residuals != nullptr) {
            std::transform(std::cbegin(outputs), std::cend(outputs), residuals, [](auto const& jet) { return jet.a; });
        }

        return std::tuple{std::move(resid), std::move(jacob)};
    }
} // namespace Turingforge::Test::Util