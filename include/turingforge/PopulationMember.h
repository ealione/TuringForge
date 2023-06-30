#include <random>
#include <memory>
#include <cassert>

// Define a member of population by equation, score, and age
template <typename T, typename L>
struct PopMember {
    Node<T> tree;
    L score;
    L loss;
    int birth;
    int complexity;
    int ref;
    int parent;
};

template <typename T, typename L>
PopMember<T, L> make_PopMember(
        Node<T> t,
        L score,
        L loss,
        Options options,
        std::optional<int> complexity = std::nullopt,
        int ref = -1,
        int parent = -1,
        bool deterministic = false
) {
    if (ref == -1) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, std::numeric_limits<int>::max());
        ref = abs(dis(gen));
    }
    complexity = complexity.has_value() ? complexity.value() : -1;
    return PopMember<T, L>{
            std::move(t),
            std::move(score),
            std::move(loss),
            get_birth_order(deterministic),
            complexity.value(),
            ref,
            parent
    };
}

template <typename T, typename L>
PopMember<T, L> make_PopMember(
        const Dataset<T, L>& dataset,
        Node<T> t,
        Options options,
        std::optional<int> complexity = std::nullopt,
        int ref = -1,
        int parent = -1,
        std::optional<bool> deterministic = std::nullopt
) {
    int set_complexity = complexity.has_value() ? complexity.value() : compute_complexity(t, options);
    assert(set_complexity != -1);
    auto [score, loss] = score_func(dataset, t, options, set_complexity);
    return make_PopMember<T, L>(
            std::move(t),
            std::move(score),
            std::move(loss),
            std::move(options),
            set_complexity,
            ref,
            parent,
            deterministic.value_or(false)
    );
}

template <typename T, typename L>
PopMember<T, L> copy_pop_member(const PopMember<T, L>& p) {
    Node<T> tree = copy_node(p.tree);
    L score = p.score;
    L loss = p.loss;
    int birth = p.birth;
    int complexity = p.complexity;
    int ref = p.ref;
    int parent = p.parent;
    return PopMember<T, L>{
            std::move(tree),
            std::move(score),
            std::move(loss),
            birth,
            complexity,
            ref,
            parent
    };
}

template <typename T, typename L>
PopMember<T, L> copy_pop_member_reset_birth(const PopMember<T, L>& p, bool deterministic) {
    PopMember<T, L> new_member = copy_pop_member(p);
    new_member.birth = get_birth_order(deterministic);
    return new_member;
}

template <typename T, typename L>
int compute_complexity(const PopMember<T, L>& member, const Options& options) {
    int complexity = member.complexity;
    if (complexity == -1)
        return recompute_complexity!(member, options);
    // TODO: Turn this into a warning, and then return normal compute_complexity instead.
    return complexity;
}

template <typename T, typename L>
int recompute_complexity!(PopMember<T, L>& member, const Options& options) {
int complexity = compute_complexity(member.tree, options);
member.complexity = complexity;
return complexity;
}
