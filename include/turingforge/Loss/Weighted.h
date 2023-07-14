#pragma once

#include <stdexcept>

#include "Traits.h"

template <typename L>
struct WeightedMarginLoss : SupervisedLoss {
    L loss;
    double weight;

    WeightedMarginLoss(L loss, double weight) : loss(loss), weight(weight) {
        if (!(weight >= 0 && weight <= 1)) {
            throw std::invalid_argument("The given weight has to be a number in the interval [0, 1]");
        }
    }

    double operator()(double output, double target) const {
        // We interpret the W to be the weight of the positive class
        if (target > 0)
            return weight * loss(output, target);
        else
            return (1 - weight) * loss(output, target);
    }

    double deriv(double output, double target) override {
        // We interpret the W to be the weight of the positive class
        if (target > 0) {
            return static_cast<double>(weight) * loss.deriv(output, target);
        } else {
            return (1 - static_cast<double>(weight)) * loss.deriv(output, target);
        }
    }

    double deriv2(double output, double target) override {
        // We interpret the W to be the weight of the positive class
        if (target > 0) {
            return static_cast<double>(weight) * loss.deriv2(output, target);
        } else {
            return (1 - static_cast<double>(weight)) * loss.deriv2(output, target);
        }
    }

    bool isclasscalibrated() override {
        return weight == 0.5 && loss.isclasscalibrated();
    }

    bool issymmetric() override {
        return false;
    }

    // Functions for checking properties
    constexpr bool isminimizable() override {
        return loss.isminimizable();
    }

    constexpr bool isdifferentiable() override {
        return loss.isdifferentiable();
    }

    constexpr bool istwicedifferentiable() override {
        return loss.istwicedifferentiable();
    }

    constexpr bool isconvex() override {
        return loss.isconvex();
    }

    constexpr bool isstrictlyconvex() override {
        return loss.isstrictlyconvex();
    }

    constexpr bool isstronglyconvex() override {
        return loss.isstronglyconvex();
    }

    constexpr bool isnemitski() override {
        return loss.isnemitski();
    }

    constexpr bool isunivfishercons() override {
        return loss.isunivfishercons();
    }

    constexpr bool isfishercons() override {
        return loss.isfishercons();
    }

    constexpr bool islipschitzcont() override {
        return loss.islipschitzcont();
    }

    constexpr bool islocallylipschitzcont() override {
        return loss.islocallylipschitzcont();
    }

    constexpr bool isclipable() override {
        return loss.isclipable();
    }

    constexpr bool ismarginbased() override {
        return loss.ismarginbased();
    }

    constexpr bool isdistancebased() override {
        return loss.isdistancebased();
    }

    // Functions for differentiability at a point
    constexpr bool isdifferentiable(double at) override {
        return loss.isdifferentiable(at);
    }

    constexpr bool istwicedifferentiable(double at) override {
        return loss.istwicedifferentiable(at);
    }  
};