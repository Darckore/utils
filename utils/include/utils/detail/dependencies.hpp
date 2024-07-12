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

#include <optional>
#include <variant>
#include <expected>

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
namespace fsys   = std::filesystem;

namespace utils
{
  using src_loc = std::source_location;
  
  using istream = std::istream;
  using ostream = std::ostream;

  template <typename I> concept fwd_it = std::forward_iterator<I>;

  //
  // Checks that a type can be copied
  //
  template <typename T>
  concept copyable =
    std::is_nothrow_copy_constructible_v<T> &&
    std::is_nothrow_copy_assignable_v<T>;

  //
  // Checks that a type can be moved
  //
  template <typename T>
  concept movable =
    std::is_nothrow_move_constructible_v<T> &&
    std::is_nothrow_move_assignable_v<T>;

  //
  // Checks that a type can be constructed from the given arguments
  //
  template <typename T, typename ...Args>
  concept constructible = std::is_nothrow_constructible_v<T, Args...>;

  //
  // Checks that a type can be moved or copied
  //
  template <typename T>
  concept clonable = copyable<T> || movable<T>;

  //
  // Checks that the parameter pack is not empty
  //
  template <typename ...Pack>
  concept not_empty = (sizeof...(Pack) != 0);

  //
  // Checks that two types are the same, disregarding cv-ref qualifiers
  //
  template <typename T1, typename T2>
  concept same_noquals = std::same_as<std::remove_cvref_t<T1>, std::remove_cvref_t<T2>>;

  //
  // Checks that all types in the pack are the same
  //
  template <typename Arg1, typename ...Args>
  concept all_same = (same_noquals<Arg1, Args> && ...);

  //
  // Checks that any of types in the pack are the same as the first provided type
  //
  template <typename Arg1, typename ...Args>
  concept any_same_as = (same_noquals<Arg1, Args> || ...);

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
  concept all_convertible = std::conjunction_v<std::is_nothrow_convertible<From1, To>,
                                               std::is_nothrow_convertible<FromN, To>...>;

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

  //
  // Any callable object
  //
  template <typename F, typename Ret, typename... Args>
  concept callable = std::is_nothrow_invocable_r_v<Ret, F, Args...>;

  //
  // A unary predicate
  //
  template <typename F, typename Obj>
  concept unary_predicate = callable<F, bool, const Obj&>;

  //
  // A binary predicate
  //
  template <typename F, typename Obj>
  concept binary_predicate = callable<F, bool, const Obj&, const Obj&>;

  //
  // A unary transform
  // Should modify the provided object in-place
  //
  template <typename F, typename Obj>
  concept unary_transform = callable<F, void, Obj&>;

  //
  // A unary generator
  // Accepts an object pointer and returns a new object based on it
  // according to some rules
  // Should handle a nullptr case
  //
  template <typename F, typename Obj>
  concept unary_generator = callable<F, Obj, const Obj*>;
}