#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <catch2/matchers/catch_matchers.hpp>

#include "turing_forge/core/dataset.hpp"
#include "turing_forge/interpreter/interpreter.hpp"
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
            Turingforge::Function(Turingforge::FunctionType::Log)
        },
        Turingforge::Vector<Turingforge::Vector<Turingforge::Scalar>>{
            {3, 0},
            {2, 1}
        }
    );
    fmt::print("{}\n", Turingforge::IndividualFormatter::Format(individual, ds));

    auto ysHat_ = ds.GetValues(target).subspan(trainingRange.Start(), trainingRange.Size()).data();
    Eigen::Map<const Eigen::Matrix<Turingforge::Scalar, Eigen::Dynamic, 1>> ysHat(ysHat_, static_cast<Eigen::Index>(trainingRange.Size()));

    Eigen::Matrix<Turingforge::Scalar, Eigen::Dynamic, Eigen::Dynamic> XsHat(trainingRange.Size(), 1);
    for (auto idx = trainingRange.Start(); idx < trainingRange.End(); ++idx) {
        const auto& _row = ds.Values().row(static_cast<int64_t>(idx)).head(ds.Values().cols() -  1);
        Eigen::Map<const Eigen::Matrix<Turingforge::Scalar, Eigen::Dynamic, 1>> _vecX(_row.data(), _row.size());
        auto _tmp = individual.eval(_vecX);
        Eigen::RowVectorXd row_vector(1, 1);
        row_vector << _tmp;
        XsHat.row(static_cast<int64_t>(idx)) = row_vector;
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

TEST_CASE ("Dispatch Table") {
    using DTable = Turingforge::DispatchTable<Turingforge::Scalar>;

    Turingforge::Dataset ds("../../data/simple.csv", /*hasHeader=*/true);
    auto range = Turingforge::Range { 0, ds.Rows<std::size_t>() };
    auto const& X = ds.Values(); // NOLINT

    Turingforge::Map<std::string, Turingforge::Hash> vars;
    for (auto const& v : ds.GetVariables()) {
        fmt::print("{} : {} {}\n", v.Name, v.Hash, v.Index);
        vars[v.Name] = v.Hash;
    }

    std::vector<size_t> indices(range.Size());
    std::iota(indices.begin(), indices.end(), 0);

    DTable dtable;

    SECTION ("Basic") {
        const auto eps = 1e-3;

        auto individual = Turingforge::Individual(
            Turingforge::Vector<Turingforge::Scalar>{17.37, 12.43},
            Turingforge::Vector<Turingforge::Function>{
                Turingforge::Function(Turingforge::FunctionType::Exp),
                Turingforge::Function(Turingforge::FunctionType::Cos)
            },
            Turingforge::Vector<Turingforge::Vector<Turingforge::Scalar>>{
                {3, 0},
                {2, 1}
            }
        );

        auto estimatedValues = Turingforge::Interpreter<Turingforge::Scalar, DTable>(dtable, ds, individual).Evaluate(range);
        Eigen::Array<Turingforge::Scalar, Eigen::Dynamic, 1> res1(range.Size());

        res1 = (17.37 * (ds.Values().col(0).array().pow(3)).exp() + 12.43 * (ds.Values().col(0).array().pow(2) * ds.Values().col(1).array()).cos());

        fmt::print("estimated: {}\n", std::span{estimatedValues.data(), 5UL});
        fmt::print("actual: {}\n", std::span{res1.data(), 5UL});
        CHECK(std::all_of(indices.begin(), indices.end(), [&](auto i) {
            return std::abs(estimatedValues[i] - res1.data()[i]) < eps;
        }));
    }
}