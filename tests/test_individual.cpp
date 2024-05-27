#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <catch2/matchers/catch_matchers.hpp>

#include "turing_forge/core/individual.hpp"

// TODO: follow this [https://github.com/catchorg/Catch2/blob/devel/docs/test-cases-and-sections.md]
TEST_CASE( "Basic Individual" ) {
    Turingforge::Individual individual;
    individual.Coefficients = {1, 2, 3};
    individual.Functions = {Turingforge::Function(Turingforge::FunctionType::Cos),
                            Turingforge::Function(Turingforge::FunctionType::Sin),
                            Turingforge::Function(Turingforge::FunctionType::Cos)};
    individual.Polynomials = {{1, 2, 3},
                              {0, 0, 0},
                              {1, 2, 3}};

    SECTION("Simple Initialization") {
        REQUIRE(individual.Functions.size() == 3);
        for (const auto &innerVector: individual.Polynomials) {
            REQUIRE(std::all_of(innerVector.begin(), innerVector.end(), [](int x) { return x >= -3 && x <= 3; }));
        }
    }

    SECTION("Sanitization") {
        individual.sanitize();
        REQUIRE(individual.Coefficients.size() == 1);
        REQUIRE(individual.Functions.size() == 1);
        REQUIRE(individual.Polynomials.size() == 1);
        REQUIRE(individual.Functions[0].Type == Turingforge::FunctionType::Cos);
    }
}