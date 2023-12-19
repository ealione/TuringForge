#include <mpi.h>
#include <fmt/format.h>

#include <iostream>
#include <vector>
#include <tuple>

#include "turing_forge/optimizer/simulated_annealing.hpp"
#include "turing_forge/optimizer/config.hpp"
#include "turing_forge/core/options.hpp"
#include "turing_forge/utils.hpp"


struct MPI_MSG {
    double evaluation;
    int id;
};

int main(int argc, char *argv[]) {

    auto opts = Turingforge::InitOptions("Turingforge", "hardware accelerated symbolic regression");
    auto result = Turingforge::ParseOptions(std::move(opts), argc, argv);
    Turingforge::SimulatedAnnealingConfig config{};
    config.Generations = result["generations"].as<size_t>();
    config.PopulationSize = result["population-size"].as<size_t>();
    config.PoolSize = result["pool-size"].as<size_t>();
    config.Evaluations = result["evaluations"].as<size_t>();
    config.Iterations = result["iterations"].as<size_t>();
    config.CrossoverProbability = result["crossover-probability"].as<Turingforge::Scalar>();
    config.MutationProbability = result["mutation-probability"].as<Turingforge::Scalar>();
    config.TimeLimit = result["timelimit"].as<size_t>();
    config.Seed = std::random_device {}();

  int ierr;
  int p, id;
  int epochs_total = MAX_ITERATIONS, epochs_before_sync = 100;

  SimulatedAnnealing sa = SimulatedAnnealing();

  //  Initialize MPI.
  ierr = MPI_Init ( &argc, &argv );

  if (ierr != 0) {
    std::cout << "\n";
    std::cout << "COMMUNICATOR_MPI - Fatal error!";
    std::cout << "  MPI_Init returned nonzero ierr.\n";
    exit (1);
  }

  //  Get the number of processes.
  ierr = MPI_Comm_size (MPI_COMM_WORLD, &p);

  //  Get the individual process ID.
  ierr = MPI_Comm_rank (MPI_COMM_WORLD, &id);

  //  Process 0 prints an introductory message.
  if (id == 0) {
    Turingforge::Utils::timestamp();
    std::cout << "\n";
      std::cout << "COMMUNICATOR_MPI - Master process:\n";
      std::cout << "\n";
      std::cout << "  The number of processes is " << p << "\n";
      std::cout << "\n";
  }

  //  Every process prints a hello.
    std::cout << "  Process " << id << " is up and running!\n";

  MPI_MSG local_best {0.0, id}, global_best {};

  for (int i = 0; i < epochs_total / epochs_before_sync; ++i) {
    sa.run(epochs_before_sync);

    // Find the best solution among all processes.
    local_best.evaluation = sa.current_evaluation;
    MPI_Allreduce(&local_best, &global_best, 1, MPI_DOUBLE_INT, MPI_MINLOC, MPI_COMM_WORLD);

    // Update all processes with the global best.
    MPI_Bcast(sa.current_solution, , MPI_INT, global_best.id, MPI_COMM_WORLD);
  }

  // Terminate MPI.
  MPI_Finalize();

  // Terminate.
  if (id == 0) {
    std::cout << "\n";
    std::cout << "Execution terminated.\n";
    std::cout << "\n";
    Turingforge::Utils::timestamp();
  }

  return 0;
}
