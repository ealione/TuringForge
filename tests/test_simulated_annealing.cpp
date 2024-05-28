#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <catch2/matchers/catch_matchers.hpp>

#include <Eigen/Dense>

#include "turing_forge/core/dataset.hpp"
#include "turing_forge/core/primitive_set.hpp"
#include "turing_forge/operators/creator.hpp"
#include "turing_forge/core/individual.hpp"

TEST_CASE( "Quick check", "[main]" ) {
    int minSize = 1;
    int maxSize = 4;
    int popSize = 150;
    int nvars = 2;
    int expolim = 3;
    int generations = 50;
    int selectedSize = 50;
    double stopScore = 0.99;
    const auto *target = "Y";

    // Dataset
    auto ds = Turingforge::Dataset("../../data/Poly-10.csv", /*hasHeader=*/true);
    auto inputs = ds.VariableHashes();
    std::erase(inputs, ds.GetVariable(target)->Hash);
    auto ysHat_ = ds.GetValues(target);

    // Initial Population
    int rangeSize = maxSize - minSize + 1;
    int numIndividualsPerExpSize = static_cast<int>(std::round(static_cast<double>(popSize) / rangeSize));

    std::vector<Eigen::Matrix<double, 1, Eigen::Dynamic>> pop(popSize);
    Turingforge::PrimitiveSet grammar;
    grammar.SetConfig(Turingforge::PrimitiveSet::Arithmetic | Turingforge::FunctionType::Log | Turingforge::FunctionType::Exp);
    Turingforge::BalancedCreator btc{grammar, inputs, /* bias= */ 0.0};
    Turingforge::RandomGenerator random(std::random_device{}());

    for (int expSize = minSize; expSize <= maxSize; ++expSize) {
        for (int i = 0; i < numIndividualsPerExpSize; ++i) {
            auto newInd = btc(random, nvars, expolim);
            newInd.sanitize();
            newInd.fit(ds.Ge);
//            pop.push_back(fitIT(newInd, Xs, ys)); // Assuming Xs and ys are defined elsewhere
        }
    }
}