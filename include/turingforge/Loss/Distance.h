#pragma once

#include <cmath>
#include <stdexcept>

#include "Traits.h"
#include "Utils.h"

/*
 * LPDistLoss{P} : DistanceLoss
 *
 * The P-th power absolute distance loss. It is Lipschitz continuous
 * iff `P == 1`, convex if and only if `P >= 1`, and strictly convex
 * iff `P > 1`.
 *
 * ```math
 * L(r) = |r|^P
 * ```
 */

struct LPDistLoss : DistanceLoss {
    double p;

    explicit LPDistLoss(double p) : p(p) {}

    constexpr double operator()(double difference) const override {
        return std::pow(std::abs(difference), p);
    }

    [[nodiscard]] double deriv(double difference) const {
        if (difference == 0) {
            return double(0);
        } else {
            return p * difference * std::pow(std::abs(difference), p - 2);
        }
    }

    [[nodiscard]] double deriv2(double difference) const {
        if (difference == 0) {
            return double(0);
        } else {
            return (std::pow(p, 2) - p) * std::pow(std::abs(difference), p) / std::pow(difference, 2);
        }
    }

    constexpr bool isminimizable() override {
        return true;
    }

    constexpr bool issymmetric() override {
        return true;
    }

    [[nodiscard]] constexpr bool isdifferentiable() override {
        return p > 1;
    }

    [[nodiscard]] constexpr bool isdifferentiable(double at) override {
        return p > 1 || at != 0;
    }

    [[nodiscard]] constexpr bool istwicedifferentiable() override {
        return p > 1;
    }

    [[nodiscard]] constexpr bool istwicedifferentiable(double at) override {
        return p > 1 || at != 0;
    }

    [[nodiscard]] constexpr bool islipschitzcont() override {
        return p == 1;
    }

    [[nodiscard]] constexpr bool islocallylipschitzcont() override {
        return p >= 1;
    }

    [[nodiscard]] constexpr bool isconvex() override {
        return p >= 1;
    }

    [[nodiscard]] constexpr bool isstrictlyconvex() override {
        return p > 1;
    }

    [[nodiscard]] constexpr bool isstronglyconvex() override {
        return p >= 2;
    }
};

/*
 * L1DistLoss : DistanceLoss
 *
 * The absolute distance loss.
 * Special case of the [`LPDistLoss`] with `P=1`.
 * It is Lipschitz continuous and convex, but not strictly convex.
 *
 * ```math
 * L(r) = |r|
 * ```
 */

struct L1DistLoss : LPDistLoss {
    explicit L1DistLoss(double p = 1) : LPDistLoss(p) {}

    constexpr double operator()(double difference) const override {
        return std::abs(difference);
    }

    constexpr double deriv(double difference) override {
        return std::signbit(difference) ? -1 : 1;
    }

    constexpr double deriv2(double) override {
        return double(0);
    }

    constexpr bool isdifferentiable() override {
        return false;
    }

    constexpr bool isdifferentiable(double at) override {
        return at != 0;
    }

    constexpr bool istwicedifferentiable() override {
        return false;
    }

    constexpr bool istwicedifferentiable(double at) override {
        return at != 0;
    }

    constexpr bool islipschitzcont() override {
        return true;
    }

    constexpr bool isconvex() override {
        return true;
    }

    constexpr bool isstrictlyconvex() override {
        return false;
    }

    constexpr bool isstronglyconvex() override {
        return false;
    }
};

/*
 * L2DistLoss : DistanceLoss
 *
 * The least squares loss.
 * Special case of the [`LPDistLoss`] with `P=2`.
 * It is strictly convex.
 *
 * ```math
 * L(r) = |r|^2
 * ```
 */

struct L2DistLoss : LPDistLoss {
    explicit L2DistLoss(double p = 2) : LPDistLoss(p) {}
    constexpr double operator()(double difference) const override {
        return std::abs(difference) * std::abs(difference);
    }

    constexpr double deriv(double difference) override {
        return 2.0 * difference;
    }

    constexpr double deriv2(double) override {
        return 2.0;
    }

    constexpr bool isdifferentiable() override {
        return true;
    }

    constexpr bool isdifferentiable(double) override {
        return true;
    }

    constexpr bool istwicedifferentiable() override {
        return true;
    }

    constexpr bool istwicedifferentiable(double) override {
        return true;
    }

    constexpr bool islipschitzcont() override {
        return false;
    }

    constexpr bool isconvex() override {
        return true;
    }

    constexpr bool isstrictlyconvex() override {
        return true;
    }

    constexpr bool isstronglyconvex() override {
        return true;
    }
};

/*
 * PeriodicLoss : DistanceLoss
 *
 * Measures distance on a circle of specified circumference `c`.
 *
 * ```math
 * L(r) = 1 - \cos \left( \frac{2 r \pi}{c} \right)
 * ```
 */

struct PeriodicLoss : DistanceLoss {
    double k;  // k = 2π / circumference

    explicit PeriodicLoss(double circ) {
        if (circ <= 0) {
            throw std::invalid_argument("circumference should be strictly positive");
        }
        k = static_cast<double>(2 * M_PI / circ);
    }

    constexpr double operator()(double difference) const override {
        return 1 - std::cos(difference * k);
    }

    [[nodiscard]] double deriv(double difference) const {
        return k * std::sin(difference * k);
    }

    [[nodiscard]] double deriv2(double difference) const {
        return std::pow(k, 2) * std::cos(difference * k);
    }

    constexpr bool isdifferentiable() override {
        return true;
    }

    constexpr bool isdifferentiable(double) override {
        return true;
    }

    constexpr bool istwicedifferentiable() override {
        return true;
    }

    constexpr bool istwicedifferentiable(double) override {
        return true;
    }

    constexpr bool islipschitzcont() override {
        return true;
    }

    constexpr bool isconvex() override {
        return false;
    }

    constexpr bool isstrictlyconvex() override {
        return false;
    }

    constexpr bool isstronglyconvex() override {
        return false;
    }
};

/*
 * HuberLoss : DistanceLoss
 *
 * Loss function commonly used for robustness to outliers.
 * For large values of `d` it becomes close to the [`L1DistLoss`],
 * while for small values of `d` it resembles the [`L2DistLoss`].
 * It is Lipschitz continuous and convex, but not strictly convex.
 *
 * ```math
 * L(r) = \begin{cases} \frac{r^2}{2} & \quad \text{if } | r | \le \alpha \\ \alpha | r | - \frac{\alpha^3}{2} & \quad \text{otherwise}\\ \end{cases}
 * ```
 */

struct HuberLoss : DistanceLoss {
    double d;  // boundary between quadratic and linear loss

    explicit HuberLoss(double d) {
        if (d <= 0) {
            throw std::invalid_argument("Huber crossover parameter must be strictly positive.");
        }
        this->d = d;
    }

    constexpr double operator()(double difference) const override {
        auto abs_diff = std::abs(difference);
        if (abs_diff <= d) {
            return static_cast<double>(0.5) * std::pow(difference, 2);  // quadratic
        } else {
            return (d * abs_diff) - static_cast<double>(0.5) * std::pow(d, 2);  // linear
        }
    }

    [[nodiscard]] auto deriv(double difference) const {
        if (std::abs(difference) <= d) {
            return difference;  // quadratic
        } else {
            return d * static_cast<double>(std::signbit(difference) ? -1 : 1);  // linear
        }
    }

    [[nodiscard]] auto deriv2(double difference) const {
        return std::abs(difference) <= d ? double(1) : double(0);
    }

    constexpr bool isdifferentiable() override {
        return true;
    }

    constexpr bool isdifferentiable(double) override {
        return true;
    }

    constexpr bool istwicedifferentiable() override {
        return false;
    }

    [[nodiscard]] bool istwicedifferentiable(double at) const {
        return at != std::abs(d);
    }

    constexpr bool islipschitzcont() override {
        return true;
    }

    constexpr bool isconvex() override {
        return true;
    }

    constexpr bool isstrictlyconvex() override {
        return false;
    }

    constexpr bool isstronglyconvex() override {
        return false;
    }

    constexpr bool issymmetric() override {
        return true;
    }
};

/*
 * L1EpsilonInsLoss : DistanceLoss
 *
 * The ``ϵ``-insensitive loss. Typically used in linear support vector
 * regression. It ignores deviances smaller than ``ϵ``, but penalizes
 * larger deviances linarily.
 * It is Lipschitz continuous and convex, but not strictly convex.
 *
 * ```math
 * L(r) = \max \{ 0, | r | - \epsilon \}
 * ```
 */

struct L1EpsilonInsLoss : DistanceLoss {
    double eps;

    explicit L1EpsilonInsLoss(double eps) {
        if (eps <= 0) {
            throw std::invalid_argument("eps must be strictly positive");
        }
        this->eps = eps;
    }

    constexpr double operator()(double difference) const override {
        return std::max(double(0), std::abs(difference) - eps);
    }

    [[nodiscard]] double deriv(double difference) const {
        return std::abs(difference) <= eps ? double(0) : static_cast<double>(std::signbit(difference) ? -1 : 1);
    }

    constexpr double deriv2(double) override {
        return double(0);
    }

    constexpr bool issymmetric() override {
        return true;
    }

    constexpr bool isdifferentiable() override {
        return false;
    }

    [[nodiscard]] bool isdifferentiable(double at) const {
        return std::abs(at) != eps;
    }

    constexpr bool istwicedifferentiable() override {
        return false;
    }

    [[nodiscard]] bool istwicedifferentiable(double at) const {
        return std::abs(at) != eps;
    }

    constexpr bool islipschitzcont() override {
        return true;
    }

    constexpr bool isconvex() override {
        return true;
    }

    constexpr bool isstrictlyconvex() override {
        return false;
    }

    constexpr bool isstronglyconvex() override {
        return false;
    }
};

struct EpsilonInsLoss : L1EpsilonInsLoss {
    explicit EpsilonInsLoss(double eps) : L1EpsilonInsLoss(eps) {}
};

/*
 * L2EpsilonInsLoss : DistanceLoss
 *
 * The quadratic ``ϵ``-insensitive loss.
 * Typically used in linear support vector regression.
 * It ignores deviances smaller than ``ϵ``, but penalizes
 * larger deviances quadratically. It is convex, but not strictly convex.
 *
 * ```math
 * L(r) = \max \{ 0, | r | - \epsilon \}^2
 * ```
 */

struct L2EpsilonInsLoss : DistanceLoss {
    double eps;

    explicit L2EpsilonInsLoss(double eps) {
        if (eps <= 0) {
            throw std::invalid_argument("eps must be strictly positive");
        }
        this->eps = eps;
    }

    constexpr double operator()(double difference) const override {
        return std::abs(std::pow(std::max(double(0), std::abs(difference) - eps), 2));
    }

    [[nodiscard]] double deriv(double difference) const {
        auto abs_diff = std::abs(difference);
        return abs_diff <= eps ? double(0) : double(2) * std::copysign(abs_diff - eps, difference);
    }

    [[nodiscard]] double deriv2(double difference) const {
        return std::abs(difference) <= eps ? double(0) : double(2);
    }

    constexpr bool issymmetric() override {
        return true;
    }

    constexpr bool isdifferentiable() override {
        return true;
    }

    constexpr bool isdifferentiable(double) override {
        return true;
    }

    constexpr bool istwicedifferentiable() override {
        return false;
    }

    [[nodiscard]] bool istwicedifferentiable(double at) const {
        return std::abs(at) != eps;
    }

    constexpr bool islipschitzcont() override {
        return false;
    }

    constexpr bool isconvex() override {
        return true;
    }

    constexpr bool isstrictlyconvex() override {
        return true;
    }

    constexpr bool isstronglyconvex() override {
        return true;
    }
};

/*
 * LogitDistLoss : DistanceLoss
 *
 * The distance-based logistic loss for regression.
 * It is strictly convex and Lipschitz continuous.
 *
 * ```math
 * L(r) = - \ln \frac{4 e^r}{(1 + e^r)^2}
 * ```
 */

struct LogitDistLoss : DistanceLoss {
    double operator()(double difference) const override {
        auto er = std::exp(difference);
        return -std::log(double(4)) - difference + 2 * std::log(double(1) + er);
    }

    constexpr double deriv(double difference) override {
        return std::tanh(difference / double(2));
    }

    constexpr double deriv2(double difference) override {
        auto er = std::exp(difference);
        return double(2) * er / std::abs(std::pow(double(1) + er, 2));
    }

    constexpr bool issymmetric() override {
        return true;
    }

    constexpr bool isdifferentiable() override {
        return true;
    }

    constexpr bool isdifferentiable(double) override {
        return true;
    }

    constexpr bool istwicedifferentiable() override {
        return true;
    }

    constexpr bool istwicedifferentiable(double) override {
        return true;
    }

    constexpr bool islipschitzcont() override {
        return true;
    }

    constexpr bool isconvex() override {
        return true;
    }

    constexpr bool isstrictlyconvex() override {
        return true;
    }

    constexpr bool isstronglyconvex() override {
        return false;
    }
};

/*
 * QuantileLoss : DistanceLoss
 *
 * The distance-based quantilealso known as pinball loss,
 * can be used to estimate conditional tau-quantiles.
 * It is Lipschitz continuous and convex, but not strictly convex.
 * Furthermore, it is symmetric if and only if `tau = 1/2`.
 *
 * ```math
 * L(r) = \begin{cases} -\left( 1 - \tau  \right) r & \quad \text{if } r < 0 \\ \tau r & \quad \text{if } r \ge 0 \\ \end{cases}
 * ```
 */

struct QuantileLoss : DistanceLoss {
    double tau;

    explicit QuantileLoss(double tau) : tau(tau) {}

    constexpr double operator()(double diff) {
        return diff * (diff > 0 ? 1 - tau : -tau);
    }

    [[nodiscard]] double deriv(double diff) const {
        return (diff > 0 ? 1 - tau : -tau);
    }

    constexpr double deriv2(double) override {
        return 0.0;
    }

    [[nodiscard]] bool issymmetric() const {
        return tau == 0.5;
    }

    constexpr bool isdifferentiable() override {
        return false;
    }

    constexpr bool isdifferentiable(double at) override {
        return at != 0.0;
    }

    constexpr bool istwicedifferentiable() override {
        return false;
    }

    constexpr bool istwicedifferentiable(double at) override {
        return at != 0.0;
    }

    constexpr bool islipschitzcont() override {
        return true;
    }

    static bool islipschitzcont_deriv(const QuantileLoss&) {
        return true;
    }

    constexpr bool isconvex() override {
        return true;
    }

    constexpr bool isstrictlyconvex() override {
        return false;
    }

    constexpr bool isstronglyconvex() override {
        return false;
    }
};

/*
 * LogCoshLoss : DistanceLoss
 *
 * The log cosh loss is twice differentiable, strongly convex,
 * Lipschitz continous function.
 *
 * ```math
 * L(r) = log ( cosh ( x ))
 * ```
 */

struct LogCoshLoss : DistanceLoss {
    constexpr double operator()(double diff) const override {
        return log_cosh(diff);
    }

    constexpr double deriv(double diff) override {
        return std::tanh(diff);
    }

    constexpr double deriv2(double diff) override {
        double sech_diff = 1.0 / std::cosh(diff);
        return sech_diff * sech_diff;
    }

    constexpr bool issymmetric() override {
        return true;
    }

    constexpr bool isdifferentiable() override {
        return true;
    }

    constexpr bool isdifferentiable(double) override {
        return true;
    }

    constexpr bool istwicedifferentiable() override {
        return true;
    }

    constexpr bool istwicedifferentiable(double) override {
        return true;
    }

    constexpr bool islipschitzcont() override {
        return true;
    }

    constexpr bool isconvex() override {
        return true;
    }

    constexpr bool isstrictlyconvex() override {
        return true;
    }

    constexpr bool isstronglyconvex() override {
        return true;
    }
};