#pragma once

#include <random>
#include <functional>

#include "../config.hpp"
#include "turing_forge/random/random.hpp"

#include "../core/individual.hpp"


class SimulatedAnnealing
{
public:
  double *current_solution = nullptr;
  double *candidate_solution = nullptr;

  double current_evaluation;
  double candidate_evaluation;
  double current_temperature = MAX_TEMPERATURE;

  void run(int epochs = MAX_ITERATIONS);

    [[nodiscard]] auto Parents() const -> Turingforge::Span<Turingforge::Individual const> { return { parents_.data(), parents_.size() }; }
    [[nodiscard]] auto Offspring() const -> Turingforge::Span<Turingforge::Individual const> { return { offspring_.data(), offspring_.size() }; }
    [[nodiscard]] auto Individuals() const -> Turingforge::Vector<Turingforge::Individual> const& { return individuals_; }

private:
    Turingforge::Vector<Turingforge::Individual> individuals_;
    Turingforge::Span<Turingforge::Individual> parents_;
    Turingforge::Span<Turingforge::Individual> offspring_;

    std::function<Turingforge::Scalar()> random_generator = Turingforge::RandomScalar(0.0F, +1.0F);
};