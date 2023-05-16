#pragma once
#include "detail/dependencies.hpp"

namespace utils
{
  template<typename... Bases>
  struct visitor : Bases...
  { 
    using Bases::operator()...;
  };
}

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