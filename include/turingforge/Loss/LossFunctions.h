#pragma once

#include <vector>
#include <functional>
#include <stdexcept>

#include "Traits.h"

// Available losses
#include "Margin.h"
#include "Distance.h"
#include "Other.h"

// Meta-losses
#include "Scaled.h"
#include "Weighted.h"

// Fallback to unary functions
auto deriv(DistanceLoss const& loss, double output, double target) {
    return loss(output - target);
}

auto deriv2(DistanceLoss const& loss, double output, double target) {
    return loss(output - target);
}

auto deriv(MarginLoss const& loss, double output, double target) {
    return loss(output * target);
}

auto deriv2(MarginLoss const& loss, double output, double target) {
    return loss(output * target);
}

// Aggregated behaviour

/*
 * Return sum of `loss` values over the iterables `outputs` and `targets`.
 */
template <typename  L>
double sum(const L& loss, const std::vector<double>& outputs, const std::vector<double>& targets) {
    if (outputs.size() != targets.size()) {
        throw std::invalid_argument("Outputs and targets must have the same size");
    }
    double s = 0;
    for (size_t i = 0; i < outputs.size(); ++i) {
        s += loss(outputs[i], targets[i]);
    }
    return s;
}

/*
 * Return sum of `loss` values over the iterables `outputs` and `targets`.
 * The `weights` determine the importance of each observation. The option
 * `normalize` divides the result by the sum of the weights.
 */
template <typename  L>
double sum(const L& loss, const std::vector<double>& outputs, const std::vector<double>& targets,
      const std::vector<double>& weights, bool normalize = true) {
    if (outputs.size() != targets.size() || outputs.size() != weights.size()) {
        throw std::invalid_argument("Outputs, targets, and weights must have the same size");
    }
    double s = 0;
    double n = 0;
    for (size_t i = 0; i < outputs.size(); ++i) {
        s += weights[i] * loss(outputs[i], targets[i]);
        n += weights[i];
    }
    if (normalize && n == 0) {
        throw std::invalid_argument("Weights must not be all zero");
    }
    return s / (normalize ? n : 1);
}

/*
 * Return mean of `loss` values over the iterables `outputs` and `targets`.
 */
template <typename  L>
double mean(const L& loss, const std::vector<double>& outputs, const std::vector<double>& targets) {
    if (outputs.size() != targets.size()) {
        throw std::invalid_argument("Outputs and targets must have the same size");
    }
    double m = 0;
    for (size_t i = 0; i < outputs.size(); ++i) {
        m += loss(outputs[i], targets[i]);
    }
    return m / outputs.size();
}

/*
 * Return mean of `loss` values over the iterables `outputs` and `targets`.
 * The `weights` determine the importance of each observation. The option
 * `normalize` divides the result by the sum of the weights.
 */
template <typename  L>
double mean(const L& loss, const std::vector<double>& outputs, const std::vector<double>& targets,
       const std::vector<double>& weights, bool normalize = true) {
    if (outputs.size() != targets.size() || outputs.size() != weights.size()) {
        throw std::invalid_argument("Outputs, targets, and weights must have the same size");
    }
    double m = 0;
    double n = 0;
    for (size_t i = 0; i < outputs.size(); ++i) {
        m += weights[i] * loss(outputs[i], targets[i]);
        n += weights[i];
    }
    if (normalize && n == 0) {
        throw std::invalid_argument("Weights must not be all zero");
    }
    return m / (normalize ? n : 1);
}