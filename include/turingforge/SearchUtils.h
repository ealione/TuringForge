#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>

template <typename T, typename L, typename D>
std::vector<std::vector<Population<T, L>>> init_dummy_pops(int nout, int npops, const std::vector<D>& datasets, const Options& options) {
    std::vector<std::vector<Population<T, L>>> dummy_pops(nout);
    for (int j = 0; j < nout; j++) {
        dummy_pops[j].reserve(npops);
        for (int i = 0; i < npops; i++) {
            Population<T, L> population(datasets[j], 1, options, datasets[j].nfeatures);
            dummy_pops[j].push_back(population);
        }
    }
    return dummy_pops;
}

int next_worker(const std::unordered_map<std::pair<int, int>, int>& worker_assignment, const std::vector<int>& procs) {
    std::unordered_map<int, int> job_counts;
    for (const auto& [key, value] : worker_assignment) {
        if (job_counts.count(value) == 0) {
            job_counts[value] = 0;
        }
        job_counts[value]++;
    }

    int least_busy_worker = std::reduce(procs.begin(), procs.end(),
                                        [](int proc1, int proc2) {
                                            return job_counts[proc1] <= job_counts[proc2] ? proc1 : proc2;
                                        });

    return least_busy_worker;
}

int next_worker(const std::unordered_map<std::pair<int, int>, int>& worker_assignment, std::nullptr_t procs) {
    return 0;
}

struct StdinReader {
    bool can_read_user_input;
    ST stream;
};

StdinReader watch_stream(ST stream) {
    bool can_read_user_input = isreadable(stream);

    if (can_read_user_input) {
        try {
            Base::start_reading(stream);
            size_t bytes = bytesavailable(stream);
            if (bytes > 0) {
                // Clear out initial data
                read(stream, bytes);
            }
        } catch (const MethodError& err) {
            can_read_user_input = false;
        } catch (const std::exception& err) {
            throw err;
        }
    }

    return StdinReader{can_read_user_input, stream};
}

void close_reader(StdinReader& reader) {
    if (reader.can_read_user_input) {
        Base::stop_reading(reader.stream);
    }
}

bool check_for_user_quit(const StdinReader& reader) {
    if (reader.can_read_user_input) {
        size_t bytes = bytesavailable(reader.stream);
        if (bytes > 0) {
            // Read:
            std::vector<char> data(bytes);
            read(reader.stream, data.data(), bytes);
            constexpr char control_c = 0x03;
            constexpr char quit = 0x71;
            if (data.size() > 1 && (data.back() == control_c || data[data.size() - 2] == quit)) {
                return true;
            }
        }
    }
    return false;
}

bool check_for_loss_threshold(const std::vector<H>& hallOfFame, const Options& options) {
    if (options.early_stop_condition == nullptr) {
        return false;
    }

    for (const auto& hof : hallOfFame) {
        std::vector<bool> stop_conditions;
        stop_conditions.reserve(hof.exists.size());
        for (const auto& [exists, member] : zip(hof.exists, hof.member)) {
            if (exists) {
                stop_conditions.push_back(options.early_stop_condition(member.loss));
            }
        }

        if (stop_conditions.size() == hof.exists.size()) {
            bool any_stop_condition = std::any_of(stop_conditions.begin(), stop_conditions.end(), [](bool cond) { return cond; });
            if (any_stop_condition) {
                return true;
            }
        }
    }

    return false;
}

template <typename T, typename L>
void print_log(const Population<T, L>& population, int generation) {
    std::cout << "Generation " << generation << ": ";
    std::cout << "Loss = " << population.member[1].loss;
    std::cout << ", Variables = ";
    for (const auto& variable : population.member[1].variables) {
        std::cout << variable << " ";
    }
    std::cout << std::endl;
}

template <typename T, typename L>
void print_results(const Population<T, L>& population, const Options& options) {
    std::cout << "Final Results:" << std::endl;
    std::cout << "Loss = " << population.member[1].loss << std::endl;
    std::cout << "Variables = ";
    for (const auto& variable : population.member[1].variables) {
        std::cout << variable << " ";
    }
    std::cout << std::endl;

    if (options.verbose) {
        std::cout << "Log:" << std::endl;
        for (const auto& log : population.logbook) {
            std::cout << "Generation " << log.generation << ": ";
            std::cout << "Loss = " << log.loss;
            std::cout << ", Variables = ";
            for (const auto& variable : log.variables) {
                std::cout << variable << " ";
            }
            std::cout << std::endl;
        }
    }
}

template <typename T, typename L, typename D>
void evaluate(const std::vector<std::vector<Population<T, L>>>& dummy_pops, int nout, int npops, const std::vector<D>& datasets, const Options& options) {
std::vector<std::unordered_map<std::pair<int, int>, int>> worker_assignment(nout);
std::vector<int> procs(npops, 0);
for (int j = 0; j < nout; j++) {
for (int i = 0; i < npops; i++) {
worker_assignment[j][std::make_pair(i, j)] = i % npops;
procs[i % npops]++;
}
}

std::vector<bool> all_finished(nout, false);
std::vector<std::vector<H>> hallOfFame(nout, std::vector<H>(npops));
std::vector<int> gens_since_improvement(nout, 0);
int generation = 0;

while (!std::all_of(all_finished.begin(), all_finished.end(), [](bool finished) { return finished; })) {
for (int j = 0; j < nout; j++) {
if (all_finished[j]) {
continue;
}

for (int i = 0; i < npops; i++) {
std::vector<H>& hof = hallOfFame[j];
Population<T, L>& pop = dummy_pops[j][i];

if (pop.status != Status::FINISHED) {
continue;
}

if (pop.member[1].loss < hof[i].member[1].loss) {
hof[i] = pop.member[1];
gens_since_improvement[j] = 0;
}

if (gens_since_improvement[j] >= options.generations) {
all_finished[j] = true;
break;
}

if (options.mutation_probability > 0.0 && i > 0) {
crossover(hof[i], hof[0], options.crossover_probability);
mutate(hof[i], options.mutation_probability);
}

if (check_for_loss_threshold(hof, options)) {
all_finished[j] = true;
break;
}

gens_since_improvement[j] += 1;
procs[worker_assignment[j][std::make_pair(i, j)]] -= 1;
worker_assignment[j][std::make_pair(i, j)] = std::distance(procs.begin(), std::min_element(procs.begin(), procs.end()));
procs[worker_assignment[j][std::make_pair(i, j)]] += 1;

pop.status = Status::WAITING;
pop.member[1] = hof[i];
pop.member[1].loss = datasets[j].loss(pop.member[1].variables);
}
}

for (int j = 0; j < nout; j++) {
for (int i = 0; i < npops; i++) {
Population<T, L>& pop = dummy_pops[j][i];

if (pop.status == Status::WAITING) {
pop.member[0] = pop.member[1];
pop.status = Status::RUNNING;
pop.futures[1] = std::async(std::launch::async, evaluate_member<T, L, D>, pop.member[1], datasets[j]);
}

if (pop.futures[1].valid() && pop.futures[1].wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
pop.futures[1].get();
pop.status = Status::FINISHED;
if (options.verbose) {
print_log(pop, generation);
}
}
}
}

generation += 1;
}

std::cout << "Completed " << generation << " generations." << std::endl;
for (int j = 0; j < nout; j++) {
std::cout << "Output " << j + 1 << " best member:" << std::endl;
std::cout << "Loss = " << hallOfFame[j][0].loss << std::endl;
std::cout << "Variables = ";
for (const auto& variable : hallOfFame[j][0].variables) {
std::cout << variable << " ";
}
std::cout << std::endl;
}
}

int main() {
    Options options;
    // Set options as needed

    std::vector<std::vector<Population<int, float>>> dummy_pops;
    int nout = 2;  // Number of outputs
    int npops = 4; // Number of populations
    std::vector<D> datasets(nout);
    // Initialize datasets

    evaluate(dummy_pops, nout, npops, datasets, options);

    return 0;
}