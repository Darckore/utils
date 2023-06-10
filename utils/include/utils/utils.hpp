#pragma once
#include "macros.hpp"
#include "detail/dependencies.hpp"
import utils;

namespace utils::detail
{
  using max_real_t = long double;
  using max_int_t = std::uintmax_t;

  template <typename T>
  concept integer = std::is_integral_v<T> && !std::is_same_v<T, bool>;
  template <typename T>
  concept real = std::is_floating_point_v<T>;

}

#include "detail/assert.hpp"
#include "detail/type_support.hpp"
#include "detail/ratio.hpp"
#include "detail/vector.hpp"
#include "detail/matrix.hpp"