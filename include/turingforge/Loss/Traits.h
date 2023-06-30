#pragma once

// Baseclass for all losses. A loss is some (possibly simplified)
// function `L(x, y, ŷ)`, of features `x`, targets `y` and outputs
// `ŷ = f(x)` for some function `f`.
struct Loss {
    virtual constexpr double operator()(double agreement) const = 0;
};

// A loss is considered **supervised**, if all the information needed
// to compute `L(x, ŷ, y)` are contained in `ŷ` and `y`, and thus allows
// for the simplification `L(ŷ, y)`.
struct SupervisedLoss : public Loss {
    // TODO: providing a base fallback for `()` overload. Is this correct?
    constexpr double operator()(double agreement) const override {
        return agreement;
    }
};

// A supervised loss that can be simplified to `L(ŷ, y) = L(ŷ - y)`
// is considered **distance-based**.
struct DistanceLoss : public SupervisedLoss {
    using SupervisedLoss::operator();
    constexpr double operator()(double output, double target) const {
        return (*this)(output - target);
    }
};

// A supervised loss with targets `y ∈ {-1, 1}`, and which
// can be simplified to `L(ŷ, y) = L(ŷ⋅y)` is considered
// **margin-based**.
struct MarginLoss : public SupervisedLoss {
    using SupervisedLoss::operator();
    constexpr double operator()(double output, double target) const {
        return (*this)(output * target);
    }
};

// A loss is considered **unsupervised**, if all the information needed
// to compute `L(x, ŷ, y)` are contained in `x` and `ŷ`, and thus allows
// for the simplification `L(x, ŷ)`.
struct UnsupervisedLoss : public Loss {};

// Compute the analytical derivative with respect to the `output` for the `loss` function.
// Note that `target` and `output` can be of different numeric type, in which case promotion is performed in the manner appropriate for the given loss.
template<typename Loss>
double deriv(const Loss&, double, double) {
    return double(0);
}
double deriv(const Loss&, double) {
    return double(0);
}

// Compute the second derivative with respect to the `output` for the `loss` function.
// Note that `target` and `output` can be of different numeric type, in which case promotion is performed in the manner appropriate for the given loss.
template<typename Loss>
double deriv2(const Loss&, double, double) {
    return double(0);
}
double deriv2(const Loss&, double) {
    return double(0);
}

// Return `true` if the given `loss` denotes a strongly convex function.
// A function `f : ℝⁿ → ℝ` is `m`-strongly convex if its domain is a convex set, and if for all `x, y` in that domain where `x ≠ y`, and `θ` such that for `0 ≤ θ ≤ 1`, we have `f(θ x + (1 - θ)y) < θ f(x) + (1 - θ) f(y) - 0.5 m ⋅ θ (1 - θ) | x - y |₂²`.
bool isstronglyconvex(const SupervisedLoss&) {
    return false;
}

// Return `true` if the given `loss` denotes a strictly convex function.
// A function `f : ℝⁿ → ℝ` is strictly convex if its domain is a convex set and if for all `x, y` in that domain where `x ≠ y`, with `θ` such that for `0 < θ < 1`, we have `f(θ x + (1 - θ) y) < θ f(x) + (1 - θ) f(y)`.
bool isstrictlyconvex(const SupervisedLoss& loss) {
    return isstronglyconvex(loss);
}

// Return `true` if the given `loss` denotes a convex function.
// A function `f: ℝⁿ → ℝ` is convex if its domain is a convex set and if for all `x, y` in that domain, with `θ` such that for `0 ≦ θ ≦ 1`, we have `f(θ x + (1 - θ) y) ≦ θ f(x) + (1 - θ) f(y)`.
bool isconvex(const SupervisedLoss& loss) {
    return isstrictlyconvex(loss);
}

// Return `true` if the given `loss` is twice differentiable (optionally limited to the given point `x` if specified).
// A function `f : ℝⁿ → ℝ` is said to be twice differentiable at a point `x` in the interior domain of `f`, if the function derivative for `∇f` exists at `x`: `∇²f(x) = D∇f(x)`.
// A function is twice differentiable if its domain is open, and it is twice differentiable at every point `x`.
bool istwicedifferentiable(const SupervisedLoss&) {
    return false;
}

bool istwicedifferentiable(const SupervisedLoss& loss, double at) {
    return istwicedifferentiable(loss);
}

// Return `true` if the given `loss` is differentiable (optionally limited to the given point `x` if specified).
// A function `f : ℝⁿ → ℝᵐ` is differentiable at a point `x` in the interior domain of `f` if there exists a matrix `Df(x) ∈ ℝ^(m × n)` such that it satisfies:
// `lim_{z ≠ x, z → x} (|f(z) - f(x) - Df(x)(z-x)|₂) / |z - x|₂ = 0`.
// A function is differentiable if its domain is open, and it is differentiable at every point `x`.
bool isdifferentiable(const SupervisedLoss& loss) {
    return istwicedifferentiable(loss);
}

bool isdifferentiable(const SupervisedLoss& loss, double at) {
    return isdifferentiable(loss);
}

// Return `true` if the given `loss` function is Lipschitz continuous.
// A supervised loss function `L : Y × ℝ → [0, ∞)` is Lipschitz continous, if there exists a finite constant `M < ∞` such that `|L(y, t) - L(y, t′)| ≤ M |t - t′|, ∀ (y, t) ∈ Y × ℝ`.
bool islipschitzcont(const SupervisedLoss&) {
    return false;
}

// Return `true` if the given `loss` function is locally-Lipschitz continuous.
// A supervised loss `L : Y × ℝ → [0, ∞)` is called locally Lipschitz continuous if for all `a ≥ 0` there exists a constant `cₐ ≥ 0`, such that `sup_{y ∈ Y} | L(y,t) − L(y,t′) | ≤ cₐ |t − t′|, t, t′ ∈ [−a,a]`.
// Every convex function is locally Lipschitz continuous.
bool islocallylipschitzcont(const SupervisedLoss& loss) {
    return isconvex(loss) || islipschitzcont(loss);
}

// Return `true` if the given `loss` denotes a Nemitski loss function.
// We call a supervised loss function `L : Y × ℝ → [0,∞)` a Nemitski loss if there exist a measurable function `b : Y → [0, ∞)` and an increasing function `h : [0, ∞) → [0, ∞)` such that `L(y,ŷ) ≤ b(y) + h(|ŷ|), (y, ŷ) ∈ Y × ℝ`.
// If a loss is locally Lipschitz continuous then it is a Nemitski loss.
bool isnemitski(const SupervisedLoss& loss) {
    return islocallylipschitzcont(loss);
}

bool isnemitski(const MarginLoss&) {
    return true;
}

// Return `true` if the given `loss` is Fisher consistent.
// We call a supervised loss function `L : Y × ℝ → [0,∞)` a Fisher consistent loss if the population minimizer of the risk `E[L(y,f(x))]` for all measurable functions leads to the Bayes optimal decision rule.
bool isunivfishercons(const Loss&) {
    return false;
}

bool isfishercons(const Loss& loss) {
    return isunivfishercons(loss);
}

// Return `true` if the given `loss` function is clipable. A supervised loss `L : Y × ℝ → [0,∞)` can be clipped at `M > 0` if, for all `(y,t) ∈ Y × ℝ`, `L(y, t̂) ≤ L(y, t)` where `t̂` denotes the clipped value of `t` at `± M`.
// That is `t̂ = -M` if `t < -M`, `t̂ = t` if `t ∈ [-M, M]`, and `t = M` if `t > M`.
bool isclipable(const SupervisedLoss&) {
    return false;
}

bool isclipable(const DistanceLoss&) {
    return true;
}

// Return `true` if the given `loss` is a distance-based loss.
// A supervised loss function `L : Y × ℝ → [0,∞)` is said to be distance-based, if there exists a representing function `ψ : ℝ → [0,∞)` satisfying `ψ(0) = 0` and `L(y, ŷ) = ψ (ŷ - y), (y, ŷ) ∈ Y × ℝ`.
bool isdistancebased(const Loss&) {
    return false;
}

bool isdistancebased(const DistanceLoss&) {
    return true;
}

// Return `true` if the given `loss` is a margin-based loss.
// A supervised loss function `L : Y × ℝ → [0,∞)` is said to be margin-based, if there exists a representing function `ψ : ℝ → [0,∞)` satisfying `L(y, ŷ) = ψ(y⋅ŷ), (y, ŷ) ∈ Y × ℝ`.
bool ismarginbased(const Loss&) {
    return false;
}

bool ismarginbased(const MarginLoss&) {
    return true;
}

bool isclasscalibrated(const SupervisedLoss&) {
    return false;
}

bool isclasscalibrated(const MarginLoss& loss) {
    return isconvex(loss) && isdifferentiable(loss, 0) && deriv(loss, 0) < 0;
}

// Return `true` if the given loss is a symmetric loss.
// A function `f : ℝ → [0,∞)` is said to be symmetric about origin if we have `f(x) = f(-x), ∀ x ∈ ℝ`.
// A distance-based loss is said to be symmetric if its representing function is symmetric.
bool issymmetric(const SupervisedLoss&) {
    return false;
}

// Return `true` if the given `loss` is a minimizable loss.
bool isminimizable(const SupervisedLoss& loss) {
    return isconvex(loss);
}