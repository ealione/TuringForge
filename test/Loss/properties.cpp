#include <catch2/catch_test_macros.hpp>

#include "turingforge/Loss/LossFunctions.h"

struct TstVanillaLoss : SupervisedLoss {};

TEST_CASE("Fallback implementations; not prior knowledge") {
    SECTION("isminimizable") {
        REQUIRE(isminimizable(TstVanillaLoss()) == false);
    }
    SECTION("isdifferentiable") {
        REQUIRE(isdifferentiable(TstVanillaLoss()) == false);
        REQUIRE(isdifferentiable(TstVanillaLoss(), 0) == false);
    }
    SECTION("istwicedifferentiable") {
        REQUIRE(istwicedifferentiable(TstVanillaLoss()) == false);
        REQUIRE(istwicedifferentiable(TstVanillaLoss(), 0) == false);
    }
    SECTION("isstronglyconvex") {
        REQUIRE(isstronglyconvex(TstVanillaLoss()) == false);
    }
    SECTION("isstrictlyconvex") {
        REQUIRE(isstrictlyconvex(TstVanillaLoss()) == false);
    }
    SECTION("isconvex") {
        REQUIRE(isconvex(TstVanillaLoss()) == false);
    }
    SECTION("isnemitski") {
        REQUIRE(isnemitski(TstVanillaLoss()) == false);
    }
    SECTION("islipschitzcont") {
        REQUIRE(islipschitzcont(TstVanillaLoss()) == false);
    }
    SECTION("islocallylipschitzcont") {
        REQUIRE(islocallylipschitzcont(TstVanillaLoss()) == false);
    }
    SECTION("isclipable") {
        REQUIRE(isclipable(TstVanillaLoss()) == false);
    }
    SECTION("ismarginbased") {
        REQUIRE(ismarginbased(TstVanillaLoss()) == false);
    }

    SECTION("isdistancebased") {
        REQUIRE(isdistancebased(TstVanillaLoss()) == false);
    }
    SECTION("issymmetric") {
        REQUIRE(issymmetric(TstVanillaLoss()) == false);
    }
}

struct TstStronglyConvexLoss : SupervisedLoss {};

constexpr bool isstronglyconvex(const TstStronglyConvexLoss&) {
    return true;
}

TEST_CASE("Fallback implementations; strongly convex") {
    SECTION("isminimizable") {
        REQUIRE(isminimizable(TstStronglyConvexLoss()) == true);
    }
    SECTION("isdifferentiable") {
        REQUIRE(isdifferentiable(TstStronglyConvexLoss()) == false);
        REQUIRE(isdifferentiable(TstStronglyConvexLoss(), 0) == false);
    }
    SECTION("istwicedifferentiable") {
        REQUIRE(istwicedifferentiable(TstStronglyConvexLoss()) == false);
        REQUIRE(istwicedifferentiable(TstStronglyConvexLoss(), 0) == false);
    }
    SECTION("isstronglyconvex") {
        REQUIRE(isstronglyconvex(TstStronglyConvexLoss()) == true);
    }
    SECTION("isstrictlyconvex") {
        REQUIRE(isstrictlyconvex(TstStronglyConvexLoss()) == true);
    }
    SECTION("isconvex") {
        REQUIRE(isconvex(TstStronglyConvexLoss()) == true);
    }
    SECTION("isnemitski") {
        REQUIRE(isnemitski(TstStronglyConvexLoss()) == true);
    }
    SECTION("islipschitzcont") {
        REQUIRE(islipschitzcont(TstStronglyConvexLoss()) == false);
    }
    SECTION("islocallylipschitzcont") {
        REQUIRE(islocallylipschitzcont(TstStronglyConvexLoss()) == true);
    }
    SECTION("isclipable") {
        REQUIRE(isclipable(TstStronglyConvexLoss()) == false);
    }
    SECTION("ismarginbased") {
        REQUIRE(ismarginbased(TstStronglyConvexLoss()) == false);
    }
    SECTION("isdistancebased") {
        REQUIRE(isdistancebased(TstStronglyConvexLoss()) == false);
    }
    SECTION("issymmetric") {
        REQUIRE(issymmetric(TstStronglyConvexLoss()) == false);
    }
}