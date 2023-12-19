#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <catch2/matchers/catch_matchers.hpp>

#include "turing_forge/core/individual.hpp"


TEST_CASE( "Random Individual", "[core]" ) {
    Turingforge::Individual expression = Turingforge::Individual(1, 2, 3);
    REQUIRE(expression.functions.size() == 1);
    REQUIRE_THAT(expression.coefficients, Catch::Matchers::Equals(std::vector<double>(expression.coefficients.size(), 1)));
    for (const auto& innerVector : expression.polynomials) {
        REQUIRE(std::all_of(innerVector.begin(), innerVector.end(), [](int x) { return x >= -3 && x <= 3; }));
    }
}