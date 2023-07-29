#pragma once

// std headers we'll most likely be using
#include <type_traits>
#include <concepts>
#include <utility>
#include <algorithm>
#include <ranges>

#include <bit>
#include <numeric>
#include <numbers>
#include <random>
#include <future>
#include <chrono>
#include <atomic>
#include <initializer_list>

#include <source_location>
#include <iostream>
#include <fstream>

#include <string>
#include <string_view>
#include <format>
#include <array>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <stack>
#include <queue>

#include <stdexcept>
#include <functional>

#include <filesystem>
#include <memory>

#include <source_location>
#include <stacktrace>


using namespace std::literals;

namespace ranges = std::ranges;
namespace views  = std::views;
namespace chrono = std::chrono;

namespace utils
{
  using src_loc = std::source_location;
  
  using istream = std::istream;
  using ostream = std::ostream;

  template <typename I> concept fwd_it = std::forward_iterator<I>;

  //
  // Checks that the parameter pack is not empty
  //
  template <typename ...Pack>
  concept not_empty = (sizeof...(Pack) != 0);

  //
  // Checks that all types in the pack are the same
  //
  template <typename Arg1, typename ...Args>
  concept all_same = std::conjunction_v<std::is_same<Arg1, Args>...>;

  //
  // Checks that all types in a pack are the same, and the pack is larger than 1 element
  //
  template <typename Arg1, typename ...Args>
  concept homogenous_pack = not_empty<Args...> && all_same<Arg1, Args...>;

  //
  // Checks whether types can be comared by value
  //
  template <typename Arg1, typename Arg2, typename ...Args>
  concept equ_comparable = requires (Arg1 a, Arg2 b, Args... args)
  {
    { ((a == b), ..., (a == args)) };
  };

  //
  // Checks whether all provided types are convertible to the first type
  //
  template <typename To, typename From1, typename ...FromN>
  concept all_convertible = std::conjunction_v<std::is_convertible<From1, To>,
                                               std::is_convertible<FromN, To>...>;

  //
  // A concept for any comparable type
  //
  template <typename T>
  concept comparable = requires (T a, T b)
  {
    { a < b };
    { a > b };
    { a <= b };
    { a >= b };
    { a != b };
    { a == b };
  };

  //
  // Basic numerics and the like
  //
  using max_real_t = long double;
  using max_int_t = std::uintmax_t;

  template <typename T>
  concept integer = std::integral<T> && !std::same_as<T, bool>;
  template <typename T>
  concept real = std::floating_point<T>;

  //
  // Coordinate type accepted by vectors and matrices
  //
  template <typename T>
  concept coordinate = integer<T> || real<T>;

  //
  // Defines a math vector type
  //
  template <typename T>
  concept math_vec = requires (T a)
  {
    requires coordinate<typename T::value_type>;
    requires integer<typename T::size_type>;
    requires integer<decltype(a.dimensions)>;
    requires coordinate<decltype(a.zero_coord)>;
    requires coordinate<decltype(a.unit_coord)>;
  };

  //
  // Defines a math matrix type
  //
  template <typename T>
  concept math_matr = requires (T a)
  {
    requires coordinate<typename T::value_type>;
    requires integer<decltype(a.width)>;
    requires integer<decltype(a.height)>;
    requires math_vec<typename T::row_type>;
    requires math_vec<typename T::col_type>;
  };

  //
  // Index sequence shorthand
  //
  template <typename Int, Int... Seq>
  using idx_seq = std::integer_sequence<Int, Seq...>;

  //
  // Index generator (pass to functions templated on integer sequences)
  //
  template <std::size_t N>
  using idx_gen = std::make_index_sequence<N>;
}