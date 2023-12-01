#pragma once

#include <random>

#include "config.hpp"


class SimulatedAnnealing {
public:
  double * current_solution = nullptr;
  double * candidate_solution = nullptr;

  double current_evaluation;
  double candidate_evaluation;
  double current_temperature = MAX_TEMPERATURE;

  void run(int epochs = MAX_ITERATIONS);

private:
  std::random_device rd;
  std::default_random_engine generator(rd());
  std::uniform_real_distribution<double> distribution(0.0, 1.0);
};
