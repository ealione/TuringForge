#include "turing_forge/optimizer/simulated_annealing.hpp"


void SimulatedAnnealing::run(int epochs) {
  current_solution = random_solution();
  current_evaluation = eval(current_solution);
  int current_iteration = 0;

  while (current_temperature >= MIN_TEMPERATURE || ++current_iteration < epochs) {
    candidate_solution = perturb(current_solution); 
    candidate_evaluation = eval(candidate_solution);
    
    double bypass_probability = exp((candidate_evaluation - current_evaluatiion) / current_temperature); 
    double random_number = random_generator();

    if (candidate_evaluation >= current_evaluation || random_number <= bypass_probability) {
      std::swap(current_solution, candidate_solution);
      current_evaluation = candidate_evaluation;
    } else {
      current_temperature *= COOLING_RATE;
    }
  }
}