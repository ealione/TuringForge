#pragma once

#include "Traits.h"

// Can be used to represent a `K` times scaled version of a given type of loss `L`.
template <typename L, typename K>
struct ScaledLoss : SupervisedLoss {
    L loss;
    K k;
};


// Functions for differentiability at a point
template <typename L, typename K>
double deriv(const ScaledLoss<L, K>& loss, double output, double target) {
    return loss.k * loss.deriv(output, target);
}

template <typename L, typename K>
double deriv2(const ScaledLoss<L, K>& loss, double output, double target) {
    return loss.k * loss.deriv2(output, target);
}

// Functions for checking properties
template <typename L, typename K>
constexpr bool isminimizable(const ScaledLoss<L, K>& loss) {
    return loss.isminimizable();
}

template <typename L, typename K>
constexpr bool isdifferentiable(const ScaledLoss<L, K>& loss) {
    return loss.isdifferentiable();
}

template <typename L, typename K>
constexpr bool istwicedifferentiable(const ScaledLoss<L, K>& loss) {
    return loss.istwicedifferentiable();
}

template <typename L, typename K>
constexpr bool isconvex(const ScaledLoss<L, K>& loss) {
    return loss.isconvex();
}

template <typename L, typename K>
constexpr bool isstrictlyconvex(const ScaledLoss<L, K>& loss) {
    return loss.isstrictlyconvex();
}

template <typename L, typename K>
constexpr bool isstronglyconvex(const ScaledLoss<L, K>& loss) {
    return loss.isstronglyconvex();
}

template <typename L, typename K>
constexpr bool isnemitski(const ScaledLoss<L, K>& loss) {
    return loss.isnemitski();
}

template <typename L, typename K>
constexpr bool isunivfishercons(const ScaledLoss<L, K>& loss) {
    return loss.isunivfishercons();
}

template <typename L, typename K>
constexpr bool isfishercons(const ScaledLoss<L, K>& loss) {
    return loss.isfishercons();
}

template <typename L, typename K>
constexpr bool islipschitzcont(const ScaledLoss<L, K>& loss) {
    return loss.islipschitzcont();
}

template <typename L, typename K>
constexpr bool islocallylipschitzcont(const ScaledLoss<L, K>& loss) {
    return loss.islocallylipschitzcont();
}

template <typename L, typename K>
constexpr bool isclipable(const ScaledLoss<L, K>& loss) {
    return loss.isclipable();
}

template <typename L, typename K>
constexpr bool ismarginbased(const ScaledLoss<L, K>& loss) {
    return loss.ismarginbased();
}

template <typename L, typename K>
constexpr bool isclasscalibrated(const ScaledLoss<L, K>& loss) {
    return loss.isclasscalibrated();
}

template <typename L, typename K>
constexpr bool isdistancebased(const ScaledLoss<L, K>& loss) {
    return loss.isdistancebased();
}

template <typename L, typename K>
constexpr bool issymmetric(const ScaledLoss<L, K>& loss) {
    return loss.issymmetric();
}