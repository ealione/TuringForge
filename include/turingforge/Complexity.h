#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>

struct Node {
    bool constant;
    int degree;
    std::string op;
};

struct Options {
    struct ComplexityMapping {
        bool use;
        int constant_complexity;
        int variable_complexity;
        std::map<std::string, int> unaop_complexities;
        std::map<std::string, int> binop_complexities;
    } complexity_mapping;
};

template<typename CT>
bool past_complexity_limit(Node tree, Options options, CT limit) {
    return compute_complexity(tree, options) > limit;
}

template<typename CT>
int compute_complexity(Node tree, Options options) {
    if (options.complexity_mapping.use) {
        auto raw_complexity = _compute_complexity(tree, options);
        return static_cast<int>(std::round(raw_complexity));
    } else {
        return count_nodes(tree);
    }
}

template<typename CT>
CT _compute_complexity(Node tree, Options options) {
    auto cmap = options.complexity_mapping;
    auto constant_complexity = cmap.constant_complexity;
    auto variable_complexity = cmap.variable_complexity;
    auto unaop_complexities = cmap.unaop_complexities;
    auto binop_complexities = cmap.binop_complexities;

    return tree_mapreduce(
            t -> t.constant ? constant_complexity : variable_complexity,
            t -> t.degree == 1 ? unaop_complexities[t.op] : binop_complexities[t.op],
            +,
            tree,
            CT
    );
}