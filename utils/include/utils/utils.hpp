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
namespace views  = std::views;
namespace chrono = std::chrono;

namespace utils
{
  template<typename... Bases>
  struct visitor : Bases...
  { 
    using Bases::operator()...;
  };
}

#define CLASS_DEFAULT = default

#define CLASS_DELETE = delete

#define CLASS_DEFAULT_CTOR(clName)   constexpr clName() noexcept

#define CLASS_COPY_CTOR(clName)      constexpr clName(const clName&) noexcept
#define CLASS_COPY_OPERATOR(clName)  constexpr clName& operator=(const clName&) noexcept

#define CLASS_MOVE_CTOR(clName)      constexpr clName(clName&&) noexcept
#define CLASS_MOVE_OPERATOR(clName)  constexpr clName& operator=(clName&&) noexcept

#define CLASS_SPECIALS_ALL(clName)\
  CLASS_DEFAULT_CTOR(clName)  CLASS_DEFAULT;\
  CLASS_COPY_CTOR(clName)     CLASS_DEFAULT;\
  CLASS_COPY_OPERATOR(clName) CLASS_DEFAULT;\
  CLASS_MOVE_CTOR(clName)     CLASS_DEFAULT;\
  CLASS_MOVE_OPERATOR(clName) CLASS_DEFAULT

#define CLASS_SPECIALS_ALL_CUSTOM(clName)\
  CLASS_COPY_CTOR(clName)     CLASS_DEFAULT;\
  CLASS_COPY_OPERATOR(clName) CLASS_DEFAULT;\
  CLASS_MOVE_CTOR(clName)     CLASS_DEFAULT;\
  CLASS_MOVE_OPERATOR(clName) CLASS_DEFAULT

#define CLASS_SPECIALS_NOCOPY(clName)\
  CLASS_DEFAULT_CTOR(clName)  CLASS_DEFAULT;\
  CLASS_COPY_CTOR(clName)     CLASS_DELETE;\
  CLASS_COPY_OPERATOR(clName) CLASS_DELETE;\
  CLASS_MOVE_CTOR(clName)     CLASS_DEFAULT;\
  CLASS_MOVE_OPERATOR(clName) CLASS_DEFAULT

#define CLASS_SPECIALS_NOCOPY_CUSTOM(clName)\
  CLASS_COPY_CTOR(clName)     CLASS_DELETE;\
  CLASS_COPY_OPERATOR(clName) CLASS_DELETE;\
  CLASS_MOVE_CTOR(clName)     CLASS_DEFAULT;\
  CLASS_MOVE_OPERATOR(clName) CLASS_DEFAULT

#define CLASS_SPECIALS_NODEFAULT(clName)\
  CLASS_DEFAULT_CTOR(clName)  CLASS_DELETE;\
  CLASS_COPY_CTOR(clName)     CLASS_DEFAULT;\
  CLASS_COPY_OPERATOR(clName) CLASS_DEFAULT;\
  CLASS_MOVE_CTOR(clName)     CLASS_DEFAULT;\
  CLASS_MOVE_OPERATOR(clName) CLASS_DEFAULT

#define CLASS_SPECIALS_NODEFAULT_NOCOPY(clName)\
  CLASS_DEFAULT_CTOR(clName)  CLASS_DELETE;\
  CLASS_COPY_CTOR(clName)     CLASS_DELETE;\
  CLASS_COPY_OPERATOR(clName) CLASS_DELETE;\
  CLASS_MOVE_CTOR(clName)     CLASS_DEFAULT;\
  CLASS_MOVE_OPERATOR(clName) CLASS_DEFAULT

#define CLASS_SPECIALS_NONE(clName)\
  CLASS_DEFAULT_CTOR(clName)  CLASS_DELETE;\
  CLASS_COPY_CTOR(clName)     CLASS_DELETE;\
  CLASS_COPY_OPERATOR(clName) CLASS_DELETE;\
  CLASS_MOVE_CTOR(clName)     CLASS_DELETE;\
  CLASS_MOVE_OPERATOR(clName) CLASS_DELETE

#define CLASS_SPECIALS_NONE_CUSTOM(clName)\
  CLASS_COPY_CTOR(clName)     CLASS_DELETE;\
  CLASS_COPY_OPERATOR(clName) CLASS_DELETE;\
  CLASS_MOVE_CTOR(clName)     CLASS_DELETE;\
  CLASS_MOVE_OPERATOR(clName) CLASS_DELETE

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

#include "detail/assert.hpp"
#include "detail/common.hpp"
#include "detail/math.hpp"
#include "detail/vector.hpp"
#include "detail/matrix.hpp"
#include "detail/strings.hpp"
#include "detail/random.hpp"
#include "detail/clock.hpp"
#include "detail/colour.hpp"