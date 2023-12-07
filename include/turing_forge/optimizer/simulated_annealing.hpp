#pragma once

#include <random>
#include <functional>

#include "../config.hpp"
#include "../core/random.hpp"

#include "../core/expression.hpp"


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
    std::function<Turingforge::Scalar()> random_generator = Turingforge::RandomScalar(0.0F, +1.0F);
};