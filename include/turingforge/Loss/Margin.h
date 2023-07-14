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
 * surrogatesuch as [L1HingeLoss].
 *
 * ```math
 * L(a) = \begin{cases} 1 & \quad \text{if } a < 0 \\ 0 & \quad \text{if } a >= 0\\ \end{cases}
 * ```
 */
struct ZeroOneLoss : public MarginLoss {
    using MarginLoss::operator();
    constexpr double operator()(double agreement) const override {
        return (std::signbit(agreement) ? 1 : 0);
    }

    constexpr double deriv(double target, double output) override {
        return double(0);
    }

    constexpr double deriv2(double target, double output) override {
        return double(0);
    }

    constexpr double deriv(double agreement) override {
        return double(0);
    }

    constexpr double deriv2(double agreement) override {
        return double(0);
    }

    constexpr bool isminimizable() override {
        return true;
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

    constexpr bool isnemitski() override {
        return true;
    }

    constexpr bool islipschitzcont() override {
        return true;
    }

    constexpr bool isconvex() override {
        return false;
    }

    constexpr bool isclasscalibrated() override {
        return true;
    }

    constexpr bool isclipable() override {
        return true;
    }
};

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
    using MarginLoss::operator();
    constexpr double operator()(double agreement) const override {
        return std::max(double(0), -agreement);
    }

    constexpr double deriv(double agreement) override {
        return (agreement >= double(0)) ? double(0) : -double(1);
    }

    constexpr double deriv2(double agreement) override {
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

    constexpr bool isclipable() override {
        return true;
    }
};

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
    using MarginLoss::operator();
    constexpr double operator()(double agreement) const override {
        return std::log1p(std::exp(-agreement));
    }

    constexpr double deriv(double agreement) override {
        return -double(1) / (double(1) + std::exp(agreement));
    }

    constexpr double deriv2(double agreement) override {
        auto exp_t = std::exp(agreement);
        return exp_t / std::abs(std::pow(double(1) + exp_t, double(2)));
    }

    constexpr bool isunivfishercons() override {
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

    constexpr bool isclipable() override {
        return false;
    }
};

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
    using MarginLoss::operator();
    constexpr double operator()(double agreement) const override {
        return std::max(double(0), double(1) - agreement);
    }

    constexpr double deriv(double agreement) override {
        return agreement >= double(1) ? double(0) : -double(1);
    }

    constexpr double deriv2(double agreement) override {
        return double(0);
    }

    constexpr bool isfishercons() override {
        return true;
    }

    constexpr bool isdifferentiable() override {
        return false;
    }

    constexpr bool isdifferentiable(double at) override {
        return at != double(1);
    }

    constexpr bool istwicedifferentiable() override {
        return false;
    }

    constexpr bool istwicedifferentiable(double at) override {
        return at != double(1);
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

    constexpr bool isclipable() override {
        return true;
    }
};

struct HingeLoss : public L1HingeLoss {};

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
    using MarginLoss::operator();
    constexpr double operator()(double agreement) const override {
        return agreement >= double(1) ? double(0) : std::pow(double(1) - agreement, 2);
    }

    constexpr double deriv(double agreement) override {
        return agreement >= double(1) ? double(0) : double(2) * (agreement - double(1));
    }

    constexpr double deriv2(double agreement) override {
        return agreement >= double(1) ? double(0) : double(2);
    }

    constexpr bool isunivfishercons() override {
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

    constexpr bool istwicedifferentiable(double at) override {
        return at != double(1);
    }

    constexpr bool islocallylipschitzcont() override {
        return true;
    }

    constexpr bool islipschitzcont() override {
        return false;
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

    constexpr bool isclipable() override {
        return true;
    }
};

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
    using MarginLoss::operator();
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

    [[nodiscard]] constexpr double deriv(double agreement) override {
        if (agreement >= 1 - gamma) {
            return agreement >= 1 ? double(0) : (agreement - double(1)) / gamma;
        } else {
            return -double(1);
        }
    }

    [[nodiscard]] constexpr double deriv2(double agreement) override {
        if (agreement < 1 - gamma || agreement > 1) {
            return double(0);
        } else {
            return double(1) / gamma;
        }
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

    [[nodiscard]] constexpr bool istwicedifferentiable(double at) const {
        return at != 1 && at != 1 - gamma;
    }

    constexpr bool islocallylipschitzcont() override {
        return true;
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

    constexpr bool isclipable() override {
        return true;
    }
};

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
    using MarginLoss::operator();
    constexpr double operator()(double agreement) const override {
        if (agreement >= -1) {
            return std::pow(std::max(double(0), double(1) - agreement), 2);
        } else {
            return -double(4) * agreement;
        }
    }

    constexpr double deriv(double agreement) override {
        if (agreement >= -1) {
            return agreement > 1 ? double(0) : double(2) * agreement - double(2);
        } else {
            return -double(4);
        }
    }

    constexpr double deriv2(double agreement) override {
        if (agreement < -1 || agreement > 1) {
            return double(0);
        } else {
            return double(2);
        }
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

    constexpr bool istwicedifferentiable(double at) override {
        return at != 1 && at != -1;
    }

    constexpr bool islocallylipschitzcont() override {
        return true;
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

    constexpr bool isclipable() override {
        return true;
    }
};

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
    using MarginLoss::operator();
    constexpr double operator()(double agreement) const override {
        return std::pow(double(1) - agreement, 2);
    }

    constexpr double deriv(double agreement) override {
        return double(2) * (agreement - double(1));
    }

    constexpr double deriv2(double agreement) override {
        return double(2);
    }

    constexpr bool isunivfishercons() override {
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

    constexpr bool islocallylipschitzcont() override {
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

    constexpr bool isclipable() override {
        return true;
    }
};

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
    using MarginLoss::operator();
    constexpr double operator()(double agreement) const override {
        return std::exp(-agreement);
    }

    constexpr double deriv(double agreement) override {
        return -std::exp(-agreement);
    }

    constexpr double deriv2(double agreement) override {
        return std::exp(-agreement);
    }

    constexpr bool isunivfishercons() override {
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

    constexpr bool islocallylipschitzcont() override {
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
        return false;
    }

    constexpr bool isclipable() override {
        return false;
    }
};

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
    using MarginLoss::operator();
    constexpr double operator()(double agreement) const override {
        return double(1) - std::tanh(agreement);
    }

    constexpr double deriv(double agreement) override {
        return -abs2(sech(agreement));
    }

    constexpr double deriv2(double agreement) override {
        return double(2) * std::tanh(agreement) * abs2(sech(agreement));
    }

    constexpr bool isunivfishercons() override {
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

    constexpr bool islocallylipschitzcont() override {
        return true;
    }

    constexpr bool islipschitzcont() override {
        return true;
    }

    constexpr bool isclasscalibrated() override {
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

    constexpr bool isclipable() override {
        return false;
    }
};

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
    using MarginLoss::operator();
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

    double deriv(double agreement) override {
        if (agreement <= q / (q + 1)) {
            return -double(1);
        } else {
            return -std::pow(q / (q + 1), q + 1) / std::pow(agreement, q + 1);
        }
    }

    double deriv2(double agreement) override {
        if (agreement <= q / (q + 1)) {
            return double(0);
        } else {
            return std::pow(q, q + 1) / std::pow(q + 1, q) / std::pow(agreement, q + 2);
        }
    }

    bool isdifferentiable() override {
        return true;
    }

    bool isdifferentiable(double) override {
        return true;
    }

    bool istwicedifferentiable() override {
        return true;
    }

    bool istwicedifferentiable(double) override {
        return true;
    }

    bool islocallylipschitzcont() override {
        return true;
    }

    bool islipschitzcont() override {
        return true;
    }

    bool isconvex() override {
        return true;
    }

    bool isstrictlyconvex() override {
        return false;
    }

    bool isstronglyconvex() override {
        return false;
    }

    bool isfishercons() override {
        return true;
    }

    bool isunivfishercons() override {
        return true;
    }

    bool isclipable() override {
        return false;
    }
};