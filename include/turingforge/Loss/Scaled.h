#pragma once

#include "Traits.h"

// Can be used to represent a `K` times scaled version of a given type of loss `L`.
template <typename L>
struct ScaledLoss : SupervisedLoss {
    L loss;
    double k;

    ScaledLoss(L loss, double k) : loss(loss), k(k) {}

    double operator()(double output, double target) const {
        return k * loss(output, target);
    }
    
    // Functions for differentiability at a point
    double deriv(double output, double target) override {
        return k * loss.deriv(output, target);
    }

    double deriv2(double output, double target) override {
        return k * loss.deriv2(output, target);
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

    constexpr bool isclasscalibrated() override {
        return loss.isclasscalibrated();
    }

    constexpr bool isdistancebased() override {
        return loss.isdistancebased();
    }

    constexpr bool issymmetric() override {
        return loss.issymmetric();
    }
};