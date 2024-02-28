#pragma once
#include <string>
#if defined(_MSC_VER)
#define _DZYCXX_UNREACHABLE() __assume(0)
#elif defined(__clang__)
#define _DZYCXX_UNREACHABLE() __builtin_unreachable()
#elif defined(__GNUC__)	
#define _DZYCXX_UNREACHABLE() __builtin_unreachable()
#else
#define _DZYCXX_UNREACHABLE() do{} while(1)
#endif

#if defined(_MSC_VER)
#define _DZYCXX_BOOL_TESTABLE(a) std::_Boolean_testable<a>
#elif defined(__GNUC__ || __clang__)
#define _DZYCXX_BOOL_TESTABLE(a) std::__detail::__boolean_testable<a>
#else
#define _DZYCXX_BOOL_TESTABLE(a) 1
#endif

#ifndef _DZYCXX_HASCXX14
#if __cplusplus > 201103L || _MSVC_LANG > 201103L
#define _DZYCXX_HASCXX14 1
#else
#define _DZYCXX_HASCXX14 0
#endif
#endif

#ifndef _DZYCXX_HASCXX17
# if __cplusplus > 201402L || _MSVC_LANG > 201402L
#define _DZYCXX_HASCXX17 1
#else
#define _DZYCXX_HASCXX17 0
#endif
#endif

#ifndef _DZYCXX_HASCXX20
# if __cplusplus > 201703L || _MSVC_LANG > 201703L
#define _DZYCXX_HASCXX20 1
#else
#define _DZYCXX_HASCXX20 0
#endif
#endif

#ifndef _DZYCXX_HASCXX23
# if __cplusplus > 202002L || _MSVC_LANG > 202002L
#define _DZYCXX_HASCXX23 1
#else
#define _DZYCXX_HASCXX23 0
#endif
#endif

#ifndef _DZYCXX_HASCXX26
# if __cplusplus > 202302L || _MSVC_LANG > 202302L
#define _DZYCXX_HASCXX26 1
#else
#define _DZYCXX_HASCXX26 0
#endif
#endif

#ifndef _DZYCXX_CONSTEXPR14
# if _DZYCXX_HASCXX14
#  define _DZYCXX_CONSTEXPR14 constexpr
# else
#  define _DZYCXX_CONSTEXPR14
# endif
#endif

#ifndef _DZYCXX_CONSTEXPR17
# if _DZYCXX_HASCXX17
#  define _DZYCXX_CONSTEXPR17 constexpr
# else
#  define _DZYCXX_CONSTEXPR17
# endif
#endif

#ifndef _DZYCXX_CONSTEXPR20
# if _DZYCXX_HASCXX20
#  define _DZYCXX_CONSTEXPR20 constexpr
# else
#  define _DZYCXX_CONSTEXPR20
# endif
#endif

#ifndef _DZYCXX_CONSTEXPR23
# if _DZYCXX_HASCXX23
#  define _DZYCXX_CONSTEXPR23 constexpr
# else
#  define _DZYCXX_CONSTEXPR23
# endif
#endif

#ifndef _DZYCXX_CONSTEXPR26
#if _DZYCXX_HASCXX26
#  define _DZYCXX_CONSTEXPR26 constexpr
# else
#  define _DZYCXX_CONSTEXPR26
# endif
#endif

#ifndef _DZYCXX_NODISCARD
# if __cplusplus > 201703L || _MSVC_LANG > 201703L
#define _DZYCXX_NODISCARD [[nodiscard]]
#else
#define _DZYCXX_NODISCARD
#endif
#endif

#if _HAS_CXX17
#define _DZYCXX_INLINE inline
#else
#define _DZYCXX_INLINE
#endif

#if _HAS_CXX17
#define _DZYCXX_VOID_T std::void_t
#else
template<typename...> struct void_t { using type = void; };
#define _DZYCXX_VOID_T void_t
#endif

#define _DZYCXX_OUT_OF_RANGE(_Index) throw std::out_of_range("invalid " + std::string(typeid(*this).name()) + " index, index = " + std::to_string(_Index))