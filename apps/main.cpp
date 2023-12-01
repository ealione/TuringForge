#include <mpi.h>

#include <iostream>
#include <vector>
#include <tuple>

struct MPI_MSG {
    double evaluation;
    int id;
};

int main(int argc, char *argv[]) {
  int p, id;
  int epochs_total = MAX_ITERATIONS, epochs_before_sync = 100;

  SimulatedAnnealing sa = SimulatedAnnealing();

  //  Initialize MPI.
  ierr = MPI_Init ( &argc, &argv );

  if (ierr != 0) {
    cout << "\n";
    cout << "COMMUNICATOR_MPI - Fatal error!";
    cout << "  MPI_Init returned nonzero ierr.\n";
    exit (1);
  }
  
  //  Get the number of processes.
  ierr = MPI_Comm_size (MPI_COMM_WORLD, &p);
 
  //  Get the individual process ID.
  ierr = MPI_Comm_rank (MPI_COMM_WORLD, &id);

  //  Process 0 prints an introductory message.
  if (id == 0) {
    timestamp();
    cout << "\n";
    cout << "COMMUNICATOR_MPI - Master process:\n";
    cout << "\n";
    cout << "  The number of processes is " << p << "\n";
    cout << "\n";
  }

  //  Every process prints a hello.
  cout << "  Process " << id << " is up and running!\n";

  MPI_MSG local_best {0.0, id}, global_best;

  for (int i = 0; i < epochs_total / epochs_before_sync; ++i) {
    sa->run(epochs_before_sync);

    // Find the best solution among all processes.
    local_best.evaluation = sa->current_evaluation;
    MPI_Allreduce(&local_best, &global_best, 1, MPI_DOUBLE_INT, MPI_MINLOC, MPI_COMM_WORLD);

    // Update all processes with the global best.
    MPI_Bcast($sa->current_solution, ?, MPI_INT, global_best.id, MPI_COMM_WORLD);
  }

  // Terminate MPI.
  MPI_Finalize();

  // Terminate.
  if (if == 0) {
    cout << "\n";
    cout << "Execution terminated.\n";
    cout << "\n";
    timestamp();
  }

  return 0;
}
