#include <cmath>
#include <algorithm>
#include <random>

void condition_mutation_weights(MutationWeights &weights, PopMember &member, Options &options, int curmaxsize) {
    if (member.tree.degree == 0) {
        weights.mutate_operator = 0.0;
        weights.delete_node = 0.0;
        weights.simplify = 0.0;
        if (!member.tree.constant) {
            weights.optimize = 0.0;
            weights.mutate_constant = 0.0;
        }
        return;
    }

    int n_constants = count_constants(member.tree);
    weights.mutate_constant *= std::min(8, n_constants) / 8.0;
    int complexity = compute_complexity(member, options);

    if (complexity >= curmaxsize) {
        weights.add_node = 0.0;
        weights.insert_node = 0.0;
    }

    if (!options.should_simplify) {
        weights.simplify = 0.0;
    }

    return;
}

template<typename T, typename L>
std::tuple<PopMember<T, L>, bool, double> next_generation(Dataset <T, L> &dataset,
                                                          PopMember <T, L> &member,
                                                          double temperature,
                                                          int curmaxsize,
                                                          RunningSearchStatistics &running_search_statistics,
                                                          Options &options,
                                                          RecordType tmp_recorder) {
    auto parent_ref = member.ref;
    bool mutation_accepted = false;
    double num_evals = 0.0;
    int nfeatures = dataset.nfeatures;
    auto weights = options.mutation_weights;
    auto [beforeScore, beforeLoss] = options.batching ? std::make_tuple(num_evals += (options.batch_size / dataset.n), score_func_batch(dataset, member, options))
                                                      : std::make_tuple(member.score, member.loss);

    condition_mutation_weights(weights, member, options, curmaxsize);

    auto mutation_choice = sample_mutation(weights);
    bool successful_mutation = false;
    bool is_success_always_possible = true;
    int attempts = 0;
    int max_attempts = 10;

    while (!successful_mutation && attempts < max_attempts) {
        tree = copy_node(member.tree);
        successful_mutation = true;
        if (mutation_choice == "mutate_constant") {
            tree = mutate_constant(tree, temperature, options);
            tmp_recorder["type"] = "constant";
            is_success_always_possible = true;
            // Mutating a constant shouldn't invalidate an already-valid function
        } else if (mutation_choice == "mutate_operator") {
            tree = mutate_operator(tree, options);
            tmp_recorder["type"] = "operator";
            is_success_always_possible = true;
            // Can always mutate to the same operator
        } else if (mutation_choice == "add_node") {
            if (rand() < 0.5) {
                tree = append_random_op(tree, options, nfeatures);
                tmp_recorder["type"] = "append_op";
            } else {
                tree = prepend_random_op(tree, options, nfeatures);
                tmp_recorder["type"] = "prepend_op";
            }
            is_success_always_possible = false;
            // Can potentially have a situation without success
        } else if (mutation_choice == "insert_node") {
            tree = insert_random_op(tree, options, nfeatures);
            tmp_recorder["type"] = "insert_op";
            is_success_always_possible = false;
        } else if (mutation_choice == "delete_node") {
            tree = delete_random_op(tree, options, nfeatures);
            tmp_recorder["type"] = "delete_op";
            is_success_always_possible = true;
        } else if (mutation_choice == "simplify") {
            assert(options.should_simplify);
            tree = simplify_tree(tree, options.operators);
            tree = combine_operators(tree, options.operators);
            tmp_recorder["type"] = "partial_simplify";
            mutation_accepted = true;
            return std::make_tuple(
                    PopMember(
                            tree,
                            beforeScore,
                            beforeLoss,
                            options,
                            parent_ref,
                            options.deterministic),
                    mutation_accepted,
                    num_evals);
            is_success_always_possible = true;
        } else if (mutation_choice == "randomize") {
            tree_size_to_generate = rand(1:curmaxsize);
            tree = gen_random_tree_fixed_size(tree_size_to_generate, options, nfeatures, T);
            tmp_recorder["type"] = "regenerate";
            is_success_always_possible = true;
        } else if (mutation_choice == "optimize") {
            cur_member = PopMember(
                    tree,
                    beforeScore,
                    beforeLoss,
                    options,
                    compute_complexity(member, options),
                    parent_ref,
                    options.deterministic);
            auto [new_member, new_num_evals] = optimize_constants(dataset, cur_member, options);
            num_evals += new_num_evals;
            tmp_recorder["type"] = "optimize";
            mutation_accepted = true;
            return std::make_tuple(new_member, mutation_accepted, num_evals);
            is_success_always_possible = true;
        } else if (mutation_choice == "do_nothing") {
            tmp_recorder["type"] = "identity";
            tmp_recorder["result"] = "accept";
            tmp_recorder["reason"] = "identity";
            mutation_accepted = true;
            return std::make_tuple(
                    PopMember(
                            tree,
                            beforeScore,
                            beforeLoss,
                            options,
                            compute_complexity(member, options),
                            parent_ref,
                            options.deterministic),
                    mutation_accepted,
                    num_evals);
        } else {
            error("Unknown mutation choice: $mutation_choice");
        }

        successful_mutation = successful_mutation && check_constraints(tree, options, curmaxsize);
        attempts += 1;
    }

    if (!successful_mutation) {
        tmp_recorder["result"] = "reject";
        tmp_recorder["reason"] = "failed_constraint_check";
        mutation_accepted = false;
        return std::make_tuple(
                PopMember(
                        copy_node(member.tree),
                        beforeScore,
                        beforeLoss,
                        options,
                        compute_complexity(member, options),
                        parent_ref,
                        options.deterministic),
                mutation_accepted,
                num_evals);
    }

    if (options.batching) {
        afterScore, afterLoss = score_func_batch(dataset, tree, options);
        num_evals += (options.batch_size / dataset.n);
    } else {
        afterScore, afterLoss = score_func(dataset, tree, options);
        num_evals += 1;
    }

    if (std::isnan(afterScore)) {
        tmp_recorder["result"] = "reject";
        tmp_recorder["reason"] = "nan_loss";
        mutation_accepted = false;
        return std::make_tuple(
                PopMember(
                        copy_node(member.tree),
                        beforeScore,
                        beforeLoss,
                        options,
                        compute_complexity(member, options),
                        parent_ref,
                        options.deterministic),
                mutation_accepted,
                num_evals);
    }

    double probChange = 1.0;
    if (options.annealing) {
        double delta = afterScore - beforeScore;
        probChange *= exp(-delta / (temperature * options.alpha));
    }
    int newSize = -1;
    if (options.use_frequency) {
        int oldSize = compute_complexity(member, options);
        newSize = compute_complexity(tree, options);
        double old_frequency = (0 < oldSize && oldSize <= options.maxsize)
                               ? running_search_statistics.normalized_frequencies[oldSize] : 1e-6;
        double new_frequency = (0 < newSize && newSize <= options.maxsize)
                               ? running_search_statistics.normalized_frequencies[newSize] : 1e-6;
        probChange *= old_frequency / new_frequency;
    }

    if (probChange < rand()) {
        @recorder begin
        tmp_recorder["result"] = "reject";
        tmp_recorder["reason"] = "annealing_or_frequency";
        end
                mutation_accepted = false;
        return (
                PopMember(
                        copy_node(member.tree),
                        beforeScore,
                        beforeLoss,
                        options,
                        compute_complexity(member, options),
                        parent_ref,
                        options.deterministic),
                        mutation_accepted,
                        num_evals);
    } else {
        {
            tmp_recorder["result"] = "accept";
            tmp_recorder["reason"] = "pass";
        }
        mutation_accepted = true;
        return (
                PopMember(
                        tree,
                        afterScore,
                        afterLoss,
                        options,
                        newSize,
                        parent_ref,
                        options.deterministic),
                        mutation_accepted,
                        num_evals);
    }
}

template<typename T, typename L>
std::tuple<PopMember<T, L>, PopMember<T, L>, bool, double> crossover_generation(
        PopMember <T, L> &member1,
        PopMember <T, L> &member2,
        Dataset <T, L> &dataset,
        int curmaxsize,
        Options &options
) {
    auto tree1 = member1.tree;
    auto tree2 = member2.tree;
    bool crossover_accepted = false;

    // We breed these until constraints are no longer violated:
    auto [child_tree1, child_tree2] = crossover_trees(tree1, tree2);
    int num_tries = 1;
    int max_tries = 10;
    double num_evals = 0.0;
    int afterSize1 = -1;
    int afterSize2 = -1;
    while (true) {
        afterSize1 = compute_complexity(child_tree1, options);
        afterSize2 = compute_complexity(child_tree2, options);
        // Both trees satisfy constraints
        if (check_constraints(child_tree1, options, curmaxsize, afterSize1) &&
            check_constraints(child_tree2, options, curmaxsize, afterSize2)) {
            break;
        }
        if (num_tries > max_tries) {
            crossover_accepted = false;
            return std::make_tuple(member1, member2, crossover_accepted, num_evals);  // Fail.
        }
        std::tie(child_tree1, child_tree2) = crossover_trees(tree1, tree2);
        num_tries += 1;
    }
    if (options.batching) {
        auto [afterScore1, afterLoss1] = score_func_batch(
                dataset,
                child_tree1,
                options,
                afterSize1
        );
        auto [afterScore2, afterLoss2] = score_func_batch(
                dataset,
                child_tree2,
                options,
                afterSize2
        );
        num_evals += 2 * (options.batch_size / dataset.n);
    } else {
        auto [afterScore1, afterLoss1] = score_func(
                dataset,
                child_tree1,
                options,
                afterSize1
        );
        auto [afterScore2, afterLoss2] = score_func(
                dataset,
                child_tree2,
                options,
                afterSize2
        );
        num_evals += options.batch_size / dataset.n;
    }

    auto baby1 = PopMember<T, L>(
            child_tree1,
            afterScore1,
            afterLoss1,
            options,
            afterSize1,
            member1.ref,
            options.deterministic
    );
    auto baby2 = PopMember<T, L>(
            child_tree2,
            afterScore2,
            afterLoss2,
            options,
            afterSize2,
            member2.ref,
            options.deterministic
    );

    crossover_accepted = true;
    return std::make_tuple(baby1, baby2, crossover_accepted, num_evals);
}