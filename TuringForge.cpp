#include <variant>
#include <optional>
#include <vector>
#include <functional>
#include <stdexcept>
#include <typeinfo>
#include <complex>
#include <variant>
#include <functional>
#include <optional>
#include <thread>
#include <ranges>
#include <cassert>
#include <numeric>

#include "turingforge/Mutate.h"
//#include "turingforge/AdaptiveParsimony.h"

//template<typename T, typename L>
//void EquationSearch(
//        const AbstractMatrix <T> &X,
//        const AbstractMatrix <T> &y,
//        int niterations = 10,
//        const std::variant <AbstractMatrix<T>, AbstractVector<T>, std::monostate> &weights = std::monostate{},
//        const std::variant <std::vector<std::string>, std::monostate> &varMap = std::monostate{},
//        const Options &options = Options(),
//        const std::variant <ParallelismMode, std::monostate> &parallelism = ParallelismMode::Multithreading,
//        std::optional<int> numprocs = std::nullopt,
//        const std::variant <std::vector<int>, std::monostate> &procs = std::monostate{},
//        const std::variant <std::function<void(int, bool)>, std::monostate> &addprocs_function = std::monostate{},
//        bool runtests = true,
//        const std::optional <StateType<T, L>> &saved_state = std::nullopt,
//        const std::type_info &loss_type = typeid(void)
//) {
//    int nout = y.size(FEATURE_DIM);
//    std::variant <AbstractMatrix<T>, AbstractVector<T>, std::monostate> reshaped_weights = weights;
//    if (!std::holds_alternative<std::monostate>(reshaped_weights)) {
//        reshaped_weights = reshape(std::get < AbstractMatrix < T >> (reshaped_weights), y.size());
//    }
//
//    if (std::is_same_v < T, std::complex > && loss_type == typeid(void)) {
//        loss_type = typeid(T::value_type);
//    }
//
//    std::vector <Dataset<T, L>> datasets;
//    for (int j = 0; j < nout; j++) {
//        datasets.emplace_back(
//                X,
//                y(j, Range{});
//        std::holds_alternative<std::monostate>(reshaped_weights) ? std::monostate{} : std::get < AbstractVector < T
//                >> (reshaped_weights)(j, Range{}),
//                std::holds_alternative<std::monostate>(varMap) ? std::monostate{} : std::get < std::vector <std::string >> (varMap),
//                loss_type
//        );
//    }
//
//    return EquationSearch(
//            datasets,
//            niterations,
//            options,
//            std::holds_alternative<std::monostate>(parallelism) ? ParallelismMode::Multithreading
//                                                                : std::get<ParallelismMode>(parallelism),
//            numprocs,
//            std::holds_alternative<std::monostate>(procs) ? std::nullopt : std::get < std::vector < int >> (procs),
//            std::holds_alternative<std::monostate>(addprocs_function) ? std::nullopt : std::get < std::function <void(int, bool)>> (addprocs_function),
//            runtests,
//            saved_state
//    );
//}
//
//template <typename T1, typename T2>
//auto EquationSearch(const AbstractMatrix<T1>& X, const AbstractVector<T2>& y, const auto&... kw)
//-> std::enable_if_t<std::is_same_v<T1, DATA_TYPE> && std::is_same_v<T2, DATA_TYPE>, decltype(EquationSearch(X, reshape(y, std::make_tuple(1, size(y, 1)))), kw...)>
//{
//    return EquationSearch(X, reshape(y, std::make_tuple(1, size(y, 1))), kw...);
//}
//
//EquationSearchResult EquationSearch(const Dataset& dataset, const auto&... kws)
//{
//    return EquationSearch(std::vector<Dataset>{dataset}, kws...);
//}
//
//template <typename T, typename L, typename D>
//EquationSearchResult EquationSearch(
//        const std::vector<D>& datasets,
//        int niterations = 10,
//        const Options& options = Options(),
//        const std::string& parallelism = "multithreading",
//        std::optional<int> numprocs = std::nullopt,
//        std::optional<std::vector<int>> procs = std::nullopt,
//        std::optional<std::function<void(int, bool)>> addprocs_function = std::nullopt,
//        bool runtests = true,
//        std::optional<StateType<T, L>> saved_state = std::nullopt
//)
//{
//    std::string concurrency;
//    if (parallelism == "multithreading" || parallelism == "multithreaded")
//        concurrency = "multithreading";
//    else if (parallelism == "multiprocessing")
//        concurrency = "multiprocessing";
//    else if (parallelism == "serial")
//        concurrency = "serial";
//    else
//        throw std::invalid_argument("Invalid parallelism mode: " + parallelism +
//        ". You must choose one of multithreading, multiprocessing, or serial.");
//
//    bool not_distributed = (concurrency == "multithreading" || concurrency == "serial");
//    if (not_distributed && procs.has_value())
//        throw std::invalid_argument("`procs` should not be set when using parallelism " + concurrency +
//        ". Please use `multiprocessing`.");
//    if (not_distributed && numprocs.has_value())
//        throw std::invalid_argument("`numprocs` should not be set when using parallelism " + concurrency +
//        ". Please use `multiprocessing`.");
//
//    return _EquationSearch(concurrency, datasets, niterations, options, numprocs, procs,
//            addprocs_function, runtests, saved_state);
//}
//
//template <typename T, typename L, typename D>
//void _EquationSearch(
//    Symbol parallelism,
//    std::vector<D> datasets,
//    unsigned int niterations,
//    Options options,
//    std::optional<int> numprocs,
//    std::optional<std::vector<int>> procs,
//    std::optional<std::function<void()>> addprocs_function,
//    bool runtests,
//    std::optional<std::variant<T, L>> saved_state
//) {
//    if (options.deterministic) {
//        if (parallelism != "serial") {
//            throw std::runtime_error("Determinism is only guaranteed for serial mode.");
//        }
//    }
//    if (parallelism == "multithreading") {
//        if (std::thread::hardware_concurrency() == 1) {
//            #warning "You are using multithreading mode, but only one thread is available.";
//        }
//    }
//
//    auto example_dataset = datasets[1];
//    auto nout = size(datasets, 1);
//
//    if (runtests) {
//        test_option_configuration(T, options);
//        // Testing the first output variable is the same:
//        test_dataset_configuration(example_dataset, options);
//    }
//
//    for (auto dataset : datasets) {
//        update_baseline_loss(dataset, options);
//    }
//
//    if (options.seed != std::nullptr) {
//        seed(options.seed);
//    }
//
//    // Start a population on every process
//    // Store the population, hall of fame
//    using allPopsType = std::tuple<Population, HallOfFame, RecordType, double>;
//
//    std::vector<allPopsType> allPops(nout);
//    std::vector<allPopsType> init_pops(nout);
//
//    // Set up a channel to send finished populations back to head node
//    if (parallelism == "multiprocessing" || parallelism == "multithreading") {
//        std::vector <std::vector<MPI_Request>> requests(nout);
//        std::vector <std::vector<Task>> tasks(nout);
//
//        for (auto i = 1; i <= options.npopulations; ++i) {
//            for (auto j = 1; j <= nout; ++j) {
//                MPI_Isend(&allPops[j][i], sizeof(allPopsType), MPI_BYTE, 0, 0, MPI_COMM_WORLD, &requests[j][i]);
//            }
//        }
//
//        for (auto j = 1; j <= nout; ++j) {
//            tasks[j] = Task[];
//        }
//    }
//
//    //////////////////////////////////////////////////////////////////////////////////
//
//    MPI::Init(argc, argv);
//
//    // This is a recorder for populations, but is not actually used for processing, just
//    // for the final return.
//    auto returnPops = init_dummy_pops(nout, options.npopulations, datasets, options);
//    // These initial populations are discarded:
//    auto bestSubPops = init_dummy_pops(nout, options.npopulations, datasets, options);
//
//    auto actualMaxsize = options.maxsize + MAX_DEGREE;
//
//    // TODO: Should really be one per population too.
//    std::vector<RunningSearchStatistics> all_running_search_statistics(nout);
//    for (auto i = 1; i <= nout; ++i) {
//        all_running_search_statistics[i] = RunningSearchStatistics(options);
//    }
//
//    std::vector<Int> curmaxsizes(nout);
//    auto record = RecordType("options" => "$(options)");
//
//    if (options.warmup_maxsize_by == 0.0f0) {
//        curmaxsizes = std::vector<int>(nout, options.maxsize);
//    }
//
//    // Records the number of evaluations:
//    // Real numbers indicate use of batching.
//    std::vector<std::vector<double>> num_evals(nout);
//    for (auto j = 1; j <= nout; ++j) {
//        num_evals[j] = std::vector<double>(options.npopulations, 0.0);
//    }
//
//    auto we_created_procs = false;
//
//    if (parallelism == "multiprocessing") {
//        if (addprocs_function == std::nullptr) {
//            addprocs_function = addprocs;
//        }
//        if (numprocs === std::nullptr && procs === std::nullptr) {
//            numprocs = 4;
//            procs = addprocs_function(numprocs; lazy=false);
//            we_created_procs = true;
//        } else if (numprocs === std::nullptr) {
//            numprocs = length(procs);
//        } else if (procs === nothing) {
//            procs = addprocs_function(numprocs; lazy=false);
//            we_created_procs = true;
//        }
//
//        if (we_created_procs) {
//            auto project_path = std::filesystem::path(Pkg.project().path).parent_path().string();
//            activate_env_on_workers(procs, project_path, options);
//            import_module_on_workers(procs, __FILE__, options);
//        }
//        move_functions_to_workers(procs, options, example_dataset);
//        if (runtests) {
//            test_module_on_workers(procs, options);
//        }
//
//        if (runtests) {
//            test_entire_pipeline(procs, example_dataset, options);
//        }
//    }
//
//    // Get the next worker process to give a job:
//    std::map<std::tuple<int, int>, int> worker_assignment;
//
//    auto hallOfFame = load_saved_hall_of_fame(saved_state);
//    if (hallOfFame === std::nullptr) {
//        hallOfFame = std::vector<HallOfFame<T, L>>(nout);
//        for (auto j = 1; j <= nout; ++j) {
//            hallOfFame[j] = HallOfFame(options, T, L);
//        }
//    } else {
//        // Recompute losses for the hall of fame, in
//        // case the dataset changed:
//        std::vector<HallOfFame<T, L>> hallOfFame;
//        for (auto [hof, dataset] : std::ranges::views::zip(hallOfFame, datasets)) {
//            for (auto member : hof.members[hof.exists]) {
//                auto [score, result_loss] = score_func(dataset, member, options);
//                member.score = score;
//                member.loss = result_loss;
//            }
//        }
//    }
//
//    assert(hallOfFame.size() == nout);
//    hallOfFame.clear();
//
//    for (auto j = 1; j <= nout; ++j) {
//        for (auto i = 1; i <= (options.npopulations); ++i) {
//            auto worker_idx = next_worker(worker_assignment, procs);
//            if (parallelism == "multiprocessing") {
//                worker_assignment[{j,i}] = worker_idx;
//            }
//
//            auto saved_pop = load_saved_population(saved_state; out=j, pop=i);
//
//            if (saved_pop != std::nullptr && saved_pop.members.size() == options.npop) {
//                saved_pop::Population<T,L>;
//                // Update losses:
//                for (auto member : saved_pop.members) {
//                    auto [score, result_loss] = score_func(datasets[j], member,options);
//                    member.score = score;
//                    member.loss = result_loss;
//                }
//                auto copy_pop = copy_population(saved_pop);
//                auto new_pop = @sr_spawner parallelism worker_idx (
//                        copy_pop, HallOfFame(options, T, L), RecordType(), 0.0
//                );
//            } else {
//                if (saved_pop != std::nullptr) {
//#warning "Recreating population (output=$(j), population=$(i)), as the saved one doesn't have the correct number of members.";
//                }
//                auto new_pop = @sr_spawner parallelism worker_idx (
//                        Population(
//                                datasets[j];
//                npop=options.npop,
//                nlength=3,
//                options=options,
//                nfeatures=datasets[j].nfeatures,
//                ),
//                HallOfFame(options, T, L),
//                        RecordType(),
//                        Float64(options.npop
//                        ),
//                );
//                // This involves npop evaluations, on the full dataset:
//            }
//            init_pops[j].emplace_back(new_pop);
//        }
//    }
//
//    // 2. Start the cycle on every process:
//    for (auto j = 1; j <= nout; ++j) {
//        auto dataset = datasets[j];
//        auto running_search_statistics = all_running_search_statistics[j];
//        auto curmaxsize = curmaxsizes[j];
//        for (auto i = 1; i <= (options.npopulations); ++i) {
//            @recorder record["out$(j)_pop$(i)"] = RecordType();
//            auto worker_idx = next_worker(worker_assignment, procs);
//            if (parallelism == "multiprocessing") {
//                worker_assignment[j][i] = worker_idx;
//            }
//
//            // TODO - why is this needed??
//            // Multi-threaded doesn't like to fetch within a new task:
//            auto c_rss = deepcopy(running_search_statistics);
//            auto updated_pop = @sr_spawner parallelism worker_idx let
//            auto in_pop = if parallelism in (:multiprocessing, :multithreading)
//            fetch(init_pops[j][i])[1];
//            else
//            init_pops[j][i][1];
//
//            auto cur_record = RecordType();
//            @recorder cur_record["out$(j)_pop$(i)"] = RecordType(
//                    "iteration0" => record_population(in_pop, options)
//            );
//            auto tmp_num_evals = 0.0;
//            normalize_frequencies!(c_rss);
//            auto [tmp_pop, tmp_best_seen, evals_from_cycle] = s_r_cycle(
//                    dataset,
//                    in_pop,
//                    options.ncycles_per_iteration,
//                    curmaxsize,
//                    c_rss;
//            verbosity=options.verbosity,
//            options=options,
//            record=cur_record,
//            );
//            tmp_num_evals += evals_from_cycle;
//            auto [tmp_pop, evals_from_optimize] = optimize_and_simplify_population(
//                    dataset, tmp_pop, options, curmaxsize, cur_record
//            );
//            tmp_num_evals += evals_from_optimize;
//            if (options.batching) {
//                for (auto i_member = 1; i_member <= (options.maxsize + MAX_DEGREE); ++i_member) {
//                    auto [score,result_loss] = score_func(
//                            dataset, tmp_best_seen.members[i_member], options
//                    );
//                    tmp_best_seen.members[i_member].score = score;
//                    tmp_best_seen.members[i_member].loss = result_loss;
//                    tmp_num_evals += 1;
//                }
//            }
//            (tmp_pop, tmp_best_seen, cur_record, tmp_num_evals);
//            end
//            push!(allPops[j], updated_pop);
//        }
//    }
//
//    auto last_print_time = time();
//    auto last_speed_recording_time = time();
//    auto num_evals_last = std::accumulate(num_evals.begin(), num_evals.end(), 0);
//    auto num_evals_since_last = std::accumulate(sum.begin(), num_evals.end(), 0) - num_evals_last;
//    auto print_every_n_seconds = 5;
//    std::vector<long> equation_speed;
//
//    if (parallelism == "multiprocessing") {
//        for (auto j = 1; j <= nout; ++j) {
//            for (auto i = 1; i <= (options.npopulations); ++i) {
//                // Start listening for each population to finish:
//                MPI_Request request;
//                MPI_Irecv(&allPops[j][i], 1, MPI_INT, i, j, MPI_COMM_WORLD, &request);
//                requests.push_back(request);
//            }
//        }
//        MPI_Waitall(requests.size(), requests.data(), MPI_STATUSES_IGNORE);
//    }
//
//    // Randomly order which order to check populations:
//    // This is done so that we do work on all nout equally.
//    std::vector<Tuple{Int,Int>> all_idx;
//    for (auto j = 1; j <= nout; ++j) {
//        for (auto i = 1; i <= (options.npopulations); ++i) {
//            all_idx.push_back([j,i]);
//        }
//    }
//    shuffle(all_idx);
//    auto kappa = 0;
//    auto resource_monitor = ResourceMonitor(;
//    absolute_start_time=time(),
//    // Storing n times as many monitoring intervals as populations seems like it will
//    // help get accurate resource estimates:
//    num_intervals_to_store=options.npopulations * 100 * nout,
//    );
//
//    while (sum(cycles_remaining) > 0) {
//        kappa += 1;
//        if (kappa > options.npopulations * nout) {
//            kappa = 1;
//        }
//        // nout, npopulations:
//        auto [j,i] = all_idx[kappa];
//
//        // Check if error on population:
//        if (parallelism in (:multiprocessing, :multithreading)) {
//            if (istaskfailed(tasks[j][i])) {
//                fetch(tasks[j][i]);
//                error("Task failed for population");
//            }
//        }
//        // Non-blocking check if a population is ready:
//        auto population_ready = if parallelism in (:multiprocessing, :multithreading)
//        // TODO: Implement type assertions based on parallelism.
//        isready(channels[j][i]);
//        else
//        true;
//        end
//        // Don't start more if this output has finished its cycles:
//        // TODO - this might skip extra cycles?
//        population_ready &= (cycles_remaining[j] > 0);
//        if (population_ready) {
//            start_work_monitor!(resource_monitor);
//            // Take the fetch operation from the channel since its ready
//            auto [cur_pop,best_seen,cur_record,cur_num_evals] =
//            if parallelism in (:multiprocessing, :multithreading)
//            take!(channels[j][i]);
//            else
//            allPops[j][i];
//            end
//                    cur_pop::Population;
//            best_seen::HallOfFame;
//            cur_record::RecordType;
//            cur_num_evals::Float64;
//            returnPops[j][i] = copy_population(cur_pop);
//            bestSubPops[j][i] = best_sub_pop(cur_pop; topn=options.topn);
//            @recorder record = recursive_merge(record, cur_record);
//            num_evals[j][i] += cur_num_evals;
//
//            auto dataset = datasets[j];
//            auto curmaxsize = curmaxsizes[j];
//
//            //Try normal copy...
//            auto bestPops = Population([
//            member for pop in bestSubPops[j] for member in pop.members
//            ]);
//
//            ###################################################################
//# Hall Of Fame updating ###########################################
//            for (auto [i_member,member] : enumerate(
//                    Iterators.flatten((cur_pop.members,best_seen.members[best_seen.exists]))
//            )) {
//                auto part_of_cur_pop = i_member <= length(cur_pop.members);
//                auto size = compute_complexity(member,options);
//
//                if (part_of_cur_pop) {
//                    update_frequencies!(all_running_search_statistics[j]; size=size);
//                }
//                auto actualMaxsize = options.maxsize + MAX_DEGREE;
//
//                auto valid_size = 0 < size < actualMaxsize;
//                if (valid_size) {
//                    auto already_filled = hallOfFame[j].exists[size];
//                    auto better_than_current = member.score < hallOfFame[j].members[size].score;
//                    if (!already_filled || better_than_current) {
//                        hallOfFame[j].members[size] = copy_pop_member(member);
//                        hallOfFame[j].exists[size] = true;
//                    }
//                }
//            }
//            ###################################################################
//
//            // Dominating pareto curve - must be better than all simpler equations
//            auto dominating = calculate_pareto_frontier(hallOfFame[j]);
//
//            if (options.save_to_file) {
//                auto output_file = options.output_file;
//                if (nout > 1) {
//                    output_file = output_file * ".out$j";
//                }
//                // Write file twice in case exit in middle of filewrite
//                for (auto out_file : {output_file, output_file * ".bkup"}) {
//                    open(out_file, "w") do io
//                                println(io, "Complexity,Loss,Equation");
//                    for (auto member : dominating) {
//                        println(
//                                io,
//                                "$(compute_complexity(member,options)),$(member.loss),\"" *
//                                "$(string_tree(member.tree,options.operators,varMap=dataset.varMap))\"",
//                        );
//                    }
//                    end
//                }
//            }
//            ###################################################################
//# Migration #######################################################
//            if (options.migration) {
//                migrate!(
//                        bestPops.members => cur_pop, options; frac=options.fraction_replaced
//                );
//            }
//            if (options.hof_migration && length(dominating) > 0) {
//                migrate!(dominating => cur_pop, options; frac=options.fraction_replaced_hof);
//            }
//            ###################################################################
//
//            cycles_remaining[j] -= 1;
//            if (cycles_remaining[j] == 0) {
//                break;
//            }
//            auto worker_idx = next_worker(worker_assignment, procs);
//            if (parallelism == :multiprocessing) {
//                worker_assignment[{j,i}] = worker_idx;
//            }
//            @recorder begin
//            auto key = "out$(j)_pop$(i)";
//            auto iteration = find_iteration_from_record(key, record) + 1;
//            end
//
//            auto c_rss = deepcopy(all_running_search_statistics[j]);
//            auto c_cur_pop = copy_population(cur_pop);
//            allPops[j][i] = @sr_spawner parallelism worker_idx let
//            auto cur_record = RecordType();
//            @recorder cur_record[key] = RecordType(
//                    "iteration$(iteration)" => record_population(c_cur_pop,options)
//            );
//            auto tmp_num_evals = 0.0;
//            normalize_frequencies!(c_rss);
//            // TODO: Could the dataset objects themselves be modified during the search??
//            // Perhaps inside the evaluation kernels?
//            // It shouldn't be too expensive to copy the dataset.
//            auto [tmp_pop,tmp_best_seen,evals_from_cycle] = s_r_cycle(
//                    dataset,
//                    c_cur_pop,
//                    options.ncycles_per_iteration,
//                    curmaxsize,
//                    c_rss;
//            verbosity=options.verbosity,
//            options=options,
//            record=cur_record,
//            );
//            tmp_num_evals += evals_from_cycle;
//            auto [tmp_pop,evals_from_optimize] = optimize_and_simplify_population(
//                    dataset, tmp_pop, options, curmaxsize, cur_record
//            );
//            tmp_num_evals += evals_from_optimize;
//
//            // Update scores if using batching:
//            if (options.batching) {
//                for (auto i_member = 1; i_member <= (options.maxsize + MAX_DEGREE); ++i_member) {
//                    if (tmp_best_seen.exists[i_member]) {
//                        auto [score,result_loss] = score_func(
//                                dataset, tmp_best_seen.members[i_member], options
//                        );
//                        tmp_best_seen.members[i_member].score = score;
//                        tmp_best_seen.members[i_member].loss = result_loss;
//                        tmp_num_evals += 1;
//                    }
//                }
//            }
//
//            (tmp_pop, tmp_best_seen, cur_record, tmp_num_evals);
//            end
//            if (parallelism in (:multiprocessing, :multithreading)) {
//                tasks[j][i] = @async put!(channels[j][i], fetch(allPops[j][i]));
//            }
//
//            auto cycles_elapsed = total_cycles - cycles_remaining[j];
//            if (options.warmup_maxsize_by > 0) {
//                auto fraction_elapsed = 1.0f0 * cycles_elapsed / total_cycles;
//                if (fraction_elapsed > options.warmup_maxsize_by) {
//                    curmaxsizes[j] = options.maxsize
//                } else {
//                    curmaxsizes[j] =
//                            3 + floor(
//                                    Int,
//                                    (options.maxsize - 3) * fraction_elapsed /
//                                    options.warmup_maxsize_by,
//                            );
//                }
//            }
//            stop_work_monitor!(resource_monitor);
//            move_window!(all_running_search_statistics[j]);
//            if (options.progress && nout == 1) {
//                auto head_node_occupation = estimate_work_fraction(resource_monitor);
//                update_progress_bar!(
//                        progress_bar,
//                                only(hallOfFame),
//                                only(datasets),
//                                options,
//                                equation_speed,
//                                head_node_occupation,
//                                parallelism,
//                );
//            }
//        }
//        sleep(1e-6);
//
//        ################################################################
//        ## Search statistics
//        auto elapsed_since_speed_recording = time() - last_speed_recording_time;
//        if (elapsed_since_speed_recording > 1.0) {
//            num_evals_since_last, num_evals_last = let s = sum(sum, num_evals)
//            s - num_evals_last, s;
//            end
//            auto current_speed = num_evals_since_last / elapsed_since_speed_recording;
//            push!(equation_speed, current_speed);
//            auto average_over_m_measurements = 20; // 20 second running average
//            if (length(equation_speed) > average_over_m_measurements) {
//                deleteat!(equation_speed, 1);
//            }
//            last_speed_recording_time = time();
//        }
//        ################################################################
//
//        ################################################################
//        ## Printing code
//        auto elapsed = time() - last_print_time;
//        // Update if time has passed
//        if (elapsed > print_every_n_seconds) {
//            if ((options.verbosity > 0) || (options.progress && nout > 1)) &&
//            length(equation_speed) > 0 {
//
//                // Dominating pareto curve - must be better than all simpler equations
//                auto head_node_occupation = estimate_work_fraction(resource_monitor);
//                print_search_state(
//                        hallOfFame,
//                        datasets;
//                options,
//                        equation_speed,
//                        total_cycles,
//                        cycles_remaining,
//                        head_node_occupation,
//                        parallelism,
//                        width=options.terminal_width,
//                );
//            }
//            last_print_time = time();
//        }
//        ################################################################
//
//        ################################################################
//        ## Early stopping code
//        if (any((
//                        check_for_loss_threshold(hallOfFame,options),
//                                check_for_user_quit(stdin_reader),
//                                check_for_timeout(start_time,options),
//                                check_max_evals(num_evals,options),
//                ))) {
//            break;
//        }
//        ################################################################
//    }
//
//    close_reader!(stdin_reader);
//
//    // Safely close all processes or threads
//    if (parallelism == :multiprocessing) {
//        we_created_procs && rmprocs(procs);
//    } else if (parallelism == :multithreading) {
//        for (auto j = 1; j <= nout; ++j) {
//            for (auto i = 1; i <= (options.npopulations); ++i) {
//                wait(allPops[j][i]);
//            }
//        }
//    }
//
//    MPI::Finalize();
//}

int main(int argc, char** argv) {

    return 0;
}
