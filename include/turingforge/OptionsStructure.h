#pragma once

#include <iostream>
#include <vector>
#include <tuple>
#include <string>
#include <cmath>
#include <algorithm>
#include <random>
#include <optional>

#include "turingforge/OperatorEnum.h"
#include "turingforge/Loss/LossFunctions.h"
#include "turingforge/Optim.h"

namespace OptionsStructModule {
    enum MutationMember {
        MUTATE_CONSTANT,
        MUTATE_OPERATOR,
        ADD_NODE,
        INSERT_NODE,
        DELETE_NODE,
        SIMPLIFY,
        RANDOMIZE,
        DO_NOTHING,
        OPTIMIZE
    };

    struct MutationWeights {
        double mutate_constant = 0.048;
        double mutate_operator = 0.47;
        double add_node = 0.79;
        double insert_node = 5.1;
        double delete_node = 1.7;
        double simplify = 0.0020;
        double randomize = 0.00023;
        double do_nothing = 0.21;
        double optimize = 0.0;

        explicit operator std::vector<double>() const {
            return { mutate_constant, optimize, add_node, insert_node, delete_node, simplify, randomize, do_nothing, optimize };
        }

        MutationWeights copy() {
            return MutationWeights { mutate_constant, mutate_operator, add_node, insert_node, delete_node, simplify, randomize, do_nothing, optimize };
        }
    };

    MutationMember sampleMutationMember(const MutationWeights& weights) {
        auto weight_vector = (const std::vector<double> &) weights;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::discrete_distribution<> d(weight_vector.begin(), weight_vector.end());

        return static_cast<MutationMember>(d(gen));
    }

    template <typename T>
    struct ComplexityMapping {
        bool use;                           // Whether we use custom complexity, or just use 1 for everything.
        std::vector<T> binop_complexities;  // Complexity of each binary operator.
        std::vector<T> unaop_complexities;  // Complexity of each unary operator.
        T variable_complexity;              // Complexity of using a variable.
        T constant_complexity;              // Complexity of using a constant.

        auto eltype(const ComplexityMapping<T>&) -> T {
            return T{};
        }

        template <typename T1, typename T2, typename T3, typename T4>
        auto from_types(
                const std::vector<T1>& binop_complexities_,
                const std::vector<T2>& unaop_complexities_,
                const T3& variable_complexity_,
                const T4& constant_complexity_
        ) -> ComplexityMapping<std::common_type_t<T1, T2, T3, T4>> {
            using promoted_T = std::common_type_t<T1, T2, T3, T4>;
            return ComplexityMapping<promoted_T>{
                    true,
                    binop_complexities_,
                    unaop_complexities_,
                    variable_complexity_,
                    constant_complexity_
            };
        }
    };

    template <typename T>
    struct Options {
        using CT = T;
        using OPT = Optim::Options<>;
        using EL = typename std::conditional<std::is_same<T, SupervisedLoss>::value, SupervisedLoss, std::function<T>>::type;
        using FL = typename std::conditional<std::is_same<T, std::nullptr_t>::value, std::nullptr_t, std::function<T>>::type;
        using W = std::tuple<int, float>; // Placeholder for W

        GenericOperatorEnum operators;
        std::vector<std::tuple<int, int>> bin_constraints;
        std::vector<int> una_constraints;
        ComplexityMapping<T> complexity_mapping;
        int tournament_selection_n{};
        float tournament_selection_p{};
        W tournament_selection_weights;
        float parsimony{};
        float alpha{};
        int maxsize{};
        int maxdepth{};
        bool fast_cycle{};
        bool turbo{};
        bool migration{};
        bool hof_migration{};
        bool should_simplify{};
        bool should_optimize_constants{};
        std::string output_file;
        int npopulations{};
        float perturbation_factor{};
        bool annealing{};
        bool batching{};
        int batch_size{};
        MutationWeights mutation_weights;
        float crossover_probability{};
        float warmup_maxsize_by{};
        bool use_frequency{};
        bool use_frequency_in_tournament{};
        double adaptive_parsimony_scaling{};
        int npop{};
        int ncycles_per_iteration{};
        float fraction_replaced{};
        float fraction_replaced_hof{};
        int topn{};
        int verbosity{};
        bool save_to_file{};
        float probability_negate_constant{};
        int nuna{};
        int nbin{};
        std::optional<int> seed;
        EL elementwise_loss;
        FL loss_function;
        bool progress{};
        std::optional<int> terminal_width;
        std::string optimizer_algorithm;
        float optimizer_probability{};
        int optimizer_nrestarts{};
        typename Optim::Options<> optimizer_options;
        bool recorder{};
        std::string recorder_file;
        float prob_pick_first{};
        FL early_stop_condition;
        bool return_state{};
        std::optional<double> timeout_in_seconds;
        std::optional<int> max_evals;
        bool skip_mutation_failures{};
        std::optional<std::vector<std::tuple<int, int, std::vector<std::tuple<int, int, int>>>>> nested_constraints;
        bool deterministic{};
        bool define_helper_functions{};

        void print(std::ostream& os) {
            os << "Options(\n"
               << "    # Operators:\n"
               << "        binops=" << operators.print(os, "binops") << ", unaops=" << operators.print(os, "unaops") << ",\n"
               << "    # Loss:\n"
               << "        loss=" << elementwise_loss << ",\n"
               << "    # Complexity Management:\n"
               << "        maxsize=" << maxsize << ", maxdepth=" << maxdepth << ", bin_constraints=" << bin_constraints
               << ", una_constraints=" << una_constraints << ", use_frequency=" << use_frequency
               << ", use_frequency_in_tournament=" << use_frequency_in_tournament << ", parsimony=" << parsimony
               << ", warmup_maxsize_by=" << warmup_maxsize_by << ",\n"
               << "    # Search Size:\n"
               << "        npopulations=" << npopulations << ", ncycles_per_iteration=" << ncycles_per_iteration << ", npop=" << npop << ",\n"
               << "    # Migration:\n"
               << "        migration=" << migration << ", hof_migration=" << hof_migration << ", fraction_replaced=" << fraction_replaced
               << ", fraction_replaced_hof=" << fraction_replaced_hof << ",\n"
               << "    # Tournaments:\n"
               << "        prob_pick_first=" << prob_pick_first << ", tournament_selection_n=" << tournament_selection_n << ", topn=" << topn << ",\n"
               << "    # Constant tuning:\n"
               << "        perturbation_factor=" << perturbation_factor << ", probability_negate_constant=" << probability_negate_constant
               << ", should_optimize_constants=" << should_optimize_constants << ", optimizer_algorithm=" << optimizer_algorithm
               << ", optimizer_probability=" << optimizer_probability << ", optimizer_nrestarts=" << optimizer_nrestarts
               << ", optimizer_iterations=" << optimizer_options.iterations << ",\n"
               << "    # Mutations:\n"
               << "        mutation_weights=" << mutation_weights << ", crossover_probability=" << crossover_probability
               << ", skip_mutation_failures=" << skip_mutation_failures << ",\n"
               << "    # Annealing:\n"
               << "        annealing=" << annealing << ", alpha=" << alpha << ",\n"
               << "    # Speed Tweaks:\n"
               << "        batching=" << batching << ", batch_size=" << batch_size << ", fast_cycle=" << fast_cycle << ",\n"
               << "    # Logistics:\n"
               << "        output_file=" << output_file << ", verbosity=" << verbosity << ", seed=" << seed << ", progress=" << progress << ",\n"
               << "    # Early Exit:\n"
               << "        early_stop_condition=" << early_stop_condition << ", timeout_in_seconds=" << timeout_in_seconds << ",\n"
               << ")";
        }

        std::ostream& operator << (std::ostream& os) {
            print(os);
            return os;
        }
    };
}