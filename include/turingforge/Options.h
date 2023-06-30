#include <iostream>
#include <vector>
#include <unordered_map>
#include <variant>
#include <cmath>
#include <algorithm>
#include <numeric>

enum OperatorEnum {
    PLUS,
    MULT,
    SUB,
    DIV,
    POW,
    LOG,
    LOG10,
    LOG2,
    LOG1P,
    SQRT,
    ACOSH,
    ATANH
};

struct Node {
    OperatorEnum op;
    std::variant<double, Node*, std::string> value;
    std::vector<Node*> children;
};

std::string string_tree(Node* node) {
    // Implementation omitted for brevity
    return "";
}

struct Options {
    std::vector<OperatorEnum> binary_operators;
    std::vector<OperatorEnum> unary_operators;
    std::unordered_map<OperatorEnum, std::pair<int, int>> constraints;
};

std::tuple<std::vector<int>, std::vector<std::tuple<int,int>>> build_constraints(
        std::vector<int> una_constraints, std::vector<std::tuple<int, int>> bin_constraints,
        const std::vector<std::string>& unary_operators, const std::vector<std::string>& binary_operators,
        int nuna, int nbin)
{
    bool is_bin_constraints_already_done = typeid(bin_constraints) == typeid(std::vector<std::tuple<int, int>>);
    bool is_una_constraints_already_done = typeid(una_constraints) == typeid(std::vector<int>);

    if (typeid(bin_constraints) == typeid(std::vector<std::tuple<int,int>>) && !is_bin_constraints_already_done)
    {
        std::unordered_map<std::string, std::tuple<int,int>> bin_map(bin_constraints.begin(), bin_constraints.end());
        bin_constraints.clear();
        for (const auto& op : binary_operators)
        {
            bool did_user_declare_constraints = bin_map.find(op) != bin_map.end();
            if (did_user_declare_constraints)
            {
                auto constraint = bin_map[op];
                bin_constraints.push_back(constraint);
            }
            else
            {
                bin_constraints.push_back(std::make_tuple(-1,-1));
            }
        }
    }

    if (una_constraints.empty())
    {
        una_constraints.resize(nuna, -1);
    }
    else if (!is_una_constraints_already_done)
    {
        std::unordered_map<std::string, int> una_map(una_constraints.begin(), una_constraints.end());
        una_constraints.clear();
        for (const auto& op : unary_operators)
        {
            bool did_user_declare_constraints = una_map.find(op) != una_map.end();
            if (did_user_declare_constraints)
            {
                auto constraint = una_map[op];
                una_constraints.push_back(constraint);
            }
            else
            {
                una_constraints.push_back(-1);
            }
        }
    }

    return {una_constraints, bin_constraints};
}

OperatorEnum binopmap(OperatorEnum op) {
    switch (op) {
        case PLUS:
            return OperatorEnum::PLUS;
        case MULT:
            return OperatorEnum::MULT;
        case SUB:
            return OperatorEnum::SUB;
        case DIV:
            return OperatorEnum::DIV;
        case POW:
            return OperatorEnum::POW;
        case LOG:
            return OperatorEnum::LOG;
        case LOG10:
            return OperatorEnum::LOG10;
        case LOG2:
            return OperatorEnum::LOG2;
        case LOG1P:
            return OperatorEnum::LOG1P;
        case SQRT:
            return OperatorEnum::SQRT;
        case ACOSH:
            return OperatorEnum::ACOSH;
        case ATANH:
            return OperatorEnum::ATANH;
        default:
            return op;
    }
}

OperatorEnum unaopmap(OperatorEnum op) {
    switch (op) {
        case LOG:
            return OperatorEnum::LOG;
        case LOG10:
            return OperatorEnum::LOG10;
        case LOG2:
            return OperatorEnum::LOG2;
        case LOG1P:
            return OperatorEnum::LOG1P;
        case SQRT:
            return OperatorEnum::SQRT;
        case ACOSH:
            return OperatorEnum::ACOSH;
        case ATANH:
            return OperatorEnum::ATANH;
        default:
            return op;
    }
}

const std::unordered_map<std::string, std::string> math_functions{
        {"+", "plus"},
        {"*", "mult"},
        {"-", "sub"},
        {"/", "div"},
        {"^", "pow"},
        {"log", "log"},
        {"log10", "log10"},
        {"log2", "log2"},
        {"log1p", "log1p"},
        {"sqrt", "sqrt"},
        {"acosh", "acosh"},
        {"atanh", "atanh"}
};

std::variant<double, Node*, std::string> evaluate_operator(OperatorEnum op, const std::vector<std::variant<double, Node*, std::string>>& args) {
    switch (op) {
        case OperatorEnum::PLUS:
            return std::visit([](auto&&... nums) { return (static_cast<double>(nums) + ...); }, args);
        case OperatorEnum::MULT:
            return std::visit([](auto&&... nums) { return (static_cast<double>(nums) * ...); }, args);
        case OperatorEnum::SUB:
            return std::visit([](auto&& a, auto&&... nums) { return a - (static_cast<double>(nums) + ...); }, args);
        case OperatorEnum::DIV:
            return std::visit([](auto&& a, auto&&... nums) { return a / (static_cast<double>(nums) * ...); }, args);
        case OperatorEnum::POW:
            return std::pow(std::get<double>(args[0]), std::get<double>(args[1]));
        case OperatorEnum::LOG:
            return std::log(std::get<double>(args[0]));
        case OperatorEnum::LOG10:
            return std::log10(std::get<double>(args[0]));
        case OperatorEnum::LOG2:
            return std::log2(std::get<double>(args[0]));
        case OperatorEnum::LOG1P:
            return std::log1p(std::get<double>(args[0]));
        case OperatorEnum::SQRT:
            return std::sqrt(std::get<double>(args[0]));
        case OperatorEnum::ACOSH:
            return std::acosh(std::get<double>(args[0]));
        case OperatorEnum::ATANH:
            return std::atanh(std::get<double>(args[0]));
        default:
            return 0.0;
    }
}

std::pair<Node*, std::string> parse_expr(const std::string& expr, const Options& options) {
    // Implementation omitted for brevity
    return {nullptr, ""};
}

Node* build_expression_tree(const std::string& expr, const Options& options) {
    // Implementation omitted for brevity
    return nullptr;
}

double evaluate_expression_tree(Node* node) {
    // Implementation omitted for brevity
    return 0.0;
}

int main() {
    // Example usage
    std::string expression = "3 + 4 * log(10)";
    Options options;
    options.binary_operators = {OperatorEnum::PLUS, OperatorEnum::MULT};
    options.unary_operators = {OperatorEnum::LOG};
    options.constraints[OperatorEnum::PLUS] = {1, 2};
    options.constraints[OperatorEnum::MULT] = {2, 2};
    options.constraints[OperatorEnum::LOG] = {1, 1};

    Node* tree = build_expression_tree(expression, options);
    double result = evaluate_expression_tree(tree);

    std::cout << "Expression: " << expression << std::endl;
    std::cout << "Result: " << result << std::endl;

    // Cleanup
    delete tree;

    return 0;
}