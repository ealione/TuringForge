#ifndef CERES_PUBLIC_JET_FWD_H_
#define CERES_PUBLIC_JET_FWD_H_

namespace ceres {

// Jet forward declaration necessary for the following partial specialization of
// std::common_type and type traits.
template <typename T, int N>
struct Jet;

}  // namespace ceres

#endif  // CERES_PUBLIC_JET_FWD_H_
