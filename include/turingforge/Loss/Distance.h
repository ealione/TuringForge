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
    double p{};

    constexpr double operator()(double difference) const override {
        return std::pow(std::abs(difference), p);
    }
};

auto deriv(const LPDistLoss& loss, double difference) {
    if (difference == 0) {
        return double(0);
    } else {
        return loss.p * difference * std::pow(std::abs(difference), loss.p - 2);
    }
}

auto deriv2(const LPDistLoss& loss, double difference) {
    if (difference == 0) {
        return double(0);
    } else {
        return (std::pow(loss.p, 2) - loss.p) * std::pow(std::abs(difference), loss.p) / std::pow(difference, 2);
    }
}

bool isminimizable(const LPDistLoss&) {
    return true;
}

bool issymmetric(const LPDistLoss&) {
    return true;
}

bool isdifferentiable(const LPDistLoss& loss) {
    return loss.p > 1;
}

bool isdifferentiable(const LPDistLoss& loss, double) {
    return loss.p > 1;
}

bool istwicedifferentiable(const LPDistLoss& loss) {
    return loss.p > 1;
}

bool istwicedifferentiable(const LPDistLoss& loss, double) {
    return loss.p > 1;
}

bool islipschitzcont(const LPDistLoss& loss) {
    return loss.p == 1;
}

bool islocallylipschitzcont(const LPDistLoss& loss) {
    return loss.p >= 1;
}

bool isconvex(const LPDistLoss& loss) {
    return loss.p >= 1;
}

bool isstrictlyconvex(const LPDistLoss& loss) {
    return loss.p > 1;
}

bool isstronglyconvex(const LPDistLoss& loss) {
    return loss.p >= 2;
}

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

struct L1DistLoss : DistanceLoss {
    constexpr double operator()(double difference) const override {
        return std::abs(difference);
    }
};

auto deriv(const L1DistLoss&, double difference) {
    return std::signbit(difference) ? -1 : 1;
}

auto deriv2(const L1DistLoss&, double) {
    return double(0);
}

bool isdifferentiable(const L1DistLoss&) {
    return false;
}

bool isdifferentiable(const L1DistLoss&, double at) {
return at != 0;
}

bool istwicedifferentiable(const L1DistLoss&) {
    return false;
}

bool istwicedifferentiable(const L1DistLoss&, double at) {
return at != 0;
}

bool islipschitzcont(const L1DistLoss&) {
    return true;
}

bool isconvex(const L1DistLoss&) {
    return true;
}

bool isstrictlyconvex(const L1DistLoss&) {
    return false;
}

bool isstronglyconvex(const L1DistLoss&) {
    return false;
}

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

struct L2DistLoss : DistanceLoss {
    constexpr double operator()(double difference) const override {
        return std::abs(difference) * std::abs(difference);
    }
};

auto deriv(const L2DistLoss&, double difference) {
    return 2.0 * difference;
}

auto deriv2(const L2DistLoss&, double) {
    return 2.0;
}

bool isdifferentiable(const L2DistLoss&) {
    return true;
}

bool isdifferentiable(const L2DistLoss&, double) {
    return true;
}

bool istwicedifferentiable(const L2DistLoss&) {
    return true;
}

bool istwicedifferentiable(const L2DistLoss&, double) {
    return true;
}

bool islipschitzcont(const L2DistLoss&) {
    return false;
}

bool isconvex(const L2DistLoss&) {
    return true;
}

bool isstrictlyconvex(const L2DistLoss&) {
    return true;
}

bool isstronglyconvex(const L2DistLoss&) {
    return true;
}

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
};

auto deriv(const PeriodicLoss& loss, double difference) {
    return loss.k * std::sin(difference * loss.k);
}

auto deriv2(const PeriodicLoss& loss, double difference) {
    return std::pow(loss.k, 2) * std::cos(difference * loss.k);
}

bool isdifferentiable(const PeriodicLoss&) {
    return true;
}

bool isdifferentiable(const PeriodicLoss&, double) {
    return true;
}

bool istwicedifferentiable(const PeriodicLoss&) {
    return true;
}

bool istwicedifferentiable(const PeriodicLoss&, double) {
    return true;
}

bool islipschitzcont(const PeriodicLoss&) {
    return true;
}

bool isconvex(const PeriodicLoss&) {
    return false;
}

bool isstrictlyconvex(const PeriodicLoss&) {
    return false;
}

bool isstronglyconvex(const PeriodicLoss&) {
    return false;
}

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
};

auto deriv(const HuberLoss& loss, double difference) {
    if (std::abs(difference) <= loss.d) {
        return difference;  // quadratic
    } else {
        return loss.d * static_cast<double>(std::signbit(difference) ? -1 : 1);  // linear
    }
}

auto deriv2(const HuberLoss& loss, double difference) {
    return std::abs(difference) <= loss.d ? double(1) : double(0);
}

bool isdifferentiable(const HuberLoss&) {
    return true;
}

bool isdifferentiable(const HuberLoss&, double) {
    return true;
}

bool istwicedifferentiable(const HuberLoss&) {
    return false;
}

bool istwicedifferentiable(const HuberLoss& loss, double at) {
    return at != std::abs(loss.d);
}

bool islipschitzcont(const HuberLoss&) {
    return true;
}

bool isconvex(const HuberLoss&) {
    return true;
}

bool isstrictlyconvex(const HuberLoss&) {
    return false;
}

bool isstronglyconvex(const HuberLoss&) {
    return false;
}

bool issymmetric(const HuberLoss&) {
    return true;
}

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
};

auto deriv(const L1EpsilonInsLoss& loss, double difference) {
    return std::abs(difference) <= loss.eps ? double(0) : static_cast<double>(std::signbit(difference) ? -1 : 1);
}

auto deriv2(const L1EpsilonInsLoss& loss, double) {
    return double(0);
}

bool issymmetric(const L1EpsilonInsLoss&) {
    return true;
}

bool isdifferentiable(const L1EpsilonInsLoss&) {
    return false;
}

bool isdifferentiable(const L1EpsilonInsLoss& loss, double at) {
    return std::abs(at) != loss.eps;
}

bool istwicedifferentiable(const L1EpsilonInsLoss&) {
    return false;
}

bool istwicedifferentiable(const L1EpsilonInsLoss& loss, double at) {
    return std::abs(at) != loss.eps;
}

bool islipschitzcont(const L1EpsilonInsLoss&) {
    return true;
}

bool isconvex(const L1EpsilonInsLoss&) {
    return true;
}

bool isstrictlyconvex(const L1EpsilonInsLoss&) {
    return false;
}

bool isstronglyconvex(const L1EpsilonInsLoss&) {
    return false;
}

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
};

auto deriv(const L2EpsilonInsLoss& loss, double difference) {
    auto abs_diff = std::abs(difference);
    return abs_diff <= loss.eps ? double(0) : double(2) * std::copysign(abs_diff - loss.eps, difference);
}

auto deriv2(const L2EpsilonInsLoss& loss, double difference) {
    return std::abs(difference) <= loss.eps ? double(0) : double(2);
}

bool issymmetric(const L2EpsilonInsLoss&) {
    return true;
}

bool isdifferentiable(const L2EpsilonInsLoss&) {
    return true;
}

bool isdifferentiable(const L2EpsilonInsLoss&, double) {
    return true;
}

bool istwicedifferentiable(const L2EpsilonInsLoss&) {
    return false;
}

bool istwicedifferentiable(const L2EpsilonInsLoss& loss, double at) {
    return std::abs(at) != loss.eps;
}

bool islipschitzcont(const L2EpsilonInsLoss&) {
    return false;
}

bool isconvex(const L2EpsilonInsLoss&) {
    return true;
}

bool isstrictlyconvex(const L2EpsilonInsLoss&) {
    return true;
}

bool isstronglyconvex(const L2EpsilonInsLoss&) {
    return true;
}

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
};

auto deriv(const LogitDistLoss&, double difference) {
    return std::tanh(difference / double(2));
}

auto deriv2(const LogitDistLoss&, double difference) {
    auto er = std::exp(difference);
    return double(2) * er / std::abs(std::pow(double(1) + er, 2));
}

bool issymmetric(const LogitDistLoss&) {
    return true;
}

bool isdifferentiable(const LogitDistLoss&) {
    return true;
}

bool isdifferentiable(const LogitDistLoss&, double) {
    return true;
}

bool istwicedifferentiable(const LogitDistLoss&) {
    return true;
}

bool istwicedifferentiable(const LogitDistLoss&, double) {
    return true;
}

bool islipschitzcont(const LogitDistLoss&) {
    return true;
}

bool isconvex(const LogitDistLoss&) {
    return true;
}

bool isstrictlyconvex(const LogitDistLoss&) {
    return true;
}

bool isstronglyconvex(const LogitDistLoss&) {
    return false;
}

/*
 * QuantileLoss : DistanceLoss
 *
 * The distance-based quantile loss, also known as pinball loss,
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

    constexpr double operator()(const QuantileLoss& loss, double diff) {
        return diff * (diff > 0 ? 1 - loss.tau : -loss.tau);
    }
};

double deriv(const QuantileLoss& loss, double diff) {
    return (diff > 0 ? 1 - loss.tau : -loss.tau);
}

double deriv2(const QuantileLoss&, double) {
    return 0.0;
}

bool issymmetric(const QuantileLoss& loss) {
    return loss.tau == 0.5;
}

bool isdifferentiable(const QuantileLoss&) {
    return false;
}

bool isdifferentiable(const QuantileLoss&, double at) {
    return at != 0.0;
}

bool istwicedifferentiable(const QuantileLoss&) {
    return false;
}

bool istwicedifferentiable(const QuantileLoss&, double at) {
    return at != 0.0;
}

bool islipschitzcont(const QuantileLoss&) {
    return true;
}

bool islipschitzcont_deriv(const QuantileLoss&) {
    return true;
}

bool isconvex(const QuantileLoss&) {
    return true;
}

bool isstrictlyconvex(const QuantileLoss&) {
    return false;
}

bool isstronglyconvex(const QuantileLoss&) {
    return false;
}

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
};

double deriv(const LogCoshLoss&, double diff) {
    return std::tanh(diff);
}

double deriv2(const LogCoshLoss&, double diff) {
    double sech_diff = 1.0 / std::cosh(diff);
    return sech_diff * sech_diff;
}

bool issymmetric(const LogCoshLoss&) {
    return true;
}

bool isdifferentiable(const LogCoshLoss&) {
    return true;
}

bool isdifferentiable(const LogCoshLoss&, double) {
    return true;
}

bool istwicedifferentiable(const LogCoshLoss&) {
    return true;
}

bool istwicedifferentiable(const LogCoshLoss&, double) {
    return true;
}

bool islipschitzcont(const LogCoshLoss&) {
    return true;
}

bool isconvex(const LogCoshLoss&) {
    return true;
}

bool isstrictlyconvex(const LogCoshLoss&) {
    return true;
}

bool isstronglyconvex(const LogCoshLoss&) {
    return true;
}