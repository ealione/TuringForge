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
};

double deriv(const MisclassLoss&, bool agreement) {
    return double(0);
}

double deriv2(const MisclassLoss&, bool agreement) {
    return double(0);
}

auto deriv(const MisclassLoss& loss, double output, double target) {
    return deriv(loss, target == output);
}

auto deriv2(const MisclassLoss& loss, double output, double target) {
    return deriv2(loss, target == output);
}

bool isminimizable(const MisclassLoss&) {
    return false;
}

bool isdifferentiable(const MisclassLoss&) {
    return false;
}

bool isdifferentiable(const MisclassLoss&, double at) {
    return at != 0;
}

bool istwicedifferentiable(const MisclassLoss&) {
    return false;
}

bool istwicedifferentiable(const MisclassLoss&, double at) {
    return at != 0;
}

bool isnemitski(const MisclassLoss&) {
    return false;
}

bool islipschitzcont(const MisclassLoss&) {
    return false;
}

bool isconvex(const MisclassLoss&) {
    return false;
}

bool isclasscalibrated(const MisclassLoss&) {
    return false;
}

bool isclipable(const MisclassLoss&) {
    return false;
}

/*
 * PoissonLoss <: SupervisedLoss
 *
 * Loss under a Poisson noise distribution (KL-divergence)
 *
 * ``L(output, target) = exp(output) - target*output``
 */

struct PoissonLoss : SupervisedLoss {
    auto operator()(const PoissonLoss&, double output, double target) {
        return std::exp(output) - target * output;
    }
};

auto deriv(const PoissonLoss&, double output, double target) {
    return std::exp(output) - target;
}

auto deriv2(const PoissonLoss&, double output, double target) {
    return std::exp(output);
}

bool isdifferentiable(const PoissonLoss&) {
    return true;
}

bool isdifferentiable(const PoissonLoss&, double, double) {
    return true;
}

bool istwicedifferentiable(const PoissonLoss&) {
    return true;
}

bool istwicedifferentiable(const PoissonLoss&, double, double) {
    return true;
}

bool islipschitzcont(const PoissonLoss&) {
    return false;
}

bool isconvex(const PoissonLoss&) {
    return true;
}

bool isstronglyconvex(const PoissonLoss&) {
    return false;
}

/*
 * CrossEntropyLoss <: SupervisedLoss
 *
 * The cross-entropy loss is defined as:
 *
 * ``L(output, target) = - target*log(output) - (1-target)*log(1-output)``
 */

struct CrossEntropyLoss : SupervisedLoss {
    auto operator()(const CrossEntropyLoss&, double output, double target) {
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
};

auto deriv(const CrossEntropyLoss&, double output, double target) {
    return (1 - target) / (1 - output) - target / output;
}

auto deriv2(const CrossEntropyLoss&, double output, double target) {
    return (1 - target) / std::pow(1 - output, 2) + target / std::pow(output, 2);
}

bool isdifferentiable(const CrossEntropyLoss&) {
    return true;
}

bool isdifferentiable(const CrossEntropyLoss&, double, double) {
    return true;
}

bool istwicedifferentiable(const CrossEntropyLoss&) {
    return true;
}

bool istwicedifferentiable(const CrossEntropyLoss&, double, double) {
    return true;
}

bool isconvex(const CrossEntropyLoss&) {
    return true;
}