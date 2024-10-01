#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <catch2/matchers/catch_matchers.hpp>

#include "turing_forge/formatter/formatter.hpp"
#include "turing_forge/interpreter/dispatch_table.hpp"
#include "turing_forge/core/dataset.hpp"
#include "turing_forge/interpreter/interpreter.hpp"

TEST_CASE( "Basic Evaluation" ) {
    using DT = Turingforge::DispatchTable<Turingforge::Scalar>;

    std::string x{"x"};
    std::vector<Turingforge::Scalar> v{0};
    Turingforge::Dataset ds({x}, {v});

    auto check = [&](DT const& dt, const Turingforge::Individual& ind, Turingforge::Scalar expected) {
        Turingforge::Map<std::string, Turingforge::Hash> vars;
        fmt::print("Check expression {} == {}\n", Turingforge::IndividualFormatter::Format(ind, ds), expected);
        auto r = Turingforge::Interpreter<Turingforge::Scalar, DT>(dt, ds, ind).Evaluate(Turingforge::Range(0, 1));
        CHECK(r[0] == expected);
    };

    // default ctor
    DT dt;
    check(dt, Turingforge::Individual({2}, {Turingforge::Function(Turingforge::FunctionType::Cos)}, {{2}}), 2);

    // copy ctor
    DT dt1(dt);
    check(dt1, Turingforge::Individual({3}, {Turingforge::Function(Turingforge::FunctionType::Cos)}, {{2}}), 3);

    // move ctor
    DT dt2(std::move(dt1));
    check(dt2, Turingforge::Individual({2, 3}, {Turingforge::Function(Turingforge::FunctionType::Sin), Turingforge::Function(Turingforge::FunctionType::Cos)}, {{2}, {4}}), 3);

    // ctor from map const&
    auto const& map = dt.GetMap();
    const DT dt3(map);
    check(dt3, Turingforge::Individual({2}, {Turingforge::Function(Turingforge::FunctionType::Cos)}, {{2}}), 2 * std::cos(0));

    // ctor from map&&
    DT dt4(std::move(map));
    check(dt4, Turingforge::Individual({2}, {Turingforge::Function(Turingforge::FunctionType::Cos)}, {{2}}), 2.f);
}