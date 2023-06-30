#pragma once

#include <cmath>

template <typename T>
inline constexpr int signum(T x, std::false_type is_signed) {
    return T(0) < x;
}

template <typename T>
inline constexpr int signum(T x, std::true_type is_signed) {
    return (T(0) < x) - (x < T(0));
}

template <typename T>
inline constexpr int signum(T x) {
    return signum(x, std::is_signed<T>());
}

template <typename T>
inline constexpr T abs2(T x) {
    return x * x;
}

template <typename T>
inline constexpr T sech(T x) {
    return 1 / std::cosh(x);
}

constexpr double softplus(double x) {
    return x > 0.0 ? x + std::log1p(std::exp(-x)) : std::log1p(std::exp(x));
}

constexpr double log_cosh(double x) {
    return x + softplus(-2 * x) - 0.6931471805599453; // std::log(2.0)
}