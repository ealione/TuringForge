#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <catch2/matchers/catch_matchers.hpp>

#include "turing_forge/core/dataset.hpp"
#include "turing_forge/core/types.hpp"
#include "turing_forge/core/problem.hpp"
#include "turing_forge/error_metrics/mean_squared_error.hpp"
#include "turing_forge/formatter/formatter.hpp"
#include "turing_forge/interpreter/dispatch_table.hpp"

TEST_CASE("Evaluation correctness", "Basic operations")
{
    Turingforge::Dataset ds("../../data/simple.csv", /*hasHeader=*/true);
    Turingforge::Range trainingRange { 0, ds.Rows<size_t>() / 2 };
    Turingforge::Range testRange     { ds.Rows<size_t>() / 2, ds.Rows<size_t>() };

    using DTable = Turingforge::DispatchTable<Turingforge::Scalar>;
    auto const& X = ds.Values(); // NOLINT

    Turingforge::Map<std::string, Turingforge::Hash> vars;
    for (auto const& v : ds.GetVariables()) {
        fmt::print("{} : {} {}\n", v.Name, v.Hash, v.Index);
        vars[v.Name] = v.Hash;
    }

    const std::string target = "ΔP";
    Turingforge::Problem problem(ds, trainingRange, testRange);
    problem.GetPrimitiveSet().SetConfig(Turingforge::PrimitiveSet::Arithmetic);

    // auto const* ptr = ds.GetValues("ρ").subspan(trainingRange.Start(), trainingRange.Size()).data();

    auto individual = Turingforge::Individual(
        Turingforge::Vector<Turingforge::Scalar>{17.37, 12.43},
        Turingforge::Vector<Turingforge::Function>{
            Turingforge::Function(Turingforge::FunctionType::Exp),
            Turingforge::Function(Turingforge::FunctionType::Log)},
        Turingforge::Vector<Turingforge::Vector<Turingforge::Scalar>>{
            {3, 0},
            {2, 1}}
    );
    fmt::print("{}\n", Turingforge::IndividualFormatter::Format(individual, ds));

    auto ysHat_ = ds.GetValues(target).subspan(trainingRange.Start(), trainingRange.Size()).data();
    Eigen::Map<const Eigen::Matrix<Turingforge::Scalar, Eigen::Dynamic, 1>> ysHat(ysHat_, static_cast<Eigen::Index>(trainingRange.Size()));

    Eigen::Matrix<Turingforge::Scalar, Eigen::Dynamic, Eigen::Dynamic> XsHat(trainingRange.Size(), individual.Functions.size());
    for (auto idx = trainingRange.Start(); idx < trainingRange.End(); ++idx) {
        const auto& _row = ds.Values().row(static_cast<int64_t>(idx)).head(ds.Values().cols() -  1);
        Eigen::Map<const Eigen::Matrix<Turingforge::Scalar, Eigen::Dynamic, 1>> _vecX(_row.data(), _row.size());
        auto _tmp = individual.fit(_vecX);
        XsHat.row(static_cast<int64_t>(idx)) = _tmp;
    }

    // Transpose XsHat
    Eigen::Matrix<Turingforge::Scalar, Eigen::Dynamic, Eigen::Dynamic> XsHatT = XsHat.transpose();

    // Compute q1 = inv(XsHatT * XsHat)
    Eigen::Matrix<Turingforge::Scalar, Eigen::Dynamic, Eigen::Dynamic> q1 = (XsHatT * XsHat).inverse();

    // Compute q2 = XsHatT * ysHat
    Eigen::Matrix<Turingforge::Scalar, Eigen::Dynamic, Eigen::Dynamic> q2 = XsHatT * ysHat;

    // Compute ws = q1 * q2
    Eigen::Matrix<Turingforge::Scalar, Eigen::Dynamic, Eigen::Dynamic> ws = q1 * q2;

    assert(ws.cols() == 1);
    Eigen::Index numCoefficients = ws.rows();
    std::vector<double> coefficients(numCoefficients);
    std::copy(ws.data(), ws.data() + numCoefficients, individual.Coefficients.begin());

    std::cout << ws << "\n";
}