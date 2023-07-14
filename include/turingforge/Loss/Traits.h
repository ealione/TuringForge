#pragma once

// Baseclass for all losses. A loss is some (possibly simplified)
// function `L(x, y, ŷ)`, of features `x`, targets `y` and outputs
// `ŷ = f(x)` for some function `f`.
struct Loss {
    virtual constexpr double operator()(double agreement) const = 0;

    // Return `true` if the given `loss` is Fisher consistent.
    // We call a supervised loss function `L : Y × ℝ → [0,∞)` a Fisher consistent loss if the population minimizer of the risk `E[L(y,f(x))]` for all measurable functions leads to the Bayes optimal decision rule.
    virtual bool isunivfishercons() {
        return false;
    }

    virtual bool isfishercons() {
        return isunivfishercons();
    }

    // Return `true` if the given `loss` is a distance-based loss.
    // A supervised loss function `L : Y × ℝ → [0,∞)` is said to be distance-based, if there exists a representing function `ψ : ℝ → [0,∞)` satisfying `ψ(0) = 0` and `L(y, ŷ) = ψ (ŷ - y), (y, ŷ) ∈ Y × ℝ`.
    virtual bool isdistancebased() {
        return false;
    }

    // Return `true` if the given `loss` is a margin-based loss.
    // A supervised loss function `L : Y × ℝ → [0,∞)` is said to be margin-based, if there exists a representing function `ψ : ℝ → [0,∞)` satisfying `L(y, ŷ) = ψ(y⋅ŷ), (y, ŷ) ∈ Y × ℝ`.
    virtual bool ismarginbased() {
        return false;
    }

    // Compute the analytical derivative with respect to the `output` for the `loss` function.
    // Note that `target` and `output` can be of different numeric type, in which case promotion is performed in the manner appropriate for the given loss.
    virtual double deriv(double, double) {
        return double(0);
    }

    virtual double deriv(double) {
        return double(0);
    }

    // Compute the second derivative with respect to the `output` for the `loss` function.
    // Note that `target` and `output` can be of different numeric type, in which case promotion is performed in the manner appropriate for the given loss.
    virtual double deriv2(double, double) {
        return double(0);
    }

    virtual double deriv2(double) {
        return double(0);
    }
};

// A loss is considered **supervised**, if all the information needed
// to compute `L(x, ŷ, y)` are contained in `ŷ` and `y`, and thus allows
// for the simplification `L(ŷ, y)`.
struct SupervisedLoss : public Loss {
    // TODO: providing a base fallback for `()` overload. Is this correct?
    constexpr double operator()(double agreement) const override {
        return agreement;
    }

    // Overload operator* to multiply a loss struct with a scalar
    SupervisedLoss operator*(double scalar) const {
        SupervisedLoss result;
        // TODO: Multiply each component of the loss struct by the scalar
        // and store the result in the new loss struct
        // ...
        return result;
    }

    SupervisedLoss operator*=(double scalar) const {
        return (*this) * scalar;
    }

    // Return `true` if the given `loss` denotes a strongly convex function.
    // A function `f : ℝⁿ → ℝ` is `m`-strongly convex if its domain is a convex set, and if for all `x, y` in that domain where `x ≠ y`, and `θ` such that for `0 ≤ θ ≤ 1`, we have `f(θ x + (1 - θ)y) < θ f(x) + (1 - θ) f(y) - 0.5 m ⋅ θ (1 - θ) | x - y |₂²`.
    virtual bool isstronglyconvex() {
        return false;
    }

    // Return `true` if the given `loss` denotes a strictly convex function.
    // A function `f : ℝⁿ → ℝ` is strictly convex if its domain is a convex set and if for all `x, y` in that domain where `x ≠ y`, with `θ` such that for `0 < θ < 1`, we have `f(θ x + (1 - θ) y) < θ f(x) + (1 - θ) f(y)`.
    virtual bool isstrictlyconvex() {
        return isstronglyconvex();
    }

    // Return `true` if the given `loss` denotes a convex function.
    // A function `f: ℝⁿ → ℝ` is convex if its domain is a convex set and if for all `x, y` in that domain, with `θ` such that for `0 ≦ θ ≦ 1`, we have `f(θ x + (1 - θ) y) ≦ θ f(x) + (1 - θ) f(y)`.
    virtual bool isconvex() {
        return isstrictlyconvex();
    }

    // Return `true` if the given `loss` is twice differentiable (optionally limited to the given point `x` if specified).
    // A function `f : ℝⁿ → ℝ` is said to be twice differentiable at a point `x` in the interior domain of `f`, if the function derivative for `∇f` exists at `x`: `∇²f(x) = D∇f(x)`.
    // A function is twice differentiable if its domain is open, and it is twice differentiable at every point `x`.
    virtual bool istwicedifferentiable() {
        return false;
    }

    virtual bool istwicedifferentiable(double) {
        return istwicedifferentiable();
    }

    // Return `true` if the given `loss` is differentiable (optionally limited to the given point `x` if specified).
    // A function `f : ℝⁿ → ℝᵐ` is differentiable at a point `x` in the interior domain of `f` if there exists a matrix `Df(x) ∈ ℝ^(m × n)` such that it satisfies:
    // `lim_{z ≠ x, z → x} (|f(z) - f(x) - Df(x)(z-x)|₂) / |z - x|₂ = 0`.
    // A function is differentiable if its domain is open, and it is differentiable at every point `x`.
    virtual bool isdifferentiable() {
        return istwicedifferentiable();
    }

    virtual bool isdifferentiable(double) {
        return isdifferentiable();
    }

    // Return `true` if the given `loss` function is Lipschitz continuous.
    // A supervised loss function `L : Y × ℝ → [0, ∞)` is Lipschitz continuous, if there exists a finite constant `M < ∞` such that `|L(y, t) - L(y, t′)| ≤ M |t - t′|, ∀ (y, t) ∈ Y × ℝ`.
    virtual bool islipschitzcont() {
        return false;
    }

    // Return `true` if the given `loss` function is locally-Lipschitz continuous.
    // A supervised loss `L : Y × ℝ → [0, ∞)` is called locally Lipschitz continuous if for all `a ≥ 0` there exists a constant `cₐ ≥ 0`, such that `sup_{y ∈ Y} | L(y,t) − L(y,t′) | ≤ cₐ |t − t′|, t, t′ ∈ [−a,a]`.
    // Every convex function is locally Lipschitz continuous.
    virtual bool islocallylipschitzcont() {
        return isconvex() || islipschitzcont();
    }

    // Return `true` if the given `loss` denotes a Nemitski loss function.
    // We call a supervised loss function `L : Y × ℝ → [0,∞)` a Nemitski loss if there exist a measurable function `b : Y → [0, ∞)` and an increasing function `h : [0, ∞) → [0, ∞)` such that `L(y,ŷ) ≤ b(y) + h(|ŷ|), (y, ŷ) ∈ Y × ℝ`.
    // If a loss is locally Lipschitz continuous then it is a Nemitski loss.
    virtual bool isnemitski() {
        return islocallylipschitzcont();
    }

    // Return `true` if the given `loss` function is clipable. A supervised loss `L : Y × ℝ → [0,∞)` can be clipped at `M > 0` if, for all `(y,t) ∈ Y × ℝ`, `L(y, t̂) ≤ L(y, t)` where `t̂` denotes the clipped value of `t` at `± M`.
    // That is `t̂ = -M` if `t < -M`, `t̂ = t` if `t ∈ [-M, M]`, and `t = M` if `t > M`.
    virtual bool isclipable() {
        return false;
    }

    virtual bool isclasscalibrated() {
        return false;
    }

    // Return `true` if the given loss is a symmetric loss.
    // A function `f : ℝ → [0,∞)` is said to be symmetric about origin if we have `f(x) = f(-x), ∀ x ∈ ℝ`.
    // A distance-based loss is said to be symmetric if its representing function is symmetric.
    virtual bool issymmetric() {
        return false;
    }

    // Return `true` if the given `loss` is a minimizable loss.
    virtual bool isminimizable() {
        return isconvex();
    }
};

inline SupervisedLoss operator*(double scalar, const SupervisedLoss& loss) {
    return loss * scalar;
}

// A supervised loss that can be simplified to `L(ŷ, y) = L(ŷ - y)`
// is considered **distance-based**.
struct DistanceLoss : public SupervisedLoss {
    using SupervisedLoss::operator();
    constexpr double operator()(double output, double target) const {
        return (*this)(output - target);
    }

    double deriv(double output, double target) override {
        return SupervisedLoss::deriv(output - target);
    }

    double deriv2(double output, double target) override {
        return SupervisedLoss::deriv2(output - target);
    }

    bool isclipable() override {
        return true;
    }

    bool isdistancebased() override {
        return true;
    }
};

// A supervised loss with targets `y ∈ {-1, 1}`, and which
// can be simplified to `L(ŷ, y) = L(ŷ⋅y)` is considered
// **margin-based**.
struct MarginLoss : public SupervisedLoss {
    using SupervisedLoss::operator(), SupervisedLoss::deriv, SupervisedLoss::deriv2;
    constexpr double operator()(double output, double target) const {
        return (*this)(output * target);
    }

    bool isnemitski() override {
        return true;
    }

    bool ismarginbased() override {
        return true;
    }

    bool isclasscalibrated() override {
        return isconvex() && isdifferentiable(0) && deriv(0) < 0;
    }

    double deriv(double output, double target) override {
        return target * deriv(output * target);
    }

    double deriv2(double output, double target) override {
        return deriv2(output * target);
    }
};

// A loss is considered **unsupervised**, if all the information needed
// to compute `L(x, ŷ, y)` are contained in `x` and `ŷ`, and thus allows
// for the simplification `L(x, ŷ)`.
struct UnsupervisedLoss : public Loss {};