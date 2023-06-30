#pragma once

#include <vector>
#include <random>
#include <cmath>
#include <bitset>
#include <optional>
#include <algorithm>

#include "DynamicExpressions.hpp"
#include "CoreModule.hpp"

using namespace DynamicExpressions;
using namespace CoreModule;

// Return a random node from the tree
template<typename T>
Node <T> *random_node(Node <T> *tree) {
    if (tree->degree == 0)
        return tree;

    std::size_t b = 0;
    std::size_t c = 0;

    if (tree->degree >= 1)
        b = count_nodes(tree->l);

    if (tree->degree == 2)
        c = count_nodes(tree->r);

    std::size_t i = std::rand() % (1 + b + c);

    if (i <= b)
        return random_node(tree->l);
    else if (i == b + 1)
        return tree;

    return random_node(tree->r);
}

// Randomly convert an operator into another one (binary->binary; unary->unary)
template<typename T>
Node <T> *mutate_operator(Node <T> *tree, const Options &options) {
    if (!has_operators(tree))
        return tree;

    Node <T> *node = random_node(tree);

    while (node->degree == 0)
        node = random_node(tree);

    if (node->degree == 1)
        node->op = std::rand() % options.nuna + 1;
    else
        node->op = std::rand() % options.nbin + 1;

    return tree;
}

// Randomly perturb a constant
template<typename T>
Node <T> *mutate_constant(Node <T> *tree, T temperature, const Options &options) {
    if (!has_constants(tree))
        return tree;

    Node <T> *node = random_node(tree);

    while (node->degree != 0 || !node->constant)
        node = random_node(tree);

    T bottom = static_cast<T>(1) / static_cast<T>(10);
    T maxChange = options.perturbation_factor * temperature + 1 + bottom;
    T factor = std::pow(maxChange, static_cast<T>(std::rand()) / RAND_MAX);
    bool makeConstBigger = std::rand() > RAND_MAX / 2;

    if (makeConstBigger)
        node->val *= factor;
    else
        node->val /= factor;

    if (std::rand() > options.probability_negate_constant)
        node->val *= -1;

    return tree;
}

// Add a random unary/binary operation to the end of a tree
template<typename T>
Node <T> *append_random_op(Node <T> *tree, const Options &options, int nfeatures,
                           std::optional<bool> makeNewBinOp = std::nullopt) {
    Node <T> *node = random_node(tree);

    while (node->degree != 0)
        node = random_node(tree);

    if (!makeNewBinOp.has_value()) {
        float choice = static_cast<float>(std::rand()) / RAND_MAX;
        makeNewBinOp = choice < static_cast<float>(options.nbin) / (options.nuna + options.nbin);
    }

    if (makeNewBinOp.value()) {
        Node <T> *newnode = new Node<T>(
                std::rand() % options.nbin + 1,
                make_random_leaf<T>(nfeatures),
                make_random_leaf<T>(nfeatures)
        );

        set_node(node, newnode);
    } else {
        Node <T> *newnode = new Node<T>(
                std::rand() % options.nuna + 1,
                make_random_leaf<T>(nfeatures)
        );

        set_node(node, newnode);
    }

    return tree;
}

// Insert random node
template<typename T>
Node <T> *insert_random_op(Node <T> *tree, const Options &options, int nfeatures) {
    Node <T> *node = random_node(tree);
    float choice = static_cast<float>(std::rand()) / RAND_MAX;
    bool makeNewBinOp = choice < static_cast<float>(options.nbin) / (options.nuna + options.nbin);
    Node <T> *left = copy_node(node);

    if (makeNewBinOp) {
        Node <T> *right = make_random_leaf<T>(nfeatures);
        Node <T> *newnode = new Node<T>(
                std::rand() % options.nbin + 1,
                left,
                right
        );

        set_node(node, newnode);
    } else {
        Node <T> *newnode = new Node<T>(
                std::rand() % options.nuna + 1,
                left
        );

        set_node(node, newnode);
    }

    return tree;
}

// Add random node to the top of a tree
template<typename T>
Node <T> *prepend_random_op(Node <T> *tree, const Options &options, int nfeatures) {
    Node <T> *node = tree;
    float choice = static_cast<float>(std::rand()) / RAND_MAX;
    bool makeNewBinOp = choice < static_cast<float>(options.nbin) / (options.nuna + options.nbin);
    Node <T> *left = copy_node(tree);

    if (makeNewBinOp) {
        Node <T> *right = make_random_leaf<T>(nfeatures);
        Node <T> *newnode = new Node<T>(
                std::rand() % options.nbin + 1,
                left,
                right
        );

        set_node(node, newnode);
    } else {
        Node <T> *newnode = new Node<T>(
                std::rand() % options.nuna + 1,
                left
        );

        set_node(node, newnode);
    }

    return node;
}

template<typename T>
Node <T> *make_random_leaf(int nfeatures) {
    if (std::rand() > RAND_MAX / 2)
        return new Node<T>(randn<T>());
    else
        return new Node<T>(rand() % nfeatures + 1);
}

// Return a random node from the tree with parent, and side ('n' for no parent)
template<typename T>
std::tuple<Node < T>*, Node <T>*, char>
random_node_and_parent(Node<T>
* tree,
Node <T> *parent = nullptr,
char side = 'n'
)
{
if (tree->degree == 0)
return
std::make_tuple(tree, parent, side
);

std::size_t b = 0;
std::size_t c = 0;

if (tree->degree >= 1)
b = count_nodes(tree->l);

if (tree->degree == 2)
c = count_nodes(tree->r);

std::size_t i = std::rand() % (1 + b + c);

if (i <= b)
return
random_node_and_parent(tree
->l, tree, 'l');
else if (i == b + 1)
return
std::make_tuple(tree, parent, side
);

return
random_node_and_parent(tree
->r, tree, 'r');
}

template<typename T>
std::tuple<Node < T>*, Node <T>*, char>
random_node_and_parent(Node<T>
* tree)
{
return
random_node_and_parent(tree, nullptr,
'n');
}

// Select a random node, and replace it and the subtree
// with a variable or constant
template<typename T>
Node <T> *delete_random_op(Node < T > *tree,
const Options &options,
int nfeatures
)
{
Node <T> *node;
Node <T> *parent;
char side;
std::tie(node, parent, side
) =
random_node_and_parent(tree);
bool isroot = (parent == nullptr);

if (node->degree == 0)
{
// Replace with new constant
Node <T> *newnode = make_random_leaf<T>(nfeatures);
set_node(node, newnode
);
}
else if (node->degree == 1)
{
// Join one of the children with the parent
if (isroot)
return node->
l;
else if (parent->l == node)
parent->
l = node->l;
else
parent->
r = node->l;
}
else
{
// Join one of the children with the parent
if (

std::rand()

< RAND_MAX / 2)
{
if (isroot)
return node->
l;
else if (parent->l == node)
parent->
l = node->l;
else
parent->
r = node->l;
}
else
{
if (isroot)
return node->
r;
else if (parent->l == node)
parent->
l = node->r;
else
parent->
r = node->r;
}
}

return
tree;
}

// Create a random equation by appending random operators
template<typename T>
Node <T> *gen_random_tree(int length, const Options &options, int nfeatures) {
    // Note that this base tree is just a placeholder; it will be replaced.
    Node<T> * tree = new Node<T>(T(1));

    for (int i = 0; i < length; ++i) {
        // TODO: This can be larger number of nodes than length.
        tree = append_random_op(tree, options, nfeatures);
    }

    return tree;
}

template<typename T>
Node <T> *gen_random_tree_fixed_size(int node_count, const Options &options, int nfeatures) {
    Node<T> * tree = make_random_leaf<T>(nfeatures);
    int cur_size = count_nodes(tree);

    while (cur_size < node_count) {
        if (cur_size == node_count - 1)  // only unary operator allowed.
        {
            if (options.nuna == 0)
                break; // We will go over the requested amount, so we must break.
            tree = append_random_op(tree, options, nfeatures, false);
        } else {
            tree = append_random_op(tree, options, nfeatures);
        }

        cur_size = count_nodes(tree);
    }

    return tree;
}

template<typename T>
std::pair<Node < T>*, Node <T>*>
crossover_trees(Node<T>
* tree1,
Node <T> *tree2
)
{
tree1 = copy_node(tree1);
tree2 = copy_node(tree2);

Node <T> *node1;
Node <T> *parent1;
char side1;
std::tie(node1, parent1, side1
) =
random_node_and_parent(tree1);

Node <T> *node2;
Node <T> *parent2;
char side2;
std::tie(node2, parent2, side2
) =
random_node_and_parent(tree2);

bool isroot1 = (parent1 == nullptr);
bool isroot2 = (parent2 == nullptr);

if (isroot1)
tree1 = node2;
else if (parent1->l == node1)
parent1->
l = node2;
else
parent1->
r = node2;

if (isroot2)
tree2 = node1;
else if (parent2->l == node2)
parent2->
l = node1;
else
parent2->
r = node1;

    return std::make_pair(tree1, tree2);
}

template<typename T>
Node <T> *mutate_tree(Node < T > *tree,
const Options &options,
int nfeatures, T
temperature)
{
float choice = static_cast<float>(std::rand()) / RAND_MAX;

if (choice<options.probability_mutate_operator)
return
mutate_operator(tree, options
);
else if (choice<options.probability_mutate_operator + options.probability_mutate_constant)
return
mutate_constant(tree, temperature, options
);
else
return
tree;
}
