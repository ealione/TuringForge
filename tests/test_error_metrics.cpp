#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>

#include "turing_forge/error_metrics/error_metrics.hpp"


// TODO: Select a different library and test the results (maybe Elki?).
TEST_CASE( "Error Metrics" ) {
    auto const n{100UL};

    std::vector<double> x(n);
    std::vector<double> y(n);
    std::vector<double> z(n);

    Turingforge::RandomGenerator rng{1234}; // NOLINT
    std::uniform_real_distribution<double> ureal(0, 1);

    for (auto i = 0UL; i < n; ++i) {
        x[i] = ureal(rng);
        y[i] = ureal(rng);
        z[i] = ureal(rng);
    }

    auto constexpr eps{1e-6};

    using std::cbegin;
    using std::cend;

    SECTION("mean") {
        // auto m1 = ...;
        auto m2 = vstat::univariate::accumulate<double>(cbegin(x), cend(x)).mean;
        // CHECK(std::abs(m1-m2) < eps);
    }

    SECTION("weighted mean") {
        // auto m1 = ...;
        auto m2 = vstat::univariate::accumulate<double>(cbegin(x), cend(x), cbegin(z)).mean;
        // CHECK(std::abs(m1-m2) < eps);
    }

    SECTION("variance") {
        // auto m1 = ...;
        auto v2 = vstat::univariate::accumulate<double>(cbegin(x), cend(x)).variance;
        // CHECK(std::abs(v1-v2) < eps);
    }

    SECTION("weighted variance") {
        // auto m1 = ...;
        auto v2 = vstat::univariate::accumulate<double>(cbegin(x), cend(x), cbegin(z)).variance;
        // fmt::print("v1: {}\nv2: {}\n", v1, v2);
        // CHECK(std::abs(v1-v2) < eps);
    }

    SECTION("mse") {
        // auto m1 = ...;
        auto mse2 = Turingforge::MeanSquaredError(cbegin(x), cend(x), cbegin(y));
        // CHECK(std::abs(mse1-mse2) < eps);
    }

    SECTION("weighted mse") {
        // auto m1 = ...;
        auto mse2 = Turingforge::MeanSquaredError(cbegin(x), cend(x), cbegin(y), cbegin(z));
        // CHECK(std::abs(mse1-mse2) < eps);
    }

    SECTION("mae") {
        // auto m1 = ...;
        auto mae2 = Turingforge::MeanAbsoluteError(cbegin(x), cend(x), cbegin(y));
        // CHECK(std::abs(mae1-mae2) < eps);
    }

    SECTION("weighted mae") {
        // auto m1 = ...;
        auto mae2 = Turingforge::MeanAbsoluteError(cbegin(x), cend(x), cbegin(y), cbegin(z));
        // CHECK(std::abs(mae1-mae2) < eps);
    }

    SECTION("nmse") {
        // auto m1 = ...;
        auto nmse2 = Turingforge::NormalizedMeanSquaredError(cbegin(x), cend(x), cbegin(y));
        // CHECK(std::abs(nmse1-nmse2) < eps);
    }

    SECTION("weighted nmse") {
        // auto m1 = ...;
        auto nmse2 = Turingforge::NormalizedMeanSquaredError(cbegin(x), cend(x), cbegin(y), cbegin(z));
        // CHECK(std::abs(nmse1-nmse2) < eps);
    }

    SECTION("correlation") {
        // auto m1 = ...;
        auto c2 = Turingforge::CorrelationCoefficient(cbegin(x), cend(x), cbegin(y));
        // CHECK(std::abs(c1-c2) < eps);
    }

    SECTION("weighted correlation") {
        // auto m1 = ...;
        auto c2 = Turingforge::CorrelationCoefficient(cbegin(x), cend(x), cbegin(y), cbegin(z));
        // CHECK(std::abs(c1-c2) < eps);
    }

    SECTION("r2 score") {
        // auto m1 = ...;
        auto r2 = Turingforge::R2Score(cbegin(x), cend(x), cbegin(y));
        // CHECK(std::abs(r1-r2) < eps);
    }

    SECTION("weighted r2 score") {
        // auto m1 = ...;
        auto r2 = Turingforge::R2Score(cbegin(x), cend(x), cbegin(y), cbegin(z));
        // CHECK(std::abs(r1-r2) < eps);
    }
}