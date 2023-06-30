#pragma once

#include <vector>
#include <string>
#include <any>
#include <type_traits>

#include "Constants.h"

template <typename T, typename L, typename AX, typename AY = std::optional<std::vector<T>>, typename AW = std::optional<std::vector<T>>, typename NT = std::tuple<>>
struct Dataset {
    AX X;
    AY y;
    int n;
    int nfeatures;
    bool weighted;
    AW weights;
    NT extra;
    std::optional<T> avg_y;
    bool use_baseline;
    L baseline_loss;
    std::vector<std::string> varMap;

    Dataset(AX X_, AY y_ = std::nullopt, AW weights_ = std::nullopt, NT extra_ = NT(), L loss_type_ = L()) :
            X(X_), y(y_), n(X_.shape()[BATCH_DIM]), nfeatures(X_.shape()[FEATURE_DIM]), weighted(weights_.has_value()), weights(weights_), extra(extra_), avg_y(std::nullopt), use_baseline(true), baseline_loss(T(1)), varMap(nfeatures) {
        if (y.has_value()) {
            avg_y = weighted ? (y.value() * weights.value()).sum() / weights.value().sum() : y.value().mean();
        }
        if (varMap.empty()) {
            for (int i = 0; i < nfeatures; ++i) {
                varMap[i] = "x" + std::to_string(i + 1);
            }
        }
        baseline_loss = loss_type_.has_value() ? loss_type_ : T();
    }
};