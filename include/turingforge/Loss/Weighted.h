#pragma once

#include <stdexcept>

#include "Traits.h"

template <typename L, typename W>
struct WeightedMarginLoss : SupervisedLoss {
    L loss;
    W weight;

    explicit WeightedMarginLoss(const L& loss) : loss(loss) {}
    WeightedMarginLoss(const L& loss, const W& weight) : loss(loss), weight(weight) {
        if (!(weight >= 0 && weight <= 1)) {
            throw std::invalid_argument("The given weight has to be a number in the interval [0, 1]");
        }
    }
    WeightedMarginLoss(const WeightedMarginLoss<L, W>& l, double output, double target) {
        if (target > 0) {
            loss = static_cast<double>(l.weight) * l.loss(output, target);
        } else {
            loss = (1 - static_cast<double>(l.weight)) * l.loss(output, target);
        }
        weight *= l.weight;
    }

    double operator()(double output, double target) const {
        // We interpret the W to be the weight of the positive class
        if (target > 0)
            return weight * loss(output, target);
        else
            return (1 - weight) * loss(output, target);
    }    
};

template <typename L, typename W>
double deriv(const WeightedMarginLoss<L, W>& l, double output, double target) {
    // We interpret the W to be the weight of the positive class
    if (target > 0) {
        return static_cast<double>(l.weight) * deriv(l.loss, output, target);
    } else {
        return (1 - static_cast<double>(l.weight)) * deriv(l.loss, output, target);
    }
}

template <typename L, typename W>
double deriv2(const WeightedMarginLoss<L, W>& l, double output, double target) {
    // We interpret the W to be the weight of the positive class
    if (target > 0) {
        return static_cast<double>(l.weight) * deriv2(l.loss, output, target);
    } else {
        return (1 - static_cast<double>(l.weight)) * deriv2(l.loss, output, target);
    }
}

template <typename L, typename W>
bool isclasscalibrated(const WeightedMarginLoss<L, W>& l) {
    return l.weight == 0.5 && isclasscalibrated(l.loss);
}

template <typename L, typename W>
bool issymmetric(const WeightedMarginLoss<L, W>&) {
    return false;
}

// Functions for checking properties
template <typename L, typename W>
constexpr bool isminimizable(const WeightedMarginLoss<L, W>& loss) {
    return loss.isminimizable();
}

template <typename L, typename W>
constexpr bool isdifferentiable(const WeightedMarginLoss<L, W>& loss) {
    return loss.isdifferentiable();
}

template <typename L, typename W>
constexpr bool istwicedifferentiable(const WeightedMarginLoss<L, W>& loss) {
    return loss.istwicedifferentiable();
}

template <typename L, typename W>
constexpr bool isconvex(const WeightedMarginLoss<L, W>& loss) {
    return loss.isconvex();
}

template <typename L, typename W>
constexpr bool isstrictlyconvex(const WeightedMarginLoss<L, W>& loss) {
    return loss.isstrictlyconvex();
}

template <typename L, typename W>
constexpr bool isstronglyconvex(const WeightedMarginLoss<L, W>& loss) {
    return loss.isstronglyconvex();
}

template <typename L, typename W>
constexpr bool isnemitski(const WeightedMarginLoss<L, W>& loss) {
    return loss.isnemitski();
}

template <typename L, typename W>
constexpr bool isunivfishercons(const WeightedMarginLoss<L, W>& loss) {
    return loss.isunivfishercons();
}

template <typename L, typename W>
constexpr bool isfishercons(const WeightedMarginLoss<L, W>& loss) {
    return loss.isfishercons();
}

template <typename L, typename W>
constexpr bool islipschitzcont(const WeightedMarginLoss<L, W>& loss) {
    return loss.islipschitzcont();
}

template <typename L, typename W>
constexpr bool islocallylipschitzcont(const WeightedMarginLoss<L, W>& loss) {
    return loss.islocallylipschitzcont();
}

template <typename L, typename W>
constexpr bool isclipable(const WeightedMarginLoss<L, W>& loss) {
    return loss.isclipable();
}

template <typename L, typename W>
constexpr bool ismarginbased(const WeightedMarginLoss<L, W>& loss) {
    return loss.ismarginbased();
}

template <typename L, typename W>
constexpr bool isdistancebased(const WeightedMarginLoss<L, W>& loss) {
    return loss.isdistancebased();
}

// Functions for differentiability at a point
template <typename L, typename W>
constexpr bool isdifferentiable(const WeightedMarginLoss<L, W>& loss, double at) {
    return loss.isdifferentiable(at);
}

template <typename L, typename W>
constexpr bool istwicedifferentiable(const WeightedMarginLoss<L, W>& loss, double at) {
    return loss.istwicedifferentiable(at);
}