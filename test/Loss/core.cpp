#include <catch2/catch_test_macros.hpp>

#include "turingforge/Loss/LossFunctions.h"

#define STRUCT_NAME_TO_STRING(struct_type) #struct_type

auto generateRange(double start, double stop, double step){
    std::vector<double> vec;
    vec.reserve(static_cast<std::vector<double>::size_type>((stop - start) / step) + 1);
    while (start <= stop) {
        vec.push_back(start);
        start += step;
    }
    return vec;
}

template<typename Loss, typename Function>
void test_value(Loss& l, Function f, const std::vector<double>& o_vec, const std::vector<double>& t_vec) {
    CAPTURE(l); // Print the loss name in the test output
    for (const auto& o : o_vec) {
        for (const auto& t : t_vec) {
            CAPTURE(o); // Print the value of 'o' in the test output
            CAPTURE(t); // Print the value of 't' in the test output
            double expected = l(o, t);
            double result = f(o, t);
            REQUIRE(std::abs(expected - result) < 1e-10);
        }
    }
}

//void test_deriv(MarginLoss& l, const std::vector<double>& o_vec) {
//    CAPTURE(l); // Print the loss name in the test output
//    for (const auto& o : o_vec) {
//        for (const auto& t : {-1.0, 1.0}) {
//            if (l.isdifferentiable(o * t)) {
//                //double d_dual = epsilon(l(dual(o, double(1)), dual(t, double(0))));
//                double d_comp = l.deriv(o, t);
//                //REQUIRE(std::abs(d_dual - d_comp) < 1e-10);
//                double val = l(o, t);
//                REQUIRE(val == l(o, t));
//                REQUIRE(val == l(o * t));
//                REQUIRE(d_comp == t * l.deriv(o * t));
//            }
//        }
//    }
//}
//
//void test_deriv(DistanceLoss& l, const std::vector<double>& o_vec) {
//    CAPTURE(l); // Print the loss name in the test output
//    for (const auto& o : o_vec) {
//        for (const auto& t : {-10.0, 0.2, 10.0}) {
//            if (l.isdifferentiable(o - t)) {
//                //double d_dual = epsilon(l(dual(o - t, double(1))));
//                double d_comp = l.deriv(o, t);
//                //REQUIRE(std::abs(d_dual - d_comp) < 1e-10);
//                double val = l(o, t);
//                REQUIRE(val == l(o, t));
//                REQUIRE(val == l(o - t));
//                REQUIRE(d_comp == l.deriv(o - t));
//            }
//        }
//    }
//}
//
//void test_deriv(SupervisedLoss& l, const std::vector<double>& o_vec, const std::vector<double>& t_vec) {
//    CAPTURE(l); // Print the loss name in the test output
//    for (const auto& o : o_vec) {
//        for (const auto& t : t_vec) {
//            if (l.isdifferentiable(o, t)) {
//                //double d_dual = epsilon(l(dual(o, one(o)), dual(t, zero(t))));
//                double d_comp = l.deriv(o, t);
//                //REQUIRE(std::abs(d_dual - d_comp) < 1e-10);
//                double val = l(o, t);
//                REQUIRE(val == l(o, t));
//                REQUIRE(d_comp == l.deriv(o, t));
//            }
//        }
//    }
//}
//
//void test_deriv2(MarginLoss& l, const std::vector<double>& o_vec) {
//    CAPTURE(l); // Print the loss name in the test output
//    for (const auto& o : o_vec) {
//        for (const auto& t : {-1.0, 1.0}) {
//            if (l.istwicedifferentiable(o * t) && l.isdifferentiable(o * t)) {
//                //double d2_dual = epsilon(deriv(l, dual(o, one(o)), dual(t, zero(t))));
//                double d2_comp = l.deriv2(o, t);
//                //REQUIRE(std::abs(d2_dual - d2_comp) < 1e-10);
//                REQUIRE(d2_comp == l.deriv2(o, t));
//                REQUIRE(d2_comp == l.deriv2(o * t));
//            }
//        }
//    }
//}
//
//void test_deriv2(DistanceLoss& l, const std::vector<double>& o_vec) {
//    CAPTURE(l); // Print the loss name in the test output
//    for (const auto& o : o_vec) {
//        for (const auto& t : {-10.0, 0.2, 10.0}) {
//            if (l.istwicedifferentiable(o - t) && l.isdifferentiable(o - t)) {
//                //double d2_dual = epsilon(deriv(l, dual(o - t, one(o - t))));
//                double d2_comp = l.deriv2(o, t);
//                //REQUIRE(std::abs(d2_dual - d2_comp) < 1e-10);
//                REQUIRE(d2_comp == l.deriv2(o, t));
//                REQUIRE(d2_comp == l.deriv2(o - t));
//            }
//        }
//    }
//}
//
//void test_deriv2(SupervisedLoss& l, const std::vector<double>& o_vec, const std::vector<double>& t_vec) {
//    CAPTURE(l); // Print the loss name in the test output
//    for (const auto& o : o_vec) {
//        for (const auto& t : t_vec) {
//            if (l.istwicedifferentiable(o, t) && l.isdifferentiable(o, t)) {
//                //double d2_dual = epsilon(deriv(dual(o, one(o)), dual(t, zero(t))));
//                double d2_comp = l.deriv2(o, t);
//                //REQUIRE(std::abs(d2_dual - d2_comp) < 1e-10);
//                REQUIRE(d2_comp == l.deriv2(o, t));
//            }
//        }
//    }
//}

//void test_scaledloss(SupervisedLoss& l, const std::vector<double>& o_vec, const std::vector<double>& t_vec) {
//    SECTION("Scaling for ", STRUCT_NAME_TO_STRING(l)) {
//        for (double lambda : {2.0, 2.0}) {
//            ScaledLoss sl(l, lambda);
//            REQUIRE(3 * sl == ScaledLoss(sl, 3));
//            REQUIRE((lambda * 3) * l == ScaledLoss(sl, 3));
//            REQUIRE(sl == ScaledLoss(l, lambda));
//            REQUIRE(sl == lambda * l);
//            REQUIRE(sl == lambda * l);
//            for (double o : o_vec) {
//                for (double t : t_vec) {
//                    REQUIRE(sl(o, t) == lambda * l(o, t));
//                    REQUIRE(sl.deriv(o, t) == lambda * l.deriv(o, t));
//                    REQUIRE(sl.deriv2(o, t) == lambda * l.deriv2(o, t));
//                }
//            }
//        }
//    }
//}

//// Define the test_weightedloss function here
//void test_weightedloss(MarginLoss& l, const std::vector<double>& o_vec, const std::vector<double>& t_vec) {
//    SECTION("Weighted version for " + l.name()) {
//        for (double w : {0.0, 0.2, 0.7, 1.0}) {
//            WeightedMarginLoss wl(l, w);
//            REQUIRE(std::is_base_of_v<WeightedMarginLoss<decltype(l), w>, decltype(wl)>);
//            REQUIRE(WeightedMarginLoss(l, w * 0.1) == WeightedMarginLoss(wl, 0.1));
//            for (double o : o_vec) {
//                for (double t : t_vec) {
//                    if (t == 1) {
//                        REQUIRE(wl(o, t) == w * l(o, t));
//                        REQUIRE(deriv(wl, o, t) == w * deriv(l, o, t));
//                        REQUIRE(deriv2(wl, o, t) == w * deriv2(l, o, t));
//                    } else {
//                        REQUIRE(wl(o, t) == (1 - w) * l(o, t));
//                        REQUIRE(deriv(wl, o, t) == (1 - w) * deriv(l, o, t));
//                        REQUIRE(deriv2(wl, o, t) == (1 - w) * deriv2(l, o, t));
//                    }
//                }
//            }
//        }
//    }
//}

TEST_CASE("Test margin-based loss against reference function", "[MarginLoss]") {
    auto _zerooneloss = [](double o, double t) {
        return (std::signbit(o * t) ? 1.0 : 0.0);
    };
    ZeroOneLoss zeroOneLoss;
    test_value(zeroOneLoss, _zerooneloss, generateRange(-10, 10, 0.2), std::vector<double>{-1.0, 1.0});

    auto _hingeloss = [](double o, double t) {
        return std::max(0.0, 1.0 - o * t);
    };
    HingeLoss hingeLoss;
    test_value(hingeLoss, _hingeloss, generateRange(-10, 10, 0.2), std::vector<double>{-1.0, 1.0});

    auto _l2hingeloss = [](double o, double t) {
        double loss = std::max(0.0, 1.0 - o * t);
        return loss * loss;
    };
    L2HingeLoss l2HingeLoss;
    test_value(l2HingeLoss, _l2hingeloss, generateRange(-10, 10, 0.2), std::vector<double>{-1.0, 1.0});

    auto _perceptronloss = [](double o, double t) {
        return std::max(0.0, -o * t);
    };
    PerceptronLoss perceptronLoss;
    test_value(perceptronLoss, _perceptronloss, generateRange(-10, 10, 0.2), std::vector<double>{-1.0, 1.0});

    auto _logitmarginloss = [](double o, double t) {
        return std::log(1 + std::exp(-o * t));
    };
    LogitMarginLoss logitMarginLoss;
    test_value(logitMarginLoss, _logitmarginloss, generateRange(-10, 10, 0.2), std::vector<double>{-1.0, 1.0});

    auto _smoothedl1hingeloss = [](double gamma) {
        auto _value = [gamma](double o, double t) {
            if (o * t >= 1 - gamma) {
                return (1 / (2 * gamma)) * std::max(0.0, 1.0 - o * t) * std::max(0.0, 1.0 - o * t);
            } else {
                return 1 - gamma / 2 - o * t;
            }
        };
        return _value;
    };
    SmoothedL1HingeLoss smoothedL1HingeLossA(0.5);
    SmoothedL1HingeLoss smoothedL1HingeLossB(1);
    SmoothedL1HingeLoss smoothedL1HingeLossC(2);
    test_value(smoothedL1HingeLossA, _smoothedl1hingeloss(0.5), generateRange(-10, 10, 0.2), std::vector<double>{-1.0, 1.0});
    test_value(smoothedL1HingeLossB, _smoothedl1hingeloss(1), generateRange(-10, 10, 0.2), std::vector<double>{-1.0, 1.0});
    test_value(smoothedL1HingeLossC, _smoothedl1hingeloss(2), generateRange(-10, 10, 0.2), std::vector<double>{-1.0, 1.0});

    auto _modhuberloss = [](double o, double t) {
        if (o * t >= -1) {
            return std::max(0.0, 1.0 - o * t) * std::max(0.0, 1.0 - o * t);
        } else {
            return -4.0 * o * t;
        }
    };
    ModifiedHuberLoss modifiedHuberLoss;
    test_value(modifiedHuberLoss, _modhuberloss, generateRange(-10, 10, 0.2), std::vector<double>{-1.0, 1.0});

    auto _l2marginloss = [](double o, double t) {
        double loss = 1 - o * t;
        return loss * loss;
    };
    L2MarginLoss l2MarginLoss;
    test_value(l2MarginLoss, _l2marginloss, generateRange(-10, 10, 0.2), std::vector<double>{-1.0, 1.0});

    auto _exploss = [](double o, double t) {
        return std::exp(-o * t);
    };
    ExpLoss expLoss;
    test_value(expLoss, _exploss, generateRange(-10, 10, 0.2), std::vector<double>{-1.0, 1.0});

    auto _sigmoidloss = [](double o, double t) {
        return (1 - std::tanh(o * t));
    };
    SigmoidLoss sigmoidLoss;
    test_value(sigmoidLoss, _sigmoidloss, generateRange(-10, 10, 0.2), std::vector<double>{-1.0, 1.0});

    auto _dwdmarginloss = [](double q) {
        auto _value = [q](double o, double t) {
            if (o * t <= q / (q + 1)) {
                return 1 - o * t;
            } else {
                double numerator = std::pow(q, q) / std::pow(q + 1, q + 1);
                double denominator = std::pow(o * t, q);
                return numerator / denominator;
            }
        };
        return _value;
    };
    DWDMarginLoss dWDMarginLossA(0.5);
    DWDMarginLoss dWDMarginLossB(1);
    DWDMarginLoss dWDMarginLossC(2);
    test_value(dWDMarginLossA, _dwdmarginloss(0.5), generateRange(-10, 10, 0.2), std::vector<double>{-1.0, 1.0});
    test_value(dWDMarginLossB, _dwdmarginloss(1), generateRange(-10, 10, 0.2), std::vector<double>{-1.0, 1.0});
    test_value(dWDMarginLossC, _dwdmarginloss(2), generateRange(-10, 10, 0.2), std::vector<double>{-1.0, 1.0});
}

//TEST_CASE("Test distance-based loss against reference function", "[DistanceLoss]") {
//    auto or = Catch::range(-10, 20, 10);
//    auto tr = Catch::range(-30, 30, 10);
//
//    auto _l1distloss = [](double o, double t) {
//        return std::abs(t - o);
//    };
//    test_value(L1DistLoss(), _l1distloss, or, tr);
//
//    auto _l2distloss = [](double o, double t) {
//        double diff = t - o;
//        return diff * diff;
//    };
//    test_value(L2DistLoss(), _l2distloss, or, tr);
//
//    auto _lp15distloss = [](double o, double t) {
//        double diff = std::abs(t - o);
//        return std::pow(diff, 1.5);
//    };
//    test_value(LPDistLoss(1.5), _lp15distloss, or, tr);
//
//    auto _periodicloss = [](double c) {
//        auto _value = [c](double o, double t) {
//            return 1 - std::cos((o - t) * 2 * M_PI / c);
//        };
//        return _value;
//    };
//    test_value(PeriodicLoss(0.5), _periodicloss(0.5), or, tr);
//    test_value(PeriodicLoss(1), _periodicloss(1), or, tr);
//    test_value(PeriodicLoss(1.5), _periodicloss(1.5), or, tr);
//
//    auto _huberloss = [](double d) {
//        auto _value = [d](double o, double t) {
//            double diff = std::abs(o - t);
//            if (diff < d) {
//                return diff * diff / 2;
//            } else {
//                return d * (diff - d / 2);
//            }
//        };
//        return _value;
//    };
//    test_value(HuberLoss(0.5), _huberloss(0.5), or, tr);
//    test_value(HuberLoss(1), _huberloss(1), or, tr);
//    test_value(HuberLoss(1.5), _huberloss(1.5), or, tr);
//
//    auto _l1epsinsloss = [](double ɛ) {
//        auto _value = [ɛ](double o, double t) {
//            return std::max(0.0, std::abs(t - o) - ɛ);
//        };
//        return _value;
//    };
//    test_value(EpsilonInsLoss(0.5), _l1epsinsloss(0.5), or, tr);
//    test_value(EpsilonInsLoss(1), _l1epsinsloss(1), or, tr);
//    test_value(EpsilonInsLoss(1.5), _l1epsinsloss(1.5), or, tr);
//
//    auto _l2epsinsloss = [](double ɛ) {
//        auto _value = [ɛ](double o, double t) {
//            double diff = std::abs(t - o) - ɛ;
//            return diff * diff;
//        };
//        return _value;
//    };
//    test_value(L2EpsilonInsLoss(0.5), _l2epsinsloss(0.5), or, tr);
//    test_value(L2EpsilonInsLoss(1), _l2epsinsloss(1), or, tr);
//    test_value(L2EpsilonInsLoss(1.5), _l2epsinsloss(1.5), or, tr);
//
//    auto _logitdistloss = [](double o, double t) {
//        double diff = t - o;
//        return -std::log((4 * std::exp(diff)) / (1 + std::exp(diff)) / (1 + std::exp(diff)));
//    };
//    test_value(LogitDistLoss(), _logitdistloss, or, tr);
//
//    auto _quantileloss = [](double τ) {
//        auto _value = [τ](double o, double t) {
//            return (o - t) * ((o - t > 0) - τ);
//        };
//        return _value;
//    };
//    test_value(QuantileLoss(0.7), _quantileloss(0.7), or, tr);
//
//    auto _logcoshloss = [](double o, double t) {
//        return std::log(std::cosh(o - t));
//    };
//    test_value(LogCoshLoss(), _logcoshloss, or, tr);
//
//    auto _kldivloss = [](double o, double t) {
//        return t * std::log(t / o) + (1 - t) * std::log((1 - t) / (1 - o));
//    };
//    test_value(KLDivergenceLoss(), _kldivloss, or, tr);
//}
//
//TEST_CASE("Test other loss") {
//    auto _misclassloss = [](double o, double t) { return o == t ? 0 : 1; };
//    REQUIRE(_misclassloss(1, 1) == 0);
//    REQUIRE(_misclassloss(1, 2) == 1);
//
//    auto _crossentropyloss = [](double o, double t) {
//        return -t * std::log(o) - (1 - t) * std::log(1 - o);
//    };
//    REQUIRE(_crossentropyloss(0.5, 0.5) == Approx(0));
//    REQUIRE(_crossentropyloss(0.5, 0.8) == Approx(0.22314355131));
//
//    auto _poissonloss = [](double o, double t) { return std::exp(o) - t * o; };
//    REQUIRE(_poissonloss(1, 1) == Approx(0));
//    REQUIRE(_poissonloss(1, 2) == Approx(-1));
//
//    std::vector<double> range_values;
//    for (int i = 0; i <= 10; i++) {
//        range_values.push_back(i);
//    }
//
//    SECTION("MisclassLoss") {
//        std::vector<double> targets = { 1, 2, 3, 4, 5, 7, 8, 9, 10 };
//        std::vector<double> expected_results = { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
//        for (size_t i = 0; i < range_values.size(); i++) {
//            REQUIRE(misclassloss(range_values[i], targets[i]) == expected_results[i]);
//        }
//    }
//
//    SECTION("CrossEntropyLoss") {
//        std::vector<double> targets = { 0, 0.01, 0.02, 0.03, 0.04, 0.05, 0.06, 0.07, 0.08, 0.09, 0.1 };
//        std::vector<double> expected_results = { 0.69314718056, 0.66329420405, 0.64439615539, 0.62914831512,
//                                                 0.61570673577, 0.60354349518, 0.59233937032, 0.58187926569,
//                                                 0.57200775991, 0.56260669692, 0.55358840466 };
//        for (size_t i = 0; i < range_values.size(); i++) {
//            REQUIRE(crossentropyloss(range_values[i], targets[i]) == Approx(expected_results[i]));
//        }
//    }
//
//    SECTION("PoissonLoss") {
//        std::vector<double> targets = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
//        std::vector<double> expected_results = { 1, 0.71828182846, -1.26424111766, -5.28171817154,
//                                                 -13.6917362, -29.5562244, -55.5981500,
//                                                 -95.6578501, -154.6119760, -238.413285, -354.688758 };
//        for (size_t i = 0; i < range_values.size(); i++) {
//            REQUIRE(poissonloss(range_values[i], targets[i]) == Approx(expected_results[i]));
//        }
//    }
//}
//
//TEST_CASE("Test scaled loss") {
//    std::vector<double> range_values;
//    for (double i = -10; i <= 10; i += 0.5) {
//        range_values.push_back(i);
//    }
//
//    std::vector<double> scaled_values;
//    for (double i = -10; i <= 10; i += 0.2) {
//        scaled_values.push_back(i);
//    }
//
//    SECTION("DistanceLoss") {
//        for (const auto& value : range_values) {
//            // test_scaledloss(DistanceLoss(), -10:0.5:10, -10:0.2:10)
//            // Add your test code here
//        }
//    }
//
//    SECTION("MarginLoss") {
//        for (const auto& value : range_values) {
//            // test_scaledloss(MarginLoss(), -10:0.5:10, -10:0.2:10)
//            // Add your test code here
//        }
//        std::vector<double> margin_values = { -1.0, 1.0 };
//        for (const auto& value : scaled_values) {
//            // test_scaledloss(MarginLoss(), -10:0.2:10, [-1.0, 1.0])
//            // Add your test code here
//        }
//    }
//
//    SECTION("PoissonLoss") {
//        std::vector<double> targets = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
//        for (const auto& value : range_values) {
//            // test_scaledloss(PoissonLoss(), range(0, stop=10, length=11), 0:10)
//            // Add your test code here
//        }
//    }
//}
//
//TEST_CASE("Test weighted loss") {
//    std::vector<double> range_values;
//    for (double i = -10; i <= 10; i += 0.2) {
//        range_values.push_back(i);
//    }
//
//    std::vector<double> margin_values = { -1.0, 1.0 };
//    for (const auto& value : margin_values) {
//        // test_weightedloss(MarginLoss(), -10:0.2:10, [-1.0, 1.0])
//        // Add your test code here
//    }
//}
//
//TEST_CASE("Test first derivatives") {
//    std::vector<double> range_values;
//    for (double i = -10; i <= 10; i += 0.5) {
//        range_values.push_back(i);
//    }
//
//    std::vector<double> scaled_values;
//    for (double i = -10; i <= 10; i += 0.2) {
//        scaled_values.push_back(i);
//    }
//
//    SECTION("DistanceLoss") {
//        for (const auto& value : range_values) {
//            // test_deriv(DistanceLoss(), -10:0.5:10)
//            // Add your test code here
//        }
//    }
//
//    SECTION("MarginLoss") {
//        for (const auto& value : range_values) {
//            // test_deriv(MarginLoss(), -10:0.2:10)
//            // Add your test code here
//        }
//    }
//
//    SECTION("PoissonLoss") {
//        std::vector<double> targets;
//        for (int i = 0; i <= 30; i++) {
//            targets.push_back(i);
//        }
//        for (const auto& value : range_values) {
//            // test_deriv(PoissonLoss(), -10:0.2:10, 0:30)
//            // Add your test code here
//        }
//    }
//
//    SECTION("CrossEntropyLoss") {
//        std::vector<double> targets;
//        for (double i = 0.01; i <= 0.99; i += 0.01) {
//            targets.push_back(i);
//        }
//        for (double i = 0; i <= 1; i += 0.01) {
//            // test_deriv(CrossEntropyLoss(), 0.01:0.01:0.99, 0:0.01:1)
//            // Add your test code here
//        }
//    }
//}
//
//TEST_CASE("Test second derivatives") {
//    std::vector<double> range_values;
//    for (double i = -10; i <= 10; i += 0.5) {
//        range_values.push_back(i);
//    }
//
//    std::vector<double> scaled_values;
//    for (double i = -10; i <= 10; i += 0.2) {
//        scaled_values.push_back(i);
//    }
//
//    SECTION("DistanceLoss") {
//        for (const auto& value : range_values) {
//            // test_deriv2(DistanceLoss(), -10:0.5:10)
//            // Add your test code here
//        }
//    }
//
//    SECTION("MarginLoss") {
//        for (const auto& value : range_values) {
//            // test_deriv2(MarginLoss(), -10:0.2:10)
//            // Add your test code here
//        }
//    }
//
//    SECTION("PoissonLoss") {
//        std::vector<double> targets;
//        for (int i = 0; i <= 30; i++) {
//            targets.push_back(i);
//        }
//        for (const auto& value : range_values) {
//            // test_deriv2(PoissonLoss(), -10:0.2:10, 0:30)
//            // Add your test code here
//        }
//    }
//
//    SECTION("CrossEntropyLoss") {
//        std::vector<double> targets;
//        for (double i = 0.01; i <= 0.99; i += 0.01) {
//            targets.push_back(i);
//        }
//        for (double i = 0; i <= 1; i += 0.01) {
//            // test_deriv2(CrossEntropyLoss(), 0.01:0.01:0.99, 0:0.01:1)
//            // Add your test code here
//        }
//    }
//}
//
//TEST_CASE("Test losses with categorical values") {
//    std::vector<std::string> c = { "Foo", "Bar", "Baz", "Foo" };
//
//    SECTION("MisclassLoss") {
//        REQUIRE(misclassloss(c[0], c[0]) == Approx(0.0));
//        REQUIRE(misclassloss(c[0], c[1]) == Approx(1.0));
//        std::vector<double> expected_results = { 0.0, 1.0, 1.0, 0.0 };
//        std::vector<std::string> reversed_c(c.rbegin(), c.rend());
//        for (size_t i = 0; i < c.size(); i++) {
//            REQUIRE(misclassloss(c[i], reversed_c[i]) == Approx(expected_results[i]));
//        }
//    }
//
//    SECTION("MisclassLoss with Float32") {
//        REQUIRE(misclassloss<float>(c[0], c[0]) == Approx(0.0f));
//        REQUIRE(misclassloss<float>(c[0], c[1]) == Approx(1.0f));
//        REQUIRE(misclassloss<float>(c, c).size() == c.size());
//    }
//}