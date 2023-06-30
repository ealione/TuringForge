#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>

template<typename CT>
bool flag_bin_operator_complexity(Node tree, std::string op, std::vector<CT> cons, Options options) {
    return any_of(tree.begin(), tree.end(), [&](Node subtree) {
        if (subtree.degree == 2 && subtree.op == op) {
            if (cons[0] > -1 && past_complexity_limit(subtree.l, options, cons[0])) {
                return true;
            }
            if (cons[1] > -1 && past_complexity_limit(subtree.r, options, cons[1])) {
                return true;
            }
        }
        return false;
    });
}

template<typename CT>
bool flag_una_operator_complexity(Node tree, std::string op, CT cons, Options options) {
    return any_of(tree.begin(), tree.end(), [&](Node subtree) {
        if (subtree.degree == 1 && tree.op == op) {
            return past_complexity_limit(subtree.l, options, cons);
        }
        return false;
    });
}

template<typename CT>
int count_max_nestedness(Node tree, int degree, std::string op) {
    auto nestedness = tree_mapreduce(
            [](auto t) { return 0; },  // Leafs
            [&](auto t) { return (t.degree == degree && t.op == op) ? 1 : 0; },  // Branches
            [](auto p, auto... c) { return p + std::max(c...); },  // Reduce
            tree,
    );
    // Remove count of self:
    auto is_self = tree.degree == degree && tree.op == op;
    return nestedness - (is_self ? 1 : 0);
}

template<typename CT>
bool flag_una_operator_complexity(Node tree, std::string op, CT cons, Options options) {
    return any_of(tree.begin(), tree.end(), [&](Node subtree) {
        if (subtree.degree == 1 && tree.op == op) {
            return past_complexity_limit(subtree.l, options, cons);
        }
        return false;
    });
}

template<typename CT>
int count_max_nestedness(Node tree, int degree, std::string op) {
    auto nestedness = tree_mapreduce(
            [](auto t) { return 0; },  // Leafs
            [&](auto t) { return (t.degree == degree && t.op == op) ? 1 : 0; },  // Branches
            [](auto p, auto... c) { return p + std::max(c...); },  // Reduce
            tree,
    );
    // Remove count of self:
    auto is_self = tree.degree == degree && tree.op == op;
    return nestedness - (is_self ? 1 : 0);
}

bool flag_illegal_nests(Node tree, Options options) {
    if (options.nested_constraints.empty()) {
        return false;
    }
    for (auto [degree, op_idx, op_constraint] : options.nested_constraints) {
        for (auto [nested_degree, nested_op_idx, max_nestedness] : op_constraint) {
            if (auto subtree = find_if(tree.begin(), tree.end(), [&](Node subtree) {
                    if (subtree.degree == degree && subtree.op == op_idx) {
                        auto nestedness = count_max_nestedness(subtree.l, nested_degree, nested_op_idx);
                        return nestedness > max_nestedness;
                    }
                    return false;
                }); subtree != tree.end()) {
                return true;
            }
        }
    }
    return false;
}

bool check_constraints(Node tree, Options options, int maxsize, optional<int> cursize = nullopt) {
    if ((cursize ? *cursize : compute_complexity(tree, options)) > maxsize) {
        return false;
    }
    if (count_depth(tree) > options.maxdepth) {
        return false;
    }
    for (int i = 0; i < options.nbin; ++i) {
        auto [cons1, cons2] = options.bin_constraints[i];
        if (cons1 == -1 || cons2 == -1) {
            continue;
        }
        if (flag_bin_operator_complexity(tree, i, {cons1, cons2}, options)) {
            return false;
        }
    }
    for (int i = 0; i < options.nuna; ++i) {
        auto cons = options.una_constraints[i];
        if (cons == -1) {
            continue;
        }
        if (flag_una_operator_complexity(tree, i, cons, options)) {
            return false;
        }
    }
    if (flag_illegal_nests(tree, options)) {
        return false;
    }
    return true;
}

bool check_constraints(Node tree, Options options) {
    return check_constraints(tree, options.maxsize);
}