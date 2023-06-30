#pragma once

#include <iostream>
#include <vector>
#include <cmath>
#include <random>

namespace UtilsModule {
    void debug(int verbosity, const std::string& message) {
        if (verbosity > 0) {
            std::cout << message << std::endl;
        }
    }

    void debug_inline(int verbosity, const std::string& message) {
        if (verbosity > 0) {
            std::cout << message;
        }
    }

    int pseudo_time = 0;

    int get_birth_order(bool deterministic = false) {
        if (deterministic) {
            return ++pseudo_time;
        }
        else {
            constexpr double resolution = 1e7;
            return static_cast<int>(std::round(resolution * std::time(nullptr)));
        }
    }

    bool is_anonymous_function(const std::string& op) {
        return op.size() > 1 && op[0] == '#' && op[1] >= '1' && op[1] <= '9';
    }

    template <typename T>
    std::vector<T> recursive_merge(const std::vector<T>& x) {
        return x;
    }

    template <typename T, typename... Args>
    std::vector<T> recursive_merge(const std::vector<T>& x, Args... args) {
        std::vector<T> result = recursive_merge(args...);
        result.insert(result.end(), x.begin(), x.end());
        return result;
    }

    template <typename T>
    std::vector<T> recursive_merge(const std::vector<std::vector<T>>& x) {
        std::vector<T> result;
        for (const auto& vec : x) {
            result.insert(result.end(), vec.begin(), vec.end());
        }
        return result;
    }

    template <typename T>
    std::pair<T, int> bottomk_fast(const std::vector<T>& x, int k) {
        if (k == 1) {
            auto it = std::min_element(x.begin(), x.end());
            int indmin = std::distance(x.begin(), it);
            return { *it, indmin };
        }
        else {
            std::vector<T> minval(x.begin(), x.begin() + k);
            std::vector<int> indmin(k);
            for (int i = k; i < x.size(); ++i) {
                if (x[i] < minval.back()) {
                    minval.back() = x[i];
                    indmin.back() = i;
                    for (int j = k - 1; j > 0; --j) {
                        if (minval[j] < minval[j - 1]) {
                            std::swap(minval[j], minval[j - 1]);
                            std::swap(indmin[j], indmin[j - 1]);
                        }
                        else {
                            break;
                        }
                    }
                }
            }
            return { minval, indmin };
        }
    }

    template <typename T>
    std::pair<T, int> findmin_fast(const std::vector<T>& x) {
        T minval = std::numeric_limits<T>::max();
        int indmin = 0;
        for (int i = 0; i < x.size(); ++i) {
            if (x[i] < minval) {
                minval = x[i];
                indmin = i;
            }
        }
        return { minval, indmin };
    }

    template <typename T>
    int argmin_fast(const std::vector<T>& x) {
        return findmin_fast(x).second;
    }

    template <typename T>
    int poisson_sample(T lambda) {
        int k = 0;
        T p = 1;
        T L = std::exp(-lambda);
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<T> dist(0, 1);
        while (p > L) {
            k++;
            p *= dist(gen);
        }
        return k - 1;
    }
}