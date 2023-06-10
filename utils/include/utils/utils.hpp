#pragma once
#include "macros.hpp"
#include "detail/dependencies.hpp"

namespace utils
{
  template<typename... Bases>
  struct visitor : Bases...
  { 
    using Bases::operator()...;
  };
}

#include "detail/assert.hpp"
#include "detail/type_support.hpp"
#include "detail/common.hpp"
#include "detail/math.hpp"
#include "detail/vector.hpp"
#include "detail/matrix.hpp"
#include "detail/strings.hpp"
#include "detail/random.hpp"
#include "detail/clock.hpp"
#include "detail/colour.hpp"