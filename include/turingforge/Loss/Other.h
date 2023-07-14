#pragma once

#include <cmath>
#include <stdexcept>

#include "Traits.h"

/*
 *  MisclassLoss{R<:AbstractFloat} <: SupervisedLoss
 *
 * Misclassification loss that assigns `1` for misclassified
 * examples and `0` otherwise. It is a generalization of
 * `ZeroOneLoss` for more than two classes.
 */

struct MisclassLoss : SupervisedLoss {
    double operator()(bool agreement) {
        return !agreement;
    }

    auto operator()(double output, double target) {
        return (*this)(target == output);
    }

    static constexpr double deriv(bool agreement) {
        return double(0);
    }

    static constexpr double deriv2(bool agreement) {
        return double(0);
    }

    constexpr double deriv(double output, double target) override {
        return deriv(target == output);
    }

    constexpr double deriv2(double output, double target) override {
        return deriv2(target == output);
    }

    constexpr bool isminimizable() override {
        return false;
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
        return false;
    }

    constexpr bool islipschitzcont() override {
        return false;
    }

    constexpr bool isconvex() override {
        return false;
    }

    constexpr bool isclasscalibrated() override {
        return false;
    }

    constexpr bool isclipable() override {
        return false;
    }
};

/*
 * PoissonLoss <: SupervisedLoss
 *
 * Loss under a Poisson noise distribution (KL-divergence)
 *
 * ``L(output, target) = exp(output) - target*output``
 */

struct PoissonLoss : SupervisedLoss {
    auto operator()(double output, double target) {
        return std::exp(output) - target * output;
    }

    constexpr double deriv(double output, double target) override {
        return std::exp(output) - target;
    }

    constexpr double deriv2(double output, double target) override {
        return std::exp(output);
    }

    constexpr bool isdifferentiable() override {
        return true;
    }

    static constexpr bool isdifferentiable(double, double) {
        return true;
    }

    constexpr bool istwicedifferentiable() override {
        return true;
    }

    static constexpr bool istwicedifferentiable(double, double) {
        return true;
    }

    constexpr bool islipschitzcont() override {
        return false;
    }

    constexpr bool isconvex() override {
        return true;
    }

    constexpr bool isstronglyconvex() override {
        return false;
    }
};

/*
 * CrossEntropyLoss <: SupervisedLoss
 *
 * The cross-entropy loss is defined as:
 *
 * ``L(output, target) = - target*log(output) - (1-target)*log(1-output)``
 */

struct CrossEntropyLoss : SupervisedLoss {
    auto operator()(double output, double target) {
        if (target >= 0 && target <= 1) {
            if (output >= 0 && output <= 1) {
                if (target == 0) {
                    return -std::log(1 - output);
                } else if (target == 1) {
                    return -std::log(output);
                } else {
                    return -(target * std::log(output) + (1 - target) * std::log(1 - output));
                }
            } else {
                throw std::domain_error("Output must be in [0,1]");
            }
        } else {
            throw std::domain_error("Target must be in [0,1]");
        }
    }

    constexpr double deriv(double output, double target) override {
        return (1 - target) / (1 - output) - target / output;
    }

    constexpr double deriv2(double output, double target) override {
        return (1 - target) / std::pow(1 - output, 2) + target / std::pow(output, 2);
    }

    constexpr bool isdifferentiable() override {
        return true;
    }

    static constexpr bool isdifferentiable(double, double) {
        return true;
    }

    constexpr bool istwicedifferentiable() override {
        return true;
    }

    static constexpr bool istwicedifferentiable(double, double) {
        return true;
    }

    constexpr bool isconvex() override {
        return true;
    }
};