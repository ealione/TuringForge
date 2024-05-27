#ifndef CERES_PUBLIC_INTERNAL_PORT_H_
#define CERES_PUBLIC_INTERNAL_PORT_H_

// A macro to mark a function/variable/class as deprecated.
// We use compiler specific attributes rather than the c++
// attribute because they do not mix well with each other.
#if defined(_MSC_VER)
#define CERES_DEPRECATED_WITH_MSG(message) __declspec(deprecated(message))
#elif defined(__GNUC__)
#define CERES_DEPRECATED_WITH_MSG(message) __attribute__((deprecated(message)))
#else
// In the worst case fall back to c++ attribute.
#define CERES_DEPRECATED_WITH_MSG(message) [[deprecated(message)]]
#endif

#ifndef CERES_GET_FLAG
#define CERES_GET_FLAG(X) X
#endif

// Indicates whether C++20 is currently active
#ifndef CERES_HAS_CPP20
#if __cplusplus >= 202002L || (defined(_MSVC_LANG) && _MSVC_LANG >= 202002L)
#define CERES_HAS_CPP20
#endif  // __cplusplus >= 202002L || (defined(_MSVC_LANG) && _MSVC_LANG >=
        // 202002L)
#endif  // !defined(CERES_HAS_CPP20)

// Prevents symbols from being substituted by the corresponding macro definition
// under the same name. For instance, min and max are defined as macros on
// Windows (unless NOMINMAX is defined) which causes compilation errors when
// defining or referencing symbols under the same name.
//
// To be robust in all cases particularly when NOMINMAX cannot be used, use this
// macro to annotate min/max declarations/definitions. Examples:
//
//   int max CERES_PREVENT_MACRO_SUBSTITUTION();
//   min CERES_PREVENT_MACRO_SUBSTITUTION(a, b);
//   max CERES_PREVENT_MACRO_SUBSTITUTION(a, b);
//
// NOTE: In case the symbols for which the substitution must be prevented are
// used within another macro, the substitution must be inhibited using parens as
//
//   (std::numerical_limits<double>::max)()
//
// since the helper macro will not work here. Do not use this technique in
// general case, because it will prevent argument-dependent lookup (ADL).
//
#define CERES_PREVENT_MACRO_SUBSTITUTION  // Yes, it's empty

#endif  // CERES_PUBLIC_INTERNAL_PORT_H_
