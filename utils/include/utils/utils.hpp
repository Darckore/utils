#pragma once

#include <type_traits>
#include <concepts>
#include <algorithm>
#include <ranges>
#include <bit>
#include <numeric>
#include <numbers>
#include <string_view>
#include <array>
#include <random>
#include <future>
#include <chrono>
#include <atomic>
#include <initializer_list>

namespace ranges = std::ranges;
namespace views = std::views;
namespace chrono = std::chrono;

namespace utils
{
  template<typename... Bases>
  struct visitor : Bases...
  { 
    using Bases::operator()...;
  };
}

#define CLASS_SPECIALS_ALL(clName)\
  clName() = default;\
  clName(const clName&) = default;\
  clName& operator=(const clName&) = default;\
  clName(clName&&) = default;\
  clName& operator=(clName&&) = default

#define CLASS_SPECIALS_ALL_CUSTOM(clName)\
  clName(const clName&) = default;\
  clName& operator=(const clName&) = default;\
  clName(clName&&) = default;\
  clName& operator=(clName&&) = default

#define CLASS_SPECIALS_NOCOPY(clName)\
  clName() = default;\
  clName(const clName&) = delete;\
  clName& operator=(const clName&) = delete;\
  clName(clName&&) = default;\
  clName& operator=(clName&&) = default

#define CLASS_SPECIALS_NOCOPY_CUSTOM(clName)\
  clName(const clName&) = delete;\
  clName& operator=(const clName&) = delete;\
  clName(clName&&) = default;\
  clName& operator=(clName&&) = default

#define CLASS_SPECIALS_NODEFAULT(clName)\
  clName() = delete;\
  clName(const clName&) = default;\
  clName& operator=(const clName&) = default;\
  clName(clName&&) = default;\
  clName& operator=(clName&&) = default

#define CLASS_SPECIALS_NODEFAULT_NOCOPY(clName)\
  clName() = delete;\
  clName(const clName&) = delete;\
  clName& operator=(const clName&) = delete;\
  clName(clName&&) = default;\
  clName& operator=(clName&&) = default

#define CLASS_SPECIALS_NONE(clName)\
  clName() = delete;\
  clName(const clName&) = delete;\
  clName& operator=(const clName&) = delete;\
  clName(clName&&) = delete;\
  clName& operator=(clName&&) = delete

#define CLASS_SPECIALS_NONE_CUSTOM(clName)\
  clName(const clName&) = delete;\
  clName& operator=(const clName&) = delete;\
  clName(clName&&) = delete;\
  clName& operator=(clName&&) = delete

#ifndef NDEBUG
  #ifdef _MSC_VER
    #define BREAK_ON(cond) (void)((!(cond)) || ((__debugbreak()), 0))
  #elif __has_builtin(__builtin_debugtrap)
    #define BREAK_ON(cond) (void)((!(cond)) || ((__builtin_debugtrap()), 0))
  #else
    #define BREAK_ON(cond)
  #endif
#else
  #define BREAK_ON(cond) static_assert(false, "BREAK_ON is only allowed in debug builds")
#endif

#include "detail/common.hpp"
#include "detail/math.hpp"
#include "detail/vector.hpp"
#include "detail/matrix.hpp"
#include "detail/strings.hpp"
#include "detail/random.hpp"
#include "detail/clock.hpp"
#include "detail/colour.hpp"