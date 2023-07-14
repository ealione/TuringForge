#include <catch2/catch_test_macros.hpp>

#include "turingforge/Loss/LossFunctions.h"

#define STRUCT_NAME_TO_STRING(struct_type) #struct_type

struct TstVanillaLoss : SupervisedLoss {};

TEST_CASE("Fallback implementations; not prior knowledge") {
    SECTION("isminimizable") {
        REQUIRE(TstVanillaLoss().isminimizable() == false);
    }
    SECTION("isdifferentiable") {
        REQUIRE(TstVanillaLoss().isdifferentiable() == false);
        REQUIRE(TstVanillaLoss().isdifferentiable(0) == false);
    }
    SECTION("istwicedifferentiable") {
        REQUIRE(TstVanillaLoss().istwicedifferentiable() == false);
        REQUIRE(TstVanillaLoss().istwicedifferentiable(0) == false);
    }
    SECTION("isstronglyconvex") {
        REQUIRE(TstVanillaLoss().isstronglyconvex() == false);
    }
    SECTION("isstrictlyconvex") {
        REQUIRE(TstVanillaLoss().isstrictlyconvex() == false);
    }
    SECTION("isconvex") {
        REQUIRE(TstVanillaLoss().isconvex() == false);
    }
    SECTION("isnemitski") {
        REQUIRE(TstVanillaLoss().isnemitski() == false);
    }
    SECTION("islipschitzcont") {
        REQUIRE(TstVanillaLoss().islipschitzcont() == false);
    }
    SECTION("islocallylipschitzcont") {
        REQUIRE(TstVanillaLoss().islocallylipschitzcont() == false);
    }
    SECTION("isclipable") {
        REQUIRE(TstVanillaLoss().isclipable() == false);
    }
    SECTION("ismarginbased") {
        REQUIRE(TstVanillaLoss().ismarginbased() == false);
    }

    SECTION("isdistancebased") {
        REQUIRE(TstVanillaLoss().isdistancebased() == false);
    }
    SECTION("issymmetric") {
        REQUIRE(TstVanillaLoss().issymmetric() == false);
    }
}

struct TstStronglyConvexLoss : SupervisedLoss {
    constexpr bool isstronglyconvex() override {
        return true;
    }
};

TEST_CASE("Fallback implementations; strongly convex") {
    SECTION("isminimizable") {
        REQUIRE(TstStronglyConvexLoss().isminimizable() == true);
    }
    SECTION("isdifferentiable") {
        REQUIRE(TstStronglyConvexLoss().isdifferentiable() == false);
        REQUIRE(TstStronglyConvexLoss().isdifferentiable(0) == false);
    }
    SECTION("istwicedifferentiable") {
        REQUIRE(TstStronglyConvexLoss().istwicedifferentiable() == false);
        REQUIRE(TstStronglyConvexLoss().istwicedifferentiable(0) == false);
    }
    SECTION("isstronglyconvex") {
        REQUIRE(TstStronglyConvexLoss().isstronglyconvex() == true);
    }
    SECTION("isstrictlyconvex") {
        REQUIRE(TstStronglyConvexLoss().isstrictlyconvex() == true);
    }
    SECTION("isconvex") {
        REQUIRE(TstStronglyConvexLoss().isconvex() == true);
    }
    SECTION("isnemitski") {
        REQUIRE(TstStronglyConvexLoss().isnemitski() == true);
    }
    SECTION("islipschitzcont") {
        REQUIRE(TstStronglyConvexLoss().islipschitzcont() == false);
    }
    SECTION("islocallylipschitzcont") {
        REQUIRE(TstStronglyConvexLoss().islocallylipschitzcont() == true);
    }
    SECTION("isclipable") {
        REQUIRE(TstStronglyConvexLoss().isclipable() == false);
    }
    SECTION("ismarginbased") {
        REQUIRE(TstStronglyConvexLoss().ismarginbased() == false);
    }
    SECTION("isdistancebased") {
        REQUIRE(TstStronglyConvexLoss().isdistancebased() == false);
    }
    SECTION("issymmetric") {
        REQUIRE(TstStronglyConvexLoss().issymmetric() == false);
    }
}

struct TstTwiceDiffLoss : SupervisedLoss {
    constexpr bool istwicedifferentiable() override {
        return true;
    }
    constexpr bool istwicedifferentiable(double) override {
        return istwicedifferentiable();
    }
};

TEST_CASE("Fallback implementations; twice differentiable") {
    SECTION("isminimizable") {
        REQUIRE(TstTwiceDiffLoss().isminimizable() == false);
    }
    SECTION("isdifferentiable") {
        REQUIRE(TstTwiceDiffLoss().isdifferentiable() == true);
        REQUIRE(TstTwiceDiffLoss().isdifferentiable(0) == true);
    }
    SECTION("istwicedifferentiable") {
        REQUIRE(TstTwiceDiffLoss().istwicedifferentiable() == true);
        REQUIRE(TstTwiceDiffLoss().istwicedifferentiable(0.0) == true);
    }
    SECTION("isstronglyconvex") {
        REQUIRE(TstTwiceDiffLoss().isstronglyconvex() == false);
    }
    SECTION("isstrictlyconvex") {
        REQUIRE(TstTwiceDiffLoss().isstrictlyconvex() == false);
    }
    SECTION("isconvex") {
        REQUIRE(TstTwiceDiffLoss().isconvex() == false);
    }
    SECTION("isnemitski") {
        REQUIRE(TstTwiceDiffLoss().isnemitski() == false);
    }
    SECTION("islipschitzcont") {
        REQUIRE(TstTwiceDiffLoss().islipschitzcont() == false);
    }
    SECTION("islocallylipschitzcont") {
        REQUIRE(TstTwiceDiffLoss().islocallylipschitzcont() == false);
    }
    SECTION("isclipable") {
        REQUIRE(TstTwiceDiffLoss().isclipable() == false);
    }
    SECTION("ismarginbased") {
        REQUIRE(TstTwiceDiffLoss().ismarginbased() == false);
    }
    SECTION("isdistancebased") {
        REQUIRE(TstTwiceDiffLoss().isdistancebased() == false);
    }
    SECTION("issymmetric") {
        REQUIRE(TstTwiceDiffLoss().issymmetric() == false);
    }
}

struct TstMarginLoss : public MarginLoss {};

TEST_CASE("Fallback implementations; margin-based") {
    TstMarginLoss loss;

    SECTION("isminimizable") {
        REQUIRE(loss.isminimizable() == false);
    }
    SECTION("isdifferentiable") {
        REQUIRE(loss.isdifferentiable() == false);
        REQUIRE(loss.isdifferentiable(0) == false);
    }
    SECTION("istwicedifferentiable") {
        REQUIRE(loss.istwicedifferentiable() == false);
        REQUIRE(loss.istwicedifferentiable(0.0) == false);
    }
    SECTION("isstronglyconvex") {
        REQUIRE(loss.isstronglyconvex() == false);
    }
    SECTION("isstrictlyconvex") {
        REQUIRE(loss.isstrictlyconvex() == false);
    }
    SECTION("isconvex") {
        REQUIRE(loss.isconvex() == false);
    }
    SECTION("isnemitski") {
        REQUIRE(loss.isnemitski() == true);
    }
    SECTION("islipschitzcont") {
        REQUIRE(loss.islipschitzcont() == false);
    }
    SECTION("islocallylipschitzcont") {
        REQUIRE(loss.islocallylipschitzcont() == false);
    }
    SECTION("isclipable") {
        REQUIRE(loss.isclipable() == false);
    }
    SECTION("ismarginbased") {
        REQUIRE(loss.ismarginbased() == true);
    }
    SECTION("isdistancebased") {
        REQUIRE(loss.isdistancebased() == false);
    }
    SECTION("issymmetric") {
        REQUIRE(loss.issymmetric() == false);
    }
    SECTION("isclasscalibrated") {
        REQUIRE(loss.isclasscalibrated() == false);
    }
    SECTION("isfishercons") {
        REQUIRE(loss.isfishercons() == false);
    }
    SECTION("isunivfishercons") {
        REQUIRE(loss.isunivfishercons() == false);
    }
}

TEST_CASE("Fallback implementations; distance-based") {
    struct TstDistanceLoss : public DistanceLoss {};
    TstDistanceLoss loss;

    SECTION("isminimizable") {
        REQUIRE(loss.isminimizable() == false);
    }
    SECTION("isdifferentiable") {
        REQUIRE(loss.isdifferentiable() == false);
        REQUIRE(loss.isdifferentiable(0) == false);
    }
    SECTION("istwicedifferentiable") {
        REQUIRE(loss.istwicedifferentiable() == false);
        REQUIRE(loss.istwicedifferentiable(0.0) == false);
    }
    SECTION("isstronglyconvex") {
        REQUIRE(loss.isstronglyconvex() == false);
    }
    SECTION("isstrictlyconvex") {
        REQUIRE(loss.isstrictlyconvex() == false);
    }
    SECTION("isconvex") {
        REQUIRE(loss.isconvex() == false);
    }
    SECTION("isnemitski") {
        REQUIRE(loss.isnemitski() == false);
    }
    SECTION("islipschitzcont") {
        REQUIRE(loss.islipschitzcont() == false);
    }
    SECTION("islocallylipschitzcont") {
        REQUIRE(loss.islocallylipschitzcont() == false);
    }
    SECTION("isclipable") {
        REQUIRE(loss.isclipable() == true);
    }
    SECTION("ismarginbased") {
        REQUIRE(loss.ismarginbased() == false);
    }
    SECTION("isdistancebased") {
        REQUIRE(loss.isdistancebased() == true);
    }
    SECTION("issymmetric") {
        REQUIRE(loss.issymmetric() == false);
    }
}

TEST_CASE("LPDistLoss{0.5}") {
    LPDistLoss loss(0.5);

    SECTION("isminimizable") {
        REQUIRE(loss.isminimizable() == true);
    }
    SECTION("isdifferentiable") {
        REQUIRE(loss.isdifferentiable() == false);
        REQUIRE(loss.isdifferentiable(0) == false);
        REQUIRE(loss.isdifferentiable(1) == true);
    }
    SECTION("istwicedifferentiable") {
        REQUIRE(loss.istwicedifferentiable() == false);
        REQUIRE(loss.istwicedifferentiable(1) == true);
    }
    SECTION("isstronglyconvex") {
        REQUIRE(loss.isstronglyconvex() == false);
    }
    SECTION("isstrictlyconvex") {
        REQUIRE(loss.isstrictlyconvex() == false);
    }
    SECTION("isconvex") {
        REQUIRE(loss.isconvex() == false);
    }
    SECTION("isnemitski") {
        REQUIRE(loss.isnemitski() == false);
    }
    SECTION("islipschitzcont") {
        REQUIRE(loss.islipschitzcont() == false);
    }
    SECTION("islocallylipschitzcont") {
        REQUIRE(loss.islocallylipschitzcont() == false);
    }
    SECTION("isclipable") {
        REQUIRE(loss.isclipable() == true);
    }
    SECTION("ismarginbased") {
        REQUIRE(loss.ismarginbased() == false);
    }
    SECTION("isdistancebased") {
        REQUIRE(loss.isdistancebased() == true);
    }
    SECTION("issymmetric") {
        REQUIRE(loss.issymmetric() == true);
    }
}

TEST_CASE("L1DistLoss") {
    L1DistLoss loss;

    SECTION("isminimizable") {
        REQUIRE(loss.isminimizable() == true);
    }
    SECTION("isdifferentiable") {
        REQUIRE(loss.isdifferentiable() == false);
        REQUIRE(loss.isdifferentiable(0) == false);
        REQUIRE(loss.isdifferentiable(1) == true);
    }
    SECTION("istwicedifferentiable") {
        REQUIRE(loss.istwicedifferentiable() == false);
        REQUIRE(loss.istwicedifferentiable(1) == true);
    }
    SECTION("isstronglyconvex") {
        REQUIRE(loss.isstronglyconvex() == false);
    }
    SECTION("isstrictlyconvex") {
        REQUIRE(loss.isstrictlyconvex() == false);
    }
    SECTION("isconvex") {
        REQUIRE(loss.isconvex() == true);
    }
    SECTION("isnemitski") {
        REQUIRE(loss.isnemitski() == true);
    }
    SECTION("islipschitzcont") {
        REQUIRE(loss.islipschitzcont() == true);
    }
    SECTION("islocallylipschitzcont") {
        REQUIRE(loss.islocallylipschitzcont() == true);
    }
    SECTION("isclipable") {
        REQUIRE(loss.isclipable() == true);
    }
    SECTION("ismarginbased") {
        REQUIRE(loss.ismarginbased() == false);
    }
    SECTION("isdistancebased") {
        REQUIRE(loss.isdistancebased() == true);
    }
    SECTION("issymmetric") {
        REQUIRE(loss.issymmetric() == true);
    }
}

TEST_CASE("L2DistLoss") {
    L2DistLoss loss;

    SECTION("isminimizable") {
        REQUIRE(loss.isminimizable() == true);
    }
    SECTION("isdifferentiable") {
        REQUIRE(loss.isdifferentiable() == true);
        REQUIRE(loss.isdifferentiable(0) == true);
    }
    SECTION("istwicedifferentiable") {
        REQUIRE(loss.istwicedifferentiable() == true);
        REQUIRE(loss.istwicedifferentiable(0) == true);
    }
    SECTION("isstronglyconvex") {
        REQUIRE(loss.isstronglyconvex() == true);
    }
    SECTION("isstrictlyconvex") {
        REQUIRE(loss.isstrictlyconvex() == true);
    }
    SECTION("isconvex") {
        REQUIRE(loss.isconvex() == true);
    }
    SECTION("isnemitski") {
        REQUIRE(loss.isnemitski() == true);
    }
    SECTION("islipschitzcont") {
        REQUIRE(loss.islipschitzcont() == false);
    }
    SECTION("islocallylipschitzcont") {
        REQUIRE(loss.islocallylipschitzcont() == true);
    }
    SECTION("isclipable") {
        REQUIRE(loss.isclipable() == true);
    }
    SECTION("ismarginbased") {
        REQUIRE(loss.ismarginbased() == false);
    }
    SECTION("isdistancebased") {
        REQUIRE(loss.isdistancebased() == true);
    }
    SECTION("issymmetric") {
        REQUIRE(loss.issymmetric() == true);
    }
}

TEST_CASE("LPDistLoss{3}") {
    LPDistLoss loss(3);

    SECTION("isminimizable") {
        REQUIRE(loss.isminimizable() == true);
    }
    SECTION("isdifferentiable") {
        REQUIRE(loss.isdifferentiable() == true);
        REQUIRE(loss.isdifferentiable(0) == true);
    }
    SECTION("istwicedifferentiable") {
        REQUIRE(loss.istwicedifferentiable() == true);
        REQUIRE(loss.istwicedifferentiable(0) == true);
    }
    SECTION("isstronglyconvex") {
        REQUIRE(loss.isstronglyconvex() == true);
    }
    SECTION("isstrictlyconvex") {
        REQUIRE(loss.isstrictlyconvex() == true);
    }
    SECTION("isconvex") {
        REQUIRE(loss.isconvex() == true);
    }
    SECTION("isnemitski") {
        REQUIRE(loss.isnemitski() == true);
    }
    SECTION("islipschitzcont") {
        REQUIRE(loss.islipschitzcont() == false);
    }
    SECTION("islocallylipschitzcont") {
        REQUIRE(loss.islocallylipschitzcont() == true);
    }
    SECTION("isclipable") {
        REQUIRE(loss.isclipable() == true);
    }
    SECTION("ismarginbased") {
        REQUIRE(loss.ismarginbased() == false);
    }
    SECTION("isdistancebased") {
        REQUIRE(loss.isdistancebased() == true);
    }
    SECTION("issymmetric") {
        REQUIRE(loss.issymmetric() == true);
    }
}

TEST_CASE("HuberLoss(1)") {
    HuberLoss loss(1);

    SECTION("isminimizable") {
        REQUIRE(loss.isminimizable() == true);
    }
    SECTION("isdifferentiable") {
        REQUIRE(loss.isdifferentiable() == true);
        REQUIRE(loss.isdifferentiable(0) == true);
        REQUIRE(loss.isdifferentiable(1) == true);
    }
    SECTION("istwicedifferentiable") {
        REQUIRE(loss.istwicedifferentiable() == false);
        REQUIRE(loss.istwicedifferentiable(0) == true);
        REQUIRE(loss.istwicedifferentiable(1) == false);
    }
    SECTION("isstronglyconvex") {
        REQUIRE(loss.isstronglyconvex() == false);
    }
    SECTION("isstrictlyconvex") {
        REQUIRE(loss.isstrictlyconvex() == false);
    }
    SECTION("isconvex") {
        REQUIRE(loss.isconvex() == true);
    }
    SECTION("isnemitski") {
        REQUIRE(loss.isnemitski() == true);
    }
    SECTION("islipschitzcont") {
        REQUIRE(loss.islipschitzcont() == true);
    }
    SECTION("islocallylipschitzcont") {
        REQUIRE(loss.islocallylipschitzcont() == true);
    }
    SECTION("isclipable") {
        REQUIRE(loss.isclipable() == true);
    }
    SECTION("ismarginbased") {
        REQUIRE(loss.ismarginbased() == false);
    }
    SECTION("isdistancebased") {
        REQUIRE(loss.isdistancebased() == true);
    }
    SECTION("issymmetric") {
        REQUIRE(loss.issymmetric() == true);
    }
}

TEST_CASE("L1EpsilonInsLoss(1)") {
    L1EpsilonInsLoss loss(1);

    SECTION("isminimizable") {
        REQUIRE(loss.isminimizable() == true);
    }
    SECTION("isdifferentiable") {
        REQUIRE(loss.isdifferentiable() == false);
        REQUIRE(loss.isdifferentiable(0) == true);
        REQUIRE(loss.isdifferentiable(1) == false);
    }
    SECTION("istwicedifferentiable") {
        REQUIRE(loss.istwicedifferentiable() == false);
        REQUIRE(loss.istwicedifferentiable(0) == true);
    }
    SECTION("isstronglyconvex") {
        REQUIRE(loss.isstronglyconvex() == false);
    }
    SECTION("isstrictlyconvex") {
        REQUIRE(loss.isstrictlyconvex() == false);
    }
    SECTION("isconvex") {
        REQUIRE(loss.isconvex() == true);
    }
    SECTION("isnemitski") {
        REQUIRE(loss.isnemitski() == true);
    }
    SECTION("islipschitzcont") {
        REQUIRE(loss.islipschitzcont() == true);
    }
    SECTION("islocallylipschitzcont") {
        REQUIRE(loss.islocallylipschitzcont() == true);
    }
    SECTION("isclipable") {
        REQUIRE(loss.isclipable() == true);
    }
    SECTION("ismarginbased") {
        REQUIRE(loss.ismarginbased() == false);
    }
    SECTION("isdistancebased") {
        REQUIRE(loss.isdistancebased() == true);
    }
    SECTION("issymmetric") {
        REQUIRE(loss.issymmetric() == true);
    }
}

TEST_CASE("L2EpsilonInsLoss(1)") {
    L2EpsilonInsLoss loss(1);

    SECTION("isminimizable") {
        REQUIRE(loss.isminimizable() == true);
    }
    SECTION("isdifferentiable") {
        REQUIRE(loss.isdifferentiable() == true);
        REQUIRE(loss.isdifferentiable(0) == true);
        REQUIRE(loss.isdifferentiable(1) == true);
    }
    SECTION("istwicedifferentiable") {
        REQUIRE(loss.istwicedifferentiable() == false);
        REQUIRE(loss.istwicedifferentiable(0) == true);
        REQUIRE(loss.istwicedifferentiable(1) == false);
    }
    SECTION("isstronglyconvex") {
        REQUIRE(loss.isstronglyconvex() == true);
    }
    SECTION("isstrictlyconvex") {
        REQUIRE(loss.isstrictlyconvex() == true);
    }
    SECTION("isconvex") {
        REQUIRE(loss.isconvex() == true);
    }
    SECTION("isnemitski") {
        REQUIRE(loss.isnemitski() == true);
    }
    SECTION("islipschitzcont") {
        REQUIRE(loss.islipschitzcont() == false);
    }
    SECTION("islocallylipschitzcont") {
        REQUIRE(loss.islocallylipschitzcont() == true);
    }
    SECTION("isclipable") {
        REQUIRE(loss.isclipable() == true);
    }
    SECTION("ismarginbased") {
        REQUIRE(loss.ismarginbased() == false);
    }
    SECTION("isdistancebased") {
        REQUIRE(loss.isdistancebased() == true);
    }
    SECTION("issymmetric") {
        REQUIRE(loss.issymmetric() == true);
    }
}

TEST_CASE("LogitDistLoss()") {
    LogitDistLoss loss;

    SECTION("isminimizable") {
        REQUIRE(loss.isminimizable() == true);
    }
    SECTION("isdifferentiable") {
        REQUIRE(loss.isdifferentiable() == true);
        REQUIRE(loss.isdifferentiable(0) == true);
        REQUIRE(loss.isdifferentiable(1) == true);
    }
    SECTION("istwicedifferentiable") {
        REQUIRE(loss.istwicedifferentiable() == true);
        REQUIRE(loss.istwicedifferentiable(0) == true);
        REQUIRE(loss.istwicedifferentiable(1) == true);
    }
    SECTION("isstronglyconvex") {
        REQUIRE(loss.isstronglyconvex() == false);
    }
    SECTION("isstrictlyconvex") {
        REQUIRE(loss.isstrictlyconvex() == true);
    }
    SECTION("isconvex") {
        REQUIRE(loss.isconvex() == true);
    }
    SECTION("isnemitski") {
        REQUIRE(loss.isnemitski() == true);
    }
    SECTION("islipschitzcont") {
        REQUIRE(loss.islipschitzcont() == true);
    }
    SECTION("islocallylipschitzcont") {
        REQUIRE(loss.islocallylipschitzcont() == true);
    }
    SECTION("isclipable") {
        REQUIRE(loss.isclipable() == true);
    }
    SECTION("ismarginbased") {
        REQUIRE(loss.ismarginbased() == false);
    }
    SECTION("isdistancebased") {
        REQUIRE(loss.isdistancebased() == true);
    }
    SECTION("issymmetric") {
        REQUIRE(loss.issymmetric() == true);
    }
}

TEST_CASE("QuantileLoss") {
    QuantileLoss l1(0.5);
    QuantileLoss l2(0.7);

    SECTION("issymmetric") {
        REQUIRE(l1.issymmetric() == true);
        REQUIRE(l2.issymmetric() == false);
    }
    SECTION("isminimizable") {
        REQUIRE(l2.isminimizable() == true);
    }
    SECTION("isdifferentiable") {
        REQUIRE(l2.isdifferentiable() == false);
        REQUIRE(l2.isdifferentiable(0) == false);
        REQUIRE(l2.isdifferentiable(1) == true);
    }
    SECTION("istwicedifferentiable") {
        REQUIRE(l2.istwicedifferentiable() == false);
        REQUIRE(l2.istwicedifferentiable(0) == false);
        REQUIRE(l2.istwicedifferentiable(1) == true);
    }
    SECTION("isstronglyconvex") {
        REQUIRE(l2.isstronglyconvex() == false);
    }
    SECTION("isstrictlyconvex") {
        REQUIRE(l2.isstrictlyconvex() == false);
    }
    SECTION("isconvex") {
        REQUIRE(l2.isconvex() == true);
    }
    SECTION("islipschitzcont") {
        REQUIRE(l2.islipschitzcont() == true);
    }
    SECTION("islocallylipschitzcont") {
        REQUIRE(l2.islocallylipschitzcont() == true);
    }
    SECTION("ismarginbased") {
        REQUIRE(l2.ismarginbased() == false);
    }
    SECTION("isdistancebased") {
        REQUIRE(l2.isdistancebased() == true);
    }
}

TEST_CASE("LogCoshLoss") {
    LogCoshLoss loss;

    SECTION("isminimizable") {
        REQUIRE(loss.isminimizable() == true);
    }
    SECTION("isdifferentiable") {
        REQUIRE(loss.isdifferentiable() == true);
        REQUIRE(loss.isdifferentiable(0) == true);
    }
    SECTION("istwicedifferentiable") {
        REQUIRE(loss.istwicedifferentiable() == true);
        REQUIRE(loss.istwicedifferentiable(0) == true);
    }
    SECTION("isstronglyconvex") {
        REQUIRE(loss.isstronglyconvex() == true);
    }
    SECTION("isstrictlyconvex") {
        REQUIRE(loss.isstrictlyconvex() == true);
    }
    SECTION("isconvex") {
        REQUIRE(loss.isconvex() == true);
    }
    SECTION("islipschitzcont") {
        REQUIRE(loss.islipschitzcont() == true);
    }
    SECTION("islocallylipschitzcont") {
        REQUIRE(loss.islocallylipschitzcont() == true);
    }
    SECTION("isclipable") {
        REQUIRE(loss.isclipable() == true);
    }
    SECTION("ismarginbased") {
        REQUIRE(loss.ismarginbased() == false);
    }
    SECTION("isdistancebased") {
        REQUIRE(loss.isdistancebased() == true);
    }
    SECTION("issymmetric") {
        REQUIRE(loss.issymmetric() == true);
    }
}

TEST_CASE("ZeroOneLoss") {
    ZeroOneLoss loss;

    SECTION("isminimizable") {
        REQUIRE(loss.isminimizable() == true);
    }
    SECTION("isdifferentiable") {
        REQUIRE(loss.isdifferentiable() == false);
        REQUIRE(loss.isdifferentiable(0) == false);
        REQUIRE(loss.isdifferentiable(1) == true);
    }
    SECTION("istwicedifferentiable") {
        REQUIRE(loss.istwicedifferentiable() == false);
        REQUIRE(loss.istwicedifferentiable(1) == true);
    }
    SECTION("isstronglyconvex") {
        REQUIRE(loss.isstronglyconvex() == false);
    }
    SECTION("isstrictlyconvex") {
        REQUIRE(loss.isstrictlyconvex() == false);
    }
    SECTION("isconvex") {
        REQUIRE(loss.isconvex() == false);
    }
    SECTION("islipschitzcont") {
        REQUIRE(loss.islipschitzcont() == true);
    }
    SECTION("islocallylipschitzcont") {
        REQUIRE(loss.islocallylipschitzcont() == true);
    }
    SECTION("isclipable") {
        REQUIRE(loss.isclipable() == true);
    }
    SECTION("ismarginbased") {
        REQUIRE(loss.ismarginbased() == true);
    }
    SECTION("isdistancebased") {
        REQUIRE(loss.isdistancebased() == false);
    }
    SECTION("issymmetric") {
        REQUIRE(loss.issymmetric() == false);
    }
    SECTION("isclasscalibrated") {
        REQUIRE(loss.isclasscalibrated() == true);
    }
}

TEST_CASE("PerceptronLoss") {
    PerceptronLoss loss;

    SECTION("isminimizable") {
        REQUIRE(loss.isminimizable() == true);
    }
    SECTION("isdifferentiable") {
        REQUIRE(loss.isdifferentiable() == false);
        REQUIRE(loss.isdifferentiable(0) == false);
        REQUIRE(loss.isdifferentiable(1) == true);
    }
    SECTION("istwicedifferentiable") {
        REQUIRE(loss.istwicedifferentiable() == false);
        REQUIRE(loss.istwicedifferentiable(0) == false);
        REQUIRE(loss.istwicedifferentiable(1) == true);
    }
    SECTION("isstronglyconvex") {
        REQUIRE(loss.isstronglyconvex() == false);
    }
    SECTION("isstrictlyconvex") {
        REQUIRE(loss.isstrictlyconvex() == false);
    }
    SECTION("isconvex") {
        REQUIRE(loss.isconvex() == true);
    }
    SECTION("islipschitzcont") {
        REQUIRE(loss.islipschitzcont() == true);
    }
    SECTION("islocallylipschitzcont") {
        REQUIRE(loss.islocallylipschitzcont() == true);
    }
    SECTION("isclipable") {
        REQUIRE(loss.isclipable() == true);
    }
    SECTION("ismarginbased") {
        REQUIRE(loss.ismarginbased() == true);
    }
    SECTION("isdistancebased") {
        REQUIRE(loss.isdistancebased() == false);
    }
    SECTION("issymmetric") {
        REQUIRE(loss.issymmetric() == false);
    }
    SECTION("isclasscalibrated") {
        REQUIRE(loss.isclasscalibrated() == false);
    }
}

TEST_CASE("LogitMarginLoss") {
    LogitMarginLoss loss;

    SECTION("isminimizable") {
        REQUIRE(loss.isminimizable() == true);
    }
    SECTION("isdifferentiable") {
        REQUIRE(loss.isdifferentiable() == true);
        REQUIRE(loss.isdifferentiable(0) == true);
        REQUIRE(loss.isdifferentiable(1) == true);
    }
    SECTION("istwicedifferentiable") {
        REQUIRE(loss.istwicedifferentiable() == true);
        REQUIRE(loss.istwicedifferentiable(0) == true);
        REQUIRE(loss.istwicedifferentiable(1) == true);
    }
    SECTION("isstronglyconvex") {
        REQUIRE(loss.isstronglyconvex() == false);
    }
    SECTION("isstrictlyconvex") {
        REQUIRE(loss.isstrictlyconvex() == true);
    }
    SECTION("isconvex") {
        REQUIRE(loss.isconvex() == true);
    }
    SECTION("islipschitzcont") {
        REQUIRE(loss.islipschitzcont() == true);
    }
    SECTION("islocallylipschitzcont") {
        REQUIRE(loss.islocallylipschitzcont() == true);
    }
    SECTION("isclipable") {
        REQUIRE(loss.isclipable() == false);
    }
    SECTION("ismarginbased") {
        REQUIRE(loss.ismarginbased() == true);
    }
    SECTION("isdistancebased") {
        REQUIRE(loss.isdistancebased() == false);
    }
    SECTION("issymmetric") {
        REQUIRE(loss.issymmetric() == false);
    }
    SECTION("isclasscalibrated") {
        REQUIRE(loss.isclasscalibrated() == true);
    }
    SECTION("isfishercons") {
        REQUIRE(loss.isfishercons() == true);
    }
    SECTION("isunivfishercons") {
        REQUIRE(loss.isunivfishercons() == true);
    }
}

TEST_CASE("L1HingeLoss") {
    L1HingeLoss loss;

    SECTION("isminimizable") {
        REQUIRE(loss.isminimizable() == true);
    }
    SECTION("isdifferentiable") {
        REQUIRE(loss.isdifferentiable() == false);
        REQUIRE(loss.isdifferentiable(0) == true);
        REQUIRE(loss.isdifferentiable(1) == false);
    }
    SECTION("istwicedifferentiable") {
        REQUIRE(loss.istwicedifferentiable() == false);
        REQUIRE(loss.istwicedifferentiable(0) == true);
        REQUIRE(loss.istwicedifferentiable(1) == false);
    }
    SECTION("isstronglyconvex") {
        REQUIRE(loss.isstronglyconvex() == false);
    }
    SECTION("isstrictlyconvex") {
        REQUIRE(loss.isstrictlyconvex() == false);
    }
    SECTION("isconvex") {
        REQUIRE(loss.isconvex() == true);
    }
    SECTION("islipschitzcont") {
        REQUIRE(loss.islipschitzcont() == true);
    }
    SECTION("islocallylipschitzcont") {
        REQUIRE(loss.islocallylipschitzcont() == true);
    }
    SECTION("isclipable") {
        REQUIRE(loss.isclipable() == true);
    }
    SECTION("ismarginbased") {
        REQUIRE(loss.ismarginbased() == true);
    }
    SECTION("isdistancebased") {
        REQUIRE(loss.isdistancebased() == false);
    }
    SECTION("issymmetric") {
        REQUIRE(loss.issymmetric() == false);
    }
    SECTION("isclasscalibrated") {
        REQUIRE(loss.isclasscalibrated() == true);
    }
    SECTION("isfishercons") {
        REQUIRE(loss.isfishercons() == true);
    }
    SECTION("isunivfishercons") {
        REQUIRE(loss.isunivfishercons() == false);
    }
}

TEST_CASE("L2HingeLoss") {
    L2HingeLoss loss;

    SECTION("isminimizable") {
        REQUIRE(loss.isminimizable() == true);
    }
    SECTION("isdifferentiable") {
        REQUIRE(loss.isdifferentiable() == true);
        REQUIRE(loss.isdifferentiable(0) == true);
        REQUIRE(loss.isdifferentiable(1) == true);
    }
    SECTION("istwicedifferentiable") {
        REQUIRE(loss.istwicedifferentiable() == false);
        REQUIRE(loss.istwicedifferentiable(0) == true);
        REQUIRE(loss.istwicedifferentiable(1) == false);
    }
    SECTION("isstronglyconvex") {
        REQUIRE(loss.isstronglyconvex() == false);
    }
    SECTION("isstrictlyconvex") {
        REQUIRE(loss.isstrictlyconvex() == false);
    }
    SECTION("isconvex") {
        REQUIRE(loss.isconvex() == true);
    }
    SECTION("islipschitzcont") {
        REQUIRE(loss.islipschitzcont() == false);
    }
    SECTION("islocallylipschitzcont") {
        REQUIRE(loss.islocallylipschitzcont() == true);
    }
    SECTION("isclipable") {
        REQUIRE(loss.isclipable() == true);
    }
    SECTION("ismarginbased") {
        REQUIRE(loss.ismarginbased() == true);
    }
    SECTION("isdistancebased") {
        REQUIRE(loss.isdistancebased() == false);
    }
    SECTION("issymmetric") {
        REQUIRE(loss.issymmetric() == false);
    }
    SECTION("isclasscalibrated") {
        REQUIRE(loss.isclasscalibrated() == true);
    }
    SECTION("isfishercons") {
        REQUIRE(loss.isfishercons() == true);
    }
    SECTION("isunivfishercons") {
        REQUIRE(loss.isunivfishercons() == true);
    }
}

TEST_CASE("SmoothedL1HingeLoss") {
    SmoothedL1HingeLoss loss(2);

    SECTION("isminimizable") {
        REQUIRE(loss.isminimizable() == true);
    }
    SECTION("isdifferentiable") {
        REQUIRE(loss.isdifferentiable() == true);
        REQUIRE(loss.isdifferentiable(0) == true);
        REQUIRE(loss.isdifferentiable(1) == true);
    }
    SECTION("istwicedifferentiable") {
        REQUIRE(loss.istwicedifferentiable() == false);
        REQUIRE(loss.istwicedifferentiable(-1) == false);
        REQUIRE(loss.istwicedifferentiable(0) == true);
        REQUIRE(loss.istwicedifferentiable(1) == false);
        REQUIRE(loss.istwicedifferentiable(2) == true);
    }
    SECTION("isstronglyconvex") {
        REQUIRE(loss.isstronglyconvex() == false);
    }
    SECTION("isstrictlyconvex") {
        REQUIRE(loss.isstrictlyconvex() == false);
    }
    SECTION("isconvex") {
        REQUIRE(loss.isconvex() == true);
    }
    SECTION("islipschitzcont") {
        REQUIRE(loss.islipschitzcont() == true);
    }
    SECTION("islocallylipschitzcont") {
        REQUIRE(loss.islocallylipschitzcont() == true);
    }
    SECTION("isclipable") {
        REQUIRE(loss.isclipable() == true);
    }
    SECTION("ismarginbased") {
        REQUIRE(loss.ismarginbased() == true);
    }
    SECTION("isdistancebased") {
        REQUIRE(loss.isdistancebased() == false);
    }
    SECTION("issymmetric") {
        REQUIRE(loss.issymmetric() == false);
    }
    SECTION("isclasscalibrated") {
        REQUIRE(loss.isclasscalibrated() == true);
    }
}

TEST_CASE("ModifiedHuberLoss") {
    ModifiedHuberLoss loss;

    SECTION("isminimizable") {
        REQUIRE(loss.isminimizable() == true);
    }
    SECTION("isdifferentiable") {
        REQUIRE(loss.isdifferentiable() == true);
        REQUIRE(loss.isdifferentiable(0) == true);
        REQUIRE(loss.isdifferentiable(1) == true);
    }
    SECTION("istwicedifferentiable") {
        REQUIRE(loss.istwicedifferentiable() == false);
        REQUIRE(loss.istwicedifferentiable(-1) == false);
        REQUIRE(loss.istwicedifferentiable(0) == true);
        REQUIRE(loss.istwicedifferentiable(1) == false);
        REQUIRE(loss.istwicedifferentiable(2) == true);
    }
    SECTION("isstronglyconvex") {
        REQUIRE(loss.isstronglyconvex() == false);
    }
    SECTION("isstrictlyconvex") {
        REQUIRE(loss.isstrictlyconvex() == false);
    }
    SECTION("isconvex") {
        REQUIRE(loss.isconvex() == true);
    }
    SECTION("islipschitzcont") {
        REQUIRE(loss.islipschitzcont() == true);
    }
    SECTION("islocallylipschitzcont") {
        REQUIRE(loss.islocallylipschitzcont() == true);
    }
    SECTION("isclipable") {
        REQUIRE(loss.isclipable() == true);
    }
    SECTION("ismarginbased") {
        REQUIRE(loss.ismarginbased() == true);
    }
    SECTION("isdistancebased") {
        REQUIRE(loss.isdistancebased() == false);
    }
    SECTION("issymmetric") {
        REQUIRE(loss.issymmetric() == false);
    }
    SECTION("isclasscalibrated") {
        REQUIRE(loss.isclasscalibrated() == true);
    }
}

TEST_CASE("L2MarginLoss") {
    L2MarginLoss loss;

    SECTION("isminimizable") {
        REQUIRE(loss.isminimizable() == true);
    }
    SECTION("isdifferentiable") {
        REQUIRE(loss.isdifferentiable() == true);
        REQUIRE(loss.isdifferentiable(0) == true);
        REQUIRE(loss.isdifferentiable(1) == true);
    }
    SECTION("istwicedifferentiable") {
        REQUIRE(loss.istwicedifferentiable() == true);
        REQUIRE(loss.istwicedifferentiable(0) == true);
        REQUIRE(loss.istwicedifferentiable(1) == true);
    }
    SECTION("isstronglyconvex") {
        REQUIRE(loss.isstronglyconvex() == true);
    }
    SECTION("isstrictlyconvex") {
        REQUIRE(loss.isstrictlyconvex() == true);
    }
    SECTION("isconvex") {
        REQUIRE(loss.isconvex() == true);
    }
    SECTION("islipschitzcont") {
        REQUIRE(loss.islipschitzcont() == false);
    }
    SECTION("islocallylipschitzcont") {
        REQUIRE(loss.islocallylipschitzcont() == true);
    }
    SECTION("isclipable") {
        REQUIRE(loss.isclipable() == true);
    }
    SECTION("ismarginbased") {
        REQUIRE(loss.ismarginbased() == true);
    }
    SECTION("isdistancebased") {
        REQUIRE(loss.isdistancebased() == false);
    }
    SECTION("issymmetric") {
        REQUIRE(loss.issymmetric() == false);
    }
    SECTION("isclasscalibrated") {
        REQUIRE(loss.isclasscalibrated() == true);
    }
    SECTION("isfishercons") {
        REQUIRE(loss.isfishercons() == true);
    }
    SECTION("isunivfishercons") {
        REQUIRE(loss.isunivfishercons() == true);
    }
}

TEST_CASE("ExpLoss") {
    ExpLoss loss;

    SECTION("isminimizable") {
        REQUIRE(loss.isminimizable() == true);
    }
    SECTION("isdifferentiable") {
        REQUIRE(loss.isdifferentiable() == true);
        REQUIRE(loss.isdifferentiable(0) == true);
        REQUIRE(loss.isdifferentiable(1) == true);
    }
    SECTION("istwicedifferentiable") {
        REQUIRE(loss.istwicedifferentiable() == true);
        REQUIRE(loss.istwicedifferentiable(0) == true);
        REQUIRE(loss.istwicedifferentiable(1) == true);
    }
    SECTION("isstronglyconvex") {
        REQUIRE(loss.isstronglyconvex() == false);
    }
    SECTION("isstrictlyconvex") {
        REQUIRE(loss.isstrictlyconvex() == true);
    }
    SECTION("isconvex") {
        REQUIRE(loss.isconvex() == true);
    }
    SECTION("islipschitzcont") {
        REQUIRE(loss.islipschitzcont() == false);
    }
    SECTION("islocallylipschitzcont") {
        REQUIRE(loss.islocallylipschitzcont() == true);
    }
    SECTION("isclipable") {
        REQUIRE(loss.isclipable() == false);
    }
    SECTION("ismarginbased") {
        REQUIRE(loss.ismarginbased() == true);
    }
    SECTION("isdistancebased") {
        REQUIRE(loss.isdistancebased() == false);
    }
    SECTION("issymmetric") {
        REQUIRE(loss.issymmetric() == false);
    }
    SECTION("isclasscalibrated") {
        REQUIRE(loss.isclasscalibrated() == true);
    }
    SECTION("isfishercons") {
        REQUIRE(loss.isfishercons() == true);
    }
    SECTION("isunivfishercons") {
        REQUIRE(loss.isunivfishercons() == true);
    }
}

TEST_CASE("SigmoidLoss") {
    SigmoidLoss loss;

    SECTION("isminimizable") {
        REQUIRE(loss.isminimizable() == false);
    }
    SECTION("isdifferentiable") {
        REQUIRE(loss.isdifferentiable() == true);
        REQUIRE(loss.isdifferentiable(0) == true);
        REQUIRE(loss.isdifferentiable(1) == true);
    }
    SECTION("istwicedifferentiable") {
        REQUIRE(loss.istwicedifferentiable() == true);
        REQUIRE(loss.istwicedifferentiable(0) == true);
        REQUIRE(loss.istwicedifferentiable(1) == true);
    }
    SECTION("isstronglyconvex") {
        REQUIRE(loss.isstronglyconvex() == false);
    }
    SECTION("isstrictlyconvex") {
        REQUIRE(loss.isstrictlyconvex() == false);
    }
    SECTION("isconvex") {
        REQUIRE(loss.isconvex() == false);
    }
    SECTION("islipschitzcont") {
        REQUIRE(loss.islipschitzcont() == true);
    }
    SECTION("islocallylipschitzcont") {
        REQUIRE(loss.islocallylipschitzcont() == true);
    }
    SECTION("isclipable") {
        REQUIRE(loss.isclipable() == false);
    }
    SECTION("ismarginbased") {
        REQUIRE(loss.ismarginbased() == true);
    }
    SECTION("isdistancebased") {
        REQUIRE(loss.isdistancebased() == false);
    }
    SECTION("issymmetric") {
        REQUIRE(loss.issymmetric() == false);
    }
    SECTION("isclasscalibrated") {
        REQUIRE(loss.isclasscalibrated() == true);
    }
    SECTION("isfishercons") {
        REQUIRE(loss.isfishercons() == true);
    }
    SECTION("isunivfishercons") {
        REQUIRE(loss.isunivfishercons() == true);
    }
}

TEST_CASE("DWDMarginLoss") {
    DWDMarginLoss loss(2);

    SECTION("isminimizable") {
        REQUIRE(loss.isminimizable() == true);
    }
    SECTION("isdifferentiable") {
        REQUIRE(loss.isdifferentiable() == true);
        REQUIRE(loss.isdifferentiable(0) == true);
        REQUIRE(loss.isdifferentiable(1) == true);
    }
    SECTION("istwicedifferentiable") {
        REQUIRE(loss.istwicedifferentiable() == true);
        REQUIRE(loss.istwicedifferentiable(0) == true);
        REQUIRE(loss.istwicedifferentiable(1) == true);
    }
    SECTION("isstronglyconvex") {
        REQUIRE(loss.isstronglyconvex() == false);
    }
    SECTION("isstrictlyconvex") {
        REQUIRE(loss.isstrictlyconvex() == false);
    }
    SECTION("isconvex") {
        REQUIRE(loss.isconvex() == true);
    }
    SECTION("islipschitzcont") {
        REQUIRE(loss.islipschitzcont() == true);
    }
    SECTION("islocallylipschitzcont") {
        REQUIRE(loss.islocallylipschitzcont() == true);
    }
    SECTION("isclipable") {
        REQUIRE(loss.isclipable() == false);
    }
    SECTION("ismarginbased") {
        REQUIRE(loss.ismarginbased() == true);
    }
    SECTION("isdistancebased") {
        REQUIRE(loss.isdistancebased() == false);
    }
    SECTION("issymmetric") {
        REQUIRE(loss.issymmetric() == false);
    }
    SECTION("isclasscalibrated") {
        REQUIRE(loss.isclasscalibrated() == true);
    }
    SECTION("isfishercons") {
        REQUIRE(loss.isfishercons() == true);
    }
    SECTION("isunivfishercons") {
        REQUIRE(loss.isunivfishercons() == true);
    }
}

//template<typename L1 = SupervisedLoss, typename L2 = SupervisedLoss>
//void compare_losses(const L1& l1, const L2& l2, bool ccal = true) {
//    SECTION("isminimizable") {
//        REQUIRE(l1.isminimizable() == l2.isminimizable());
//    }
//    SECTION("isdifferentiable") {
//        REQUIRE(l1.isdifferentiable() == l2.isdifferentiable());
//        REQUIRE(l1.isdifferentiable(0) == l2.isdifferentiable(0));
//        REQUIRE(l1.isdifferentiable(1) == l2.isdifferentiable(1));
//    }
//    SECTION("istwicedifferentiable") {
//        REQUIRE(l1.istwicedifferentiable() == l2.istwicedifferentiable());
//        REQUIRE(l1.istwicedifferentiable(0) == l2.istwicedifferentiable(0));
//        REQUIRE(l1.istwicedifferentiable(1) == l2.istwicedifferentiable(1));
//        REQUIRE(l1.istwicedifferentiable(2) == l2.istwicedifferentiable(2));
//    }
//    SECTION("isstronglyconvex") {
//        REQUIRE(l1.isstronglyconvex() == l2.isstronglyconvex());
//    }
//    SECTION("isstrictlyconvex") {
//        REQUIRE(l1.isstrictlyconvex() == l2.isstrictlyconvex());
//    }
//    SECTION("isconvex") {
//        REQUIRE(l1.isconvex() == l2.isconvex());
//    }
//    SECTION("isnemitski") {
//        REQUIRE(l1.isnemitski() == l2.isnemitski());
//    }
//    SECTION("islipschitzcont") {
//        REQUIRE(l1.islipschitzcont() == l2.islipschitzcont());
//    }
//    SECTION("islocallylipschitzcont") {
//        REQUIRE(l1.islocallylipschitzcont() == l2.islocallylipschitzcont());
//    }
//    SECTION("isclipable") {
//        REQUIRE(l1.isclipable() == l2.isclipable());
//    }
//    SECTION("ismarginbased") {
//        REQUIRE(l1.ismarginbased() == l2.ismarginbased());
//    }
//    SECTION("isdistancebased") {
//        REQUIRE(l1.isdistancebased() == l2.isdistancebased());
//    }
//    SECTION("issymmetric") {
//        REQUIRE(l1.issymmetric() == l2.issymmetric());
//    }
//    if (ccal) {
//        SECTION("isclasscalibrated") {
//            REQUIRE(l1.isclasscalibrated() == l2.isclasscalibrated());
//        }
//    }
//}
//
//TEST_CASE("compare_losses") {
//    compare_losses(PoissonLoss(), 2 * PoissonLoss());
//    compare_losses(PoissonLoss(), 0.5 * PoissonLoss());
//
//    std::vector<std::shared_ptr<SupervisedLoss>> distance_losses = {
//            std::make_shared<L2DistLoss>(),
//            std::make_shared<LPDistLoss>(2.0),
//            std::make_shared<L1DistLoss>(),
//            std::make_shared<LPDistLoss>(1.0),
//            std::make_shared<LPDistLoss>(0.5),
//            std::make_shared<LPDistLoss>(1.5),
//            std::make_shared<LPDistLoss>(3),
//            std::make_shared<LogitDistLoss>(),
//            std::make_shared<L1EpsilonInsLoss>(0.5),
//            std::make_shared<EpsilonInsLoss>(1.5),
//            std::make_shared<L2EpsilonInsLoss>(0.5),
//            std::make_shared<L2EpsilonInsLoss>(1.5),
//            std::make_shared<PeriodicLoss>(1),
//            std::make_shared<PeriodicLoss>(1.5),
//            std::make_shared<HuberLoss>(1),
//            std::make_shared<HuberLoss>(1.5),
//            std::make_shared<QuantileLoss>(0.2),
//            std::make_shared<QuantileLoss>(0.5),
//            std::make_shared<QuantileLoss>(0.8),
//            std::make_shared<LogCoshLoss>()
//    };
//
//    std::vector<std::shared_ptr<SupervisedLoss>> margin_losses = {
//            std::make_shared<LogitMarginLoss>(),
//            std::make_shared<L1HingeLoss>(),
//            std::make_shared<L2HingeLoss>(),
//            std::make_shared<PerceptronLoss>(),
//            std::make_shared<SmoothedL1HingeLoss>(0.5),
//            std::make_shared<SmoothedL1HingeLoss>(1),
//            std::make_shared<SmoothedL1HingeLoss>(2),
//            std::make_shared<ModifiedHuberLoss>(),
//            std::make_shared<ZeroOneLoss>(),
//            std::make_shared<L2MarginLoss>(),
//            std::make_shared<ExpLoss>(),
//            std::make_shared<SigmoidLoss>(),
//            std::make_shared<DWDMarginLoss>(0.5),
//            std::make_shared<DWDMarginLoss>(1),
//            std::make_shared<DWDMarginLoss>(2)
//    };
//
//    std::vector<std::shared_ptr<SupervisedLoss>> other_losses = {
//            std::make_shared<MisclassLoss>(),
//            std::make_shared<PoissonLoss>(),
//            std::make_shared<CrossEntropyLoss>()
//    };
//
//    SECTION("Scaled losses") {
//        for (const auto& loss : distance_losses) {
//            SECTION(STRUCT_NAME_TO_STRING(loss)) {
//                compare_losses(*loss, 2 * *loss);
//                compare_losses(*loss, 0.5 * *loss);
//            }
//        }
//
//        for (const auto& loss : margin_losses) {
//            SECTION(STRUCT_NAME_TO_STRING(loss)) {
//                compare_losses(*loss, 2 * *loss);
//                compare_losses(*loss, 0.5 * *loss);
//            }
//        }
//
//        for (const auto& loss : other_losses) {
//            SECTION(STRUCT_NAME_TO_STRING(loss)) {
//                compare_losses(*loss, 2 * *loss);
//                compare_losses(*loss, 0.5 * *loss);
//            }
//        }
//    }
//
//    SECTION("Weighted Margin-based") {
//        for (const auto& loss : margin_losses) {
//            SECTION(STRUCT_NAME_TO_STRING(loss)) {
//                compare_losses<decltype(loss), WeightedMarginLoss>(loss, WeightedMarginLoss(loss, 0.2), false);
//                compare_losses<decltype(loss), WeightedMarginLoss>(loss, WeightedMarginLoss(loss, 0.5), true);
//                compare_losses<decltype(loss), WeightedMarginLoss>(loss, WeightedMarginLoss(loss, 0.7), false);
//            }
//        }
//    }
//}