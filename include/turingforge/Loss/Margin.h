#pragma once

#include <stdexcept>
#include "Traits.h"
#include "Utils.h"

/*
 * ZeroOneLoss <: MarginLoss
 *
 * The classical classification loss. It penalizes every misclassified
 * observation with a loss of `1` while every correctly classified
 * observation has a loss of `0`.
 * It is not convex nor continuous and thus seldom used directly.
 * Instead, one usually works with some classification-calibrated
 * surrogate loss, such as [L1HingeLoss].
 *
 * ```math
 * L(a) = \begin{cases} 1 & \quad \text{if } a < 0 \\ 0 & \quad \text{if } a >= 0\\ \end{cases}
 * ```
 */
struct ZeroOneLoss : public MarginLoss {
    constexpr double operator()(double agreement) const override {
        return (signum(agreement) ? 1 : 0);
    }
};

constexpr auto deriv(const ZeroOneLoss& loss, double target, double output) {
    return double(0);
}

constexpr auto deriv2(const ZeroOneLoss& loss, double target, double output) {
    return double(0);
}

constexpr auto deriv(const ZeroOneLoss& loss, double agreement) {
    return double(0);
}

constexpr auto deriv2(const ZeroOneLoss& loss, double agreement) {
    return double(0);
}

constexpr bool isminimizable(const ZeroOneLoss&) {
    return true;
}

constexpr bool isdifferentiable(const ZeroOneLoss&) {
    return false;
}

constexpr bool isdifferentiable(const ZeroOneLoss&, double at) {
    return at != 0;
}

constexpr bool istwicedifferentiable(const ZeroOneLoss&) {
    return false;
}

constexpr bool istwicedifferentiable(const ZeroOneLoss&, double at) {
    return at != 0;
}

constexpr bool isnemitski(const ZeroOneLoss&) {
    return true;
}

constexpr bool islipschitzcont(const ZeroOneLoss&) {
    return true;
}

constexpr bool isconvex(const ZeroOneLoss&) {
    return false;
}

constexpr bool isclasscalibrated(const ZeroOneLoss&) {
    return true;
}

constexpr bool isclipable(const ZeroOneLoss&) {
    return true;
}

/*
 * PerceptronLoss <: MarginLoss
 *
 * The perceptron loss linearly penalizes every prediction where the
 * resulting `agreement <= 0`.
 * It is Lipschitz continuous and convex, but not strictly convex.
 *
 * ```math
 * L(a) = \max \{ 0, -a \}
 * ```
 */

struct PerceptronLoss : public MarginLoss {
    constexpr double operator()(double agreement) const override {
        return std::max(double(0), -agreement);
    }
};

constexpr auto deriv(const PerceptronLoss& loss, double agreement) {
    return (agreement >= double(0)) ? double(0) : -double(1);
}

constexpr auto deriv2(const PerceptronLoss& loss, double agreement) {
    return double(0);
}

constexpr bool isdifferentiable(const PerceptronLoss&) {
    return false;
}

constexpr bool isdifferentiable(const PerceptronLoss&, double at) {
    return at != 0;
}

constexpr bool istwicedifferentiable(const PerceptronLoss&) {
    return false;
}

constexpr bool istwicedifferentiable(const PerceptronLoss&, double at) {
    return at != 0;
}

constexpr bool islipschitzcont(const PerceptronLoss&) {
    return true;
}

constexpr bool isconvex(const PerceptronLoss&) {
    return true;
}

constexpr bool isstrictlyconvex(const PerceptronLoss&) {
    return false;
}

constexpr bool isstronglyconvex(const PerceptronLoss&) {
    return false;
}

constexpr bool isclipable(const PerceptronLoss&) {
    return true;
}

/*
 * LogitMarginLoss <: MarginLoss
 *
 * The margin version of the logistic loss. It is infinitely many
 * times differentiable, strictly convex, and Lipschitz continuous.
 *
 * ```math
 * L(a) = \ln (1 + e^{-a})
 * ```
 */

struct LogitMarginLoss : public MarginLoss {
    constexpr double operator()(double agreement) const override {
        return std::log1p(std::exp(-agreement));
    }
};

constexpr auto deriv(const LogitMarginLoss& loss, double agreement) {
    return -double(1) / (double(1) + std::exp(agreement));
}

constexpr auto deriv2(const LogitMarginLoss& loss, double agreement) {
    auto exp_t = std::exp(agreement);
    return exp_t / std::abs(std::pow(double(1) + exp_t, double(2)));
}

constexpr bool isunivfishercons(const LogitMarginLoss&) {
    return true;
}

constexpr bool isdifferentiable(const LogitMarginLoss&) {
    return true;
}

constexpr bool isdifferentiable(const LogitMarginLoss&, double) {
    return true;
}

constexpr bool istwicedifferentiable(const LogitMarginLoss&) {
    return true;
}

constexpr bool istwicedifferentiable(const LogitMarginLoss&, double) {
    return true;
}

constexpr bool islipschitzcont(const LogitMarginLoss&) {
    return true;
}

constexpr bool isconvex(const LogitMarginLoss&) {
    return true;
}

constexpr bool isstrictlyconvex(const LogitMarginLoss&) {
    return true;
}

constexpr bool isstronglyconvex(const LogitMarginLoss&) {
    return false;
}

constexpr bool isclipable(const LogitMarginLoss&) {
    return false;
}

/*
 *  L1HingeLoss <: MarginLoss
 *
 * The hinge loss linearly penalizes every predicition where the
 * resulting `agreement < 1` .
 * It is Lipschitz continuous and convex, but not strictly convex.
 *
 * ```math
 * L(a) = \max \{ 0, 1 - a \}
 * ```
 */

struct L1HingeLoss : public MarginLoss {
    constexpr double operator()(double agreement) const override {
        return std::max(double(0), double(1) - agreement);
    }
};

constexpr auto deriv(const L1HingeLoss& loss, double agreement) {
    return agreement >= double(1) ? double(0) : -double(1);
}

constexpr auto deriv2(const L1HingeLoss& loss, double agreement) {
    return double(0);
}

constexpr bool isfishercons(const L1HingeLoss&) {
    return true;
}

constexpr bool isdifferentiable(const L1HingeLoss&) {
    return false;
}

constexpr bool isdifferentiable(const L1HingeLoss&, double at) {
    return at != double(1);
}

constexpr bool istwicedifferentiable(const L1HingeLoss&) {
    return false;
}

constexpr bool istwicedifferentiable(const L1HingeLoss&, double at) {
    return at != double(1);
}

constexpr bool islipschitzcont(const L1HingeLoss&) {
    return true;
}

constexpr bool isconvex(const L1HingeLoss&) {
    return true;
}

constexpr bool isstrictlyconvex(const L1HingeLoss&) {
    return false;
}

constexpr bool isstronglyconvex(const L1HingeLoss&) {
    return false;
}

constexpr bool isclipable(const L1HingeLoss&) {
    return true;
}

/*
 * L2HingeLoss <: MarginLoss
 *
 * The truncated least squares loss quadratically penalizes every
 * predicition where the resulting `agreement < 1`.
 * It is locally Lipschitz continuous and convex,
 * but not strictly convex.
 *
 * ```math
 * L(a) = \max \{ 0, 1 - a \}^2
 * ```
 */

struct L2HingeLoss : public MarginLoss {
    constexpr double operator()(double agreement) const override {
        return agreement >= double(1) ? double(0) : std::pow(double(1) - agreement, 2);
    }
};

constexpr auto deriv(const L2HingeLoss& loss, double agreement) {
    return agreement >= double(1) ? double(0) : double(2) * (agreement - double(1));
}

constexpr auto deriv2(const L2HingeLoss& loss, double agreement) {
    return agreement >= double(1) ? double(0) : double(2);
}

constexpr bool isunivfishercons(const L2HingeLoss&) {
    return true;
}

constexpr bool isdifferentiable(const L2HingeLoss&) {
    return true;
}

constexpr bool isdifferentiable(const L2HingeLoss&, double) {
    return true;
}

constexpr bool istwicedifferentiable(const L2HingeLoss&) {
    return false;
}

constexpr bool istwicedifferentiable(const L2HingeLoss&, double at) {
    return at != double(1);
}

constexpr bool islocallylipschitzcont(const L2HingeLoss&) {
    return true;
}

constexpr bool islipschitzcont(const L2HingeLoss&) {
    return false;
}

constexpr bool isconvex(const L2HingeLoss&) {
    return true;
}

constexpr bool isstrictlyconvex(const L2HingeLoss&) {
    return false;
}

constexpr bool isstronglyconvex(const L2HingeLoss&) {
    return false;
}

constexpr bool isclipable(const L2HingeLoss&) {
    return true;
}

/*
 * SmoothedL1HingeLoss <: MarginLoss
 *
 * As the name suggests a smoothed version of the L1 hinge loss.
 * It is Lipschitz continuous and convex, but not strictly convex.
 *
 * ```math
 * L(a) = \begin{cases} \frac{0.5}{\gamma} \cdot \max \{ 0, 1 - a \} ^2 & \quad \text{if } a \ge 1 - \gamma \\ 1 - \frac{\gamma}{2} - a & \quad \text{otherwise}\\ \end{cases}
 * ```
 */

struct SmoothedL1HingeLoss : public MarginLoss {
    double gamma;

    explicit SmoothedL1HingeLoss(double gamma) : gamma(gamma) {
        if (gamma <= 0) {
            throw std::invalid_argument("gamma must be strictly positive");
        }
    }

    constexpr double operator()(double agreement) const override {
        if (agreement >= 1 - gamma) {
            return double(0.5) / gamma * std::pow(std::max(double(0), double(1) - agreement), 2);
        } else {
            return double(1) - gamma / double(2) - agreement;
        }
    }
};

constexpr auto deriv(const SmoothedL1HingeLoss& loss, double agreement) {
    if (agreement >= 1 - loss.gamma) {
        return agreement >= 1 ? double(0) : (agreement - double(1)) / loss.gamma;
    } else {
        return -double(1);
    }
}

constexpr auto deriv2(const SmoothedL1HingeLoss& loss, double agreement) {
    if (agreement < 1 - loss.gamma || agreement > 1) {
        return double(0);
    } else {
        return double(1) / loss.gamma;
    }
}

constexpr bool isdifferentiable(const SmoothedL1HingeLoss&) {
    return true;
}

constexpr bool isdifferentiable(const SmoothedL1HingeLoss&, double) {
    return true;
}

constexpr bool istwicedifferentiable(const SmoothedL1HingeLoss&) {
    return false;
}

constexpr bool istwicedifferentiable(const SmoothedL1HingeLoss& loss, double at) {
    return at != 1 && at != 1 - loss.gamma;
}

constexpr bool islocallylipschitzcont(const SmoothedL1HingeLoss&) {
    return true;
}

constexpr bool islipschitzcont(const SmoothedL1HingeLoss&) {
    return true;
}

constexpr bool isconvex(const SmoothedL1HingeLoss&) {
    return true;
}

constexpr bool isstrictlyconvex(const SmoothedL1HingeLoss&) {
    return false;
}

constexpr bool isstronglyconvex(const SmoothedL1HingeLoss&) {
    return false;
}

constexpr bool isclipable(const SmoothedL1HingeLoss&) {
    return true;
}

/*
 * ModifiedHuberLoss <: MarginLoss
 *
 * A special (4 times scaled) case of the [`SmoothedL1HingeLoss`]
 * with `γ=2`. It is Lipschitz continuous and convex,
 * but not strictly convex.
 *
 * ```math
 * L(a) = \begin{cases} \max \{ 0, 1 - a \} ^2 & \quad \text{if } a \ge -1 \\ - 4 a & \quad \text{otherwise}\\ \end{cases}
 * ```
 */

struct ModifiedHuberLoss : public MarginLoss {
    constexpr double operator()(double agreement) const override {
        if (agreement >= -1) {
            return std::pow(std::max(double(0), double(1) - agreement), 2);
        } else {
            return -double(4) * agreement;
        }
    }
};

constexpr auto deriv(const ModifiedHuberLoss& loss, double agreement) {
    if (agreement >= -1) {
        return agreement > 1 ? double(0) : double(2) * agreement - double(2);
    } else {
        return -double(4);
    }
}

constexpr auto deriv2(const ModifiedHuberLoss& loss, double agreement) {
    if (agreement < -1 || agreement > 1) {
        return double(0);
    } else {
        return double(2);
    }
}

constexpr bool isdifferentiable(const ModifiedHuberLoss&) {
    return true;
}

constexpr bool isdifferentiable(const ModifiedHuberLoss&, double) {
    return true;
}

constexpr bool istwicedifferentiable(const ModifiedHuberLoss&) {
    return false;
}

constexpr bool istwicedifferentiable(const ModifiedHuberLoss&, double at) {
    return at != 1 && at != -1;
}

constexpr bool islocallylipschitzcont(const ModifiedHuberLoss&) {
    return true;
}

constexpr bool islipschitzcont(const ModifiedHuberLoss&) {
    return true;
}

constexpr bool isconvex(const ModifiedHuberLoss&) {
    return true;
}

constexpr bool isstrictlyconvex(const ModifiedHuberLoss&) {
    return false;
}

constexpr bool isstronglyconvex(const ModifiedHuberLoss&) {
    return false;
}

constexpr bool isclipable(const ModifiedHuberLoss&) {
    return true;
}

/*
 * L2MarginLoss <: MarginLoss
 *
 * The margin-based least-squares loss for classification,
 * which penalizes every prediction where `agreement != 1` quadratically.
 * It is locally Lipschitz continuous and strongly convex.
 *
 * ```math
 * L(a) = {\left( 1 - a \right)}^2
 * ```
 */

struct L2MarginLoss : public MarginLoss {
    constexpr double operator()(double agreement) const override {
        return std::pow(double(1) - agreement, 2);
    }
};


template <typename T>
constexpr auto deriv(const L2MarginLoss& loss, double agreement) {
    return double(2) * (agreement - double(1));
}

constexpr auto deriv2(const L2MarginLoss& loss, double agreement) {
    return double(2);
}

constexpr bool isunivfishercons(const L2MarginLoss&) {
    return true;
}

constexpr bool isdifferentiable(const L2MarginLoss&) {
    return true;
}

constexpr bool isdifferentiable(const L2MarginLoss&, double) {
    return true;
}

constexpr bool istwicedifferentiable(const L2MarginLoss&) {
    return true;
}

constexpr bool istwicedifferentiable(const L2MarginLoss&, double) {
    return true;
}

constexpr bool islocallylipschitzcont(const L2MarginLoss&) {
    return true;
}

constexpr bool islipschitzcont(const L2MarginLoss&) {
    return false;
}

constexpr bool isconvex(const L2MarginLoss&) {
    return true;
}

constexpr bool isstrictlyconvex(const L2MarginLoss&) {
    return true;
}

constexpr bool isstronglyconvex(const L2MarginLoss&) {
    return true;
}

constexpr bool isclipable(const L2MarginLoss&) {
    return true;
}

/*
 * ExpLoss <: MarginLoss
 *
 * The margin-based exponential loss for classification, which
 * penalizes every prediction exponentially. It is infinitely many
 * times differentiable, locally Lipschitz continuous and strictly
 * convex, but not clipable.
 *
 * ```math
 * L(a) = e^{-a}
 * ```
 */

struct ExpLoss : public MarginLoss {
    constexpr double operator()(double agreement) const override {
        return std::exp(-agreement);
    }
};

constexpr auto deriv(const ExpLoss& loss, double agreement) {
    return -std::exp(-agreement);
}

constexpr auto deriv2(const ExpLoss& loss, double agreement) {
    return std::exp(-agreement);
}

constexpr bool isunivfishercons(const ExpLoss&) {
    return true;
}

constexpr bool isdifferentiable(const ExpLoss&) {
    return true;
}

constexpr bool isdifferentiable(const ExpLoss&, double) {
    return true;
}

constexpr bool istwicedifferentiable(const ExpLoss&) {
    return true;
}

constexpr bool istwicedifferentiable(const ExpLoss&, double) {
    return true;
}

constexpr bool islocallylipschitzcont(const ExpLoss&) {
    return true;
}

constexpr bool islipschitzcont(const ExpLoss&) {
    return false;
}

constexpr bool isconvex(const ExpLoss&) {
    return true;
}

constexpr bool isstrictlyconvex(const ExpLoss&) {
    return true;
}

constexpr bool isstronglyconvex(const ExpLoss&) {
    return false;
}

constexpr bool isclipable(const ExpLoss&) {
    return false;
}

/*
 * SigmoidLoss <: MarginLoss
 *
 * Continuous loss which penalizes every prediction with a loss
 * within in the range (0,2). It is infinitely many times
 * differentiable, Lipschitz continuous but non-convex.
 *
 * ```math
 * L(a) = 1 - \tanh(a)
 * ```
 */

struct SigmoidLoss : public MarginLoss {
    constexpr double operator()(double agreement) const override {
        return double(1) - std::tanh(agreement);
    }
};

constexpr auto deriv(const SigmoidLoss& loss, double agreement) {
    return -abs2(sech(agreement));
}

constexpr auto deriv2(const SigmoidLoss& loss, double agreement) {
    return double(2) * std::tanh(agreement) * abs2(sech(agreement));
}

constexpr bool isunivfishercons(const SigmoidLoss&) {
    return true;
}

constexpr bool isdifferentiable(const SigmoidLoss&) {
    return true;
}

constexpr bool isdifferentiable(const SigmoidLoss&, double) {
    return true;
}

constexpr bool istwicedifferentiable(const SigmoidLoss&) {
    return true;
}

constexpr bool istwicedifferentiable(const SigmoidLoss&, double) {
    return true;
}

constexpr bool islocallylipschitzcont(const SigmoidLoss&) {
    return true;
}

constexpr bool islipschitzcont(const SigmoidLoss&) {
    return true;
}

constexpr bool isclasscalibrated(const SigmoidLoss&) {
    return true;
}

constexpr bool isconvex(const SigmoidLoss&) {
    return false;
}

constexpr bool isstrictlyconvex(const SigmoidLoss&) {
    return false;
}

constexpr bool isstronglyconvex(const SigmoidLoss&) {
    return false;
}

constexpr bool isclipable(const SigmoidLoss&) {
    return false;
}

/*
 * DWDMarginLoss <: MarginLoss
 *
 * The distance weighted discrimination margin loss. It is a
 * differentiable generalization of the [L1HingeLoss] that is
 * different from the [SmoothedL1HingeLoss]. It is Lipschitz
 * continuous and convex, but not strictly convex.
 *
 * ```math
 * L(a) = \begin{cases} 1 - a & \quad \text{if } a \ge \frac{q}{q+1} \\ \frac{1}{a^q} \frac{q^q}{(q+1)^{q+1}} & \quad \text{otherwise}\\ \end{cases}
 * ```
 */

struct DWDMarginLoss : MarginLoss {
    double q;

    explicit DWDMarginLoss(double q) : q(q) {
        if (q <= 0) {
            throw std::invalid_argument("q must be strictly positive");
        }
    }

    constexpr double operator()(double agreement) const override {
        if (agreement <= q / (q + 1)) {
            return double(1) - agreement;
        } else {
            return std::pow(q, q) / std::pow(q + 1, q + 1) / std::pow(agreement, q);
        }
    }
};

double deriv(const DWDMarginLoss& loss, double agreement) {
    double q = loss.q;
    if (agreement <= q / (q + 1)) {
        return -double(1);
    } else {
        return -std::pow(q / (q + 1), q + 1) / std::pow(agreement, q + 1);
    }
}

double deriv2(const DWDMarginLoss& loss, double agreement) {
    double q = loss.q;
    if (agreement <= q / (q + 1)) {
        return double(0);
    } else {
        return std::pow(q, q + 1) / std::pow(q + 1, q) / std::pow(agreement, q + 2);
    }
}

bool isdifferentiable(const DWDMarginLoss&) {
    return true;
}

bool isdifferentiable(DWDMarginLoss&, double) {
    return true;
}

bool istwicedifferentiable(DWDMarginLoss&) {
    return true;
}

bool istwicedifferentiable(DWDMarginLoss&, double) {
    return true;
}

bool islocallylipschitzcont(DWDMarginLoss&) {
    return true;
}

bool islipschitzcont(DWDMarginLoss&) {
    return true;
}

bool isconvex(DWDMarginLoss&) {
    return true;
}

bool isstrictlyconvex(DWDMarginLoss&) {
    return false;
}

bool isstronglyconvex(DWDMarginLoss&) {
    return false;
}

bool isfishercons(DWDMarginLoss&) {
    return true;
}

bool isunivfishercons(DWDMarginLoss&) {
    return true;
}

bool isclipable(DWDMarginLoss&) {
    return false;
}