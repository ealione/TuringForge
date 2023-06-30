#pragma once

#include <optional>
#include <limits>

namespace Optim {
    template<typename T = double, typename TCallback = std::nullptr_t>
    struct Options {
        T x_abstol;
        T x_reltol;
        T f_abstol;
        T f_reltol;
        T g_abstol;
        T g_reltol;
        T outer_x_abstol;
        T outer_x_reltol;
        T outer_f_abstol;
        T outer_f_reltol;
        T outer_g_abstol;
        T outer_g_reltol;
        int f_calls_limit;
        int g_calls_limit;
        int h_calls_limit;
        bool allow_f_increases;
        bool allow_outer_f_increases;
        int successive_f_tol;
        int iterations;
        int outer_iterations;
        bool store_trace;
        bool trace_simplex;
        bool show_trace;
        bool extended_trace;
        int show_every;
        TCallback callback;
        double time_limit;

        explicit Options(const std::optional<T> &x_tol = std::nullopt,
                const std::optional<T> &f_tol = std::nullopt,
                const std::optional<T> &g_tol = std::nullopt,
                const T &x_abstol = 0.0,
                const T &x_reltol = 0.0,
                const T &f_abstol = 0.0,
                const T &f_reltol = 0.0,
                const T &g_abstol = 1e-8,
                const T &g_reltol = 1e-8,
                const T &outer_x_tol = 0.0,
                const T &outer_f_tol = 0.0,
                const std::optional<T> &outer_g_tol = std::nullopt,
                const T &outer_x_abstol = 0.0,
                const T &outer_x_reltol = 0.0,
                const T &outer_f_abstol = 0.0,
                const T &outer_f_reltol = 0.0,
                const T &outer_g_abstol = 1e-8,
                const T &outer_g_reltol = 1e-8,
                const int f_calls_limit = 0,
                const int g_calls_limit = 0,
                const int h_calls_limit = 0,
                const bool allow_f_increases = true,
                const bool allow_outer_f_increases = true,
                const int successive_f_tol = 1,
                const int iterations = 1000,
                const int outer_iterations = 1000,
                const bool store_trace = false,
                const bool trace_simplex = false,
                const bool show_trace = false,
                const bool extended_trace = false,
                const int show_every = 1,
                const std::optional<TCallback> &callback = std::nullopt,
                double time_limit = std::numeric_limits<double>::quiet_NaN())
                : x_abstol(x_abstol)
                , x_reltol(x_reltol)
                , f_abstol(f_abstol)
                , f_reltol(f_reltol)
                , g_abstol(g_abstol)
                , g_reltol(g_reltol)
                , outer_x_abstol(outer_x_abstol)
                , outer_x_reltol(outer_x_reltol)
                , outer_f_abstol(outer_f_abstol)
                , outer_f_reltol(outer_f_reltol)
                , outer_g_abstol(outer_g_abstol)
                , outer_g_reltol(outer_g_reltol)
                , f_calls_limit(f_calls_limit)
                , g_calls_limit(g_calls_limit)
                , h_calls_limit(h_calls_limit)
                , allow_f_increases(allow_f_increases)
                , allow_outer_f_increases(allow_outer_f_increases)
                , successive_f_tol(successive_f_tol)
                , iterations(iterations)
                , outer_iterations(outer_iterations)
                , store_trace(store_trace)
                , trace_simplex(trace_simplex)
                , show_trace(show_trace)
                , extended_trace(extended_trace)
                , show_every(show_every > 0 ? show_every : 1)
                , callback(callback.value_or(nullptr))
                , time_limit(time_limit) {
            if (x_tol.has_value()) {
                x_abstol = x_tol.value();
            }
            if (g_tol.has_value()) {
                g_abstol = g_tol.value();
            }
            if (f_tol.has_value()) {
                f_reltol = f_tol.value();
            }
            if (outer_x_tol.has_value()) {
                outer_x_abstol = outer_x_tol.value();
            }
            if (outer_g_tol.has_value()) {
                outer_g_abstol = outer_g_tol.value();
            }
            if (outer_f_tol.has_value()) {
                outer_f_reltol = outer_f_tol.value();
            }
        }
    };
}  // namespace Optim