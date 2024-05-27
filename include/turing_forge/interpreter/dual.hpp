#pragma once

#include "turing_forge/core/types.hpp"
#include "ceres/jet.h"

namespace Turingforge {

    using Dual = ceres::Jet<Turingforge::Scalar, 4 * sizeof(double) / sizeof(Turingforge::Scalar)>;

} // namespace Turingforge