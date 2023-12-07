#pragma once

#include <random>

#include "config.hpp"
#include "core/random.hpp"

class SimulatedAnnealing
{
public:
  double *current_solution = nullptr;
  double *candidate_solution = nullptr;

  double current_evaluation;
  double candidate_evaluation;
  double current_temperature = MAX_TEMPERATURE;

  void run(int epochs = MAX_ITERATIONS);

private:
  Turingforge::RandomScalar random_generator = Turingforge::RandomScalar();
};
