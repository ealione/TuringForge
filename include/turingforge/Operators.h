#pragma once

#include <cmath>
#include <limits>
#include <type_traits>

template <typename T>
concept DataType = std::is_arithmetic_v<T>;

template <typename T>
T gamma(T x) {
    T out = std::tgamma(x);
    return std::isinf(out) ? std::numeric_limits<T>::quiet_NaN() : out;
}

template <typename T>
T atanh_clip(T x) {
    return std::atanh(std::fmod(x + 1, 2) - 1);
}

template <typename T>
T square(T x) {
    return x * x;
}

template <typename T>
T cube(T x) {
    return x * x * x;
}

template <typename T>
T safe_pow(T x, T y) {
    if (std::is_integral_v<T>) {
        if (y < T(0) && x == T(0)) {
            return std::numeric_limits<T>::quiet_NaN();
        }
    } else {
        if ((y > T(0) && x < T(0)) || (y < T(0) && x <= T(0))) {
            return std::numeric_limits<T>::quiet_NaN();
        }
    }
    return std::pow(x, y);
}

template <typename T>
T div(T x, T y) {
    return x / y;
}

template <typename T>
T safe_log(T x) {
    if (x <= T(0)) {
        return std::numeric_limits<T>::quiet_NaN();
    }
    return std::log(x);
}

template <typename T>
T safe_log2(T x) {
    if (x <= T(0)) {
        return std::numeric_limits<T>::quiet_NaN();
    }
    return std::log2(x);
}

template <typename T>
T safe_log10(T x) {
    if (x <= T(0)) {
        return std::numeric_limits<T>::quiet_NaN();
    }
    return std::log10(x);
}

template <typename T>
T safe_log1p(T x) {
    if (x <= T(-1)) {
        return std::numeric_limits<T>::quiet_NaN();
    }
    return std::log1p(x);
}

template <typename T>
T safe_acosh(T x) {
    if (x < T(1)) {
        return std::numeric_limits<T>::quiet_NaN();
    }
    return std::acosh(x);
}

template <typename T>
T safe_sqrt(T x) {
    if (x < T(0)) {
        return std::numeric_limits<T>::quiet_NaN();
    }
    return std::sqrt(x);
}

template <typename T>
T neg(T x) {
    return -x;
}

template <typename T>
T greater(T x, T y) {
    return static_cast<T>(x > y);
}

template <typename T>
T relu(T x) {
    return (x + std::abs(x)) / T(2);
}

template <typename T>
T logical_or(T x, T y) {
    return static_cast<T>((x > T(0)) || (y > T(0)));
}

template <typename T>
T logical_and(T x, T y) {
    return static_cast<T>((x > T(0)) && (y > T(0)));
}