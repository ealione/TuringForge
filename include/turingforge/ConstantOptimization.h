#pragma once

#include <vector>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <random>

// Proxy function for optimization
template <typename T, typename L>
L opt_func(const std::vector<T>& x, const Dataset<T, L>& dataset, Node<T>* tree, const std::vector<Node<T>*>& constant_nodes, const Options& options)
{
_set_constants(x, constant_nodes);
// TODO: This should use score_func batching.
L loss = eval_loss(tree, dataset, options);
return loss;
}

template <typename T>
void _set_constants(const std::vector<T>& x, const std::vector<Node<T>*>& constant_nodes)
{
for (std::size_t i = 0; i < x.size(); ++i)
{
constant_nodes[i]->val = x[i];
}
}

// Use Nelder-Mead to optimize the constants in an equation
template <typename T, typename L>
std::pair<PopMember<T, L>, double> optimize_constants(const Dataset<T, L>& dataset, const PopMember<T, L>& member, const Options& options)
{
    int nconst = count_constants(member.tree);
    if (nconst == 0)
        return std::make_pair(member, 0.0);
    if constexpr (std::is_same_v<T, std::complex<T>>)
    {
        // TODO: Make this more general. Also, do we even need Newton here at all??
        Optim::Algorithm algorithm(Optim::BFGS{}, LineSearches::BackTracking{});
        return _optimize_constants(dataset, member, options, algorithm, options.optimizer_options);
    }
    else if (nconst == 1)
    {
        Optim::Algorithm algorithm(Optim::Newton{}, LineSearches::BackTracking{});
        return _optimize_constants(dataset, member, options, algorithm, options.optimizer_options);
    }
    else
    {
        if (options.optimizer_algorithm == "NelderMead")
        {
            Optim::Algorithm algorithm(Optim::NelderMead{}, LineSearches::BackTracking{});
            return _optimize_constants(dataset, member, options, algorithm, options.optimizer_options);
        }
        else if (options.optimizer_algorithm == "BFGS")
        {
            Optim::Algorithm algorithm(Optim::BFGS{}, LineSearches::BackTracking{});
            return _optimize_constants(dataset, member, options, algorithm, options.optimizer_options);
        }
        else
        {
            throw std::runtime_error("Optimization function not implemented.");
        }
    }
}

template <typename T, typename L>
std::pair<PopMember<T, L>, double> _optimize_constants(const Dataset<T, L>& dataset, const PopMember<T, L>& member, const Options& options, const Optim::Algorithm& algorithm, const Optim::OptimizerOptions& optimizer_options)
{
    Node<T>* tree = member.tree;
    std::vector<Node<T>*> constant_nodes;
    for (Node<T>* node : tree)
    {
        if (node->degree == 0 && node->constant)
            constant_nodes.push_back(node);
    }
    std::vector<T> x0(constant_nodes.size());
    for (std::size_t i = 0; i < constant_nodes.size(); ++i)
    {
        x0[i] = constant_nodes[i]->val;
    }
    auto f = [&](const std::vector<T>& x) { return opt_func(x, dataset, tree, constant_nodes, options); };
    auto result = Optim::optimize(f, x0, algorithm, optimizer_options);
    double num_evals = result.f_calls;
    // Try other initial conditions:
    for (int i = 1; i <= options.optimizer_nrestarts; ++i)
    {
        std::vector<T> new_start(x0.size());
        std::transform(x0.begin(), x0.end(), new_start.begin(), [&](const T& val) { return val * (T(1) + T(1 / 2.0) * std::normal_distribution<T>()(std::mt19937(std::random_device{}()))); });
        auto tmpresult = Optim::optimize(f, new_start, algorithm, optimizer_options);
        num_evals += tmpresult.f_calls;

        if (tmpresult.minimum < result.minimum)
        {
            result = tmpresult;
        }
    }

    if (Optim::converged(result))
    {
        _set_constants(result.minimizer, constant_nodes);
        auto [score, loss] = score_func(dataset, member, options);
        num_evals += 1.0;
        member.birth = get_birth_order(options.deterministic);
    }
    else
    {
        _set_constants(x0, constant_nodes);
    }

    return std::make_pair(member, num_evals);
}