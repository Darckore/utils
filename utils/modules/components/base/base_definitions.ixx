module;

#include <type_traits>
#include <concepts>
#include <iterator>
#include <random>
#include <array>
#include <utility>
#include <numbers>

export module utils:definitions;

//
// Common
//
export namespace utils
{
  //
  // Base visitor for variant-like things
  //
  template<typename... Bases>
  struct visitor : Bases...
  {
    using Bases::operator()...;
  };

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
  // Checks whether we've got an iterable range
  //
  template <typename T>
  concept iterable = requires(T a)
  {
    std::begin(a);
    std::end(a);
  };

  //
  // Checks whether we've got a reverse iterable range
  //
  template <typename T>
  concept r_iterable = requires(T a)
  {
    std::rbegin(a);
    std::rend(a);
  };

  //
  // Checks whether a range can be hashed
  //
  template <typename T>
  concept hashable_range = requires(T rng)
  {
    requires iterable<T>;
  static_cast<unsigned char>(*std::begin(rng));
  };

  //
  // Checks whether we've got an iterator from a hashable range
  //
  template <typename I>
  concept hashable_it = requires(I it)
  {
    requires std::is_base_of_v<
      std::forward_iterator_tag,
        typename std::iterator_traits<I>::iterator_category
    >;
      static_cast<unsigned char>(std::declval<typename std::iterator_traits<I>::value_type>());
  };


  //
  // Basic numerics and the like
  //

  using max_real_t = long double;
  using max_int_t = std::uintmax_t;

  template <typename T>
  concept integer = std::is_integral_v<T> && !std::is_same_v<T, bool>;
  template <typename T>
  concept real = std::is_floating_point_v<T>;
}

namespace utils::detail
{
  template <typename T>
  struct rnd_distr;

  template <integer T>
  struct rnd_distr<T>
  {
    using type = std::uniform_int_distribution<T>;
  };

  template <real T>
  struct rnd_distr<T>
  {
    using type = std::uniform_real_distribution<T>;
  };

  template <>
  struct rnd_distr<bool>
  {
    using type = std::bernoulli_distribution;
  };
}

//
// Random
//
export namespace utils
{
  //
  // Random distribution type
  //
  template <typename T>
  using random_distribution_t = detail::rnd_distr<T>::type;
}

//
// Pi
//
export namespace utils
{
  template <real R>
  inline constexpr auto pi = std::numbers::template pi_v<R>;

  template <real R>
  inline constexpr auto piHalf = pi<R> / 2;

  template <real R>
  inline constexpr auto piQuarter = pi<R> / 4;

  template <real R>
  inline constexpr auto piDouble = pi<R> *2;
}

//
// Types
//
export namespace utils
{
  template <typename From, typename To>
  concept static_convertible = requires(From & from, To & to)
  {
    static_cast<To&>(from);
  };

  template <typename T> struct type_to_id;
  template <auto TypeId> struct id_to_type;

  template <typename T>
  inline constexpr auto type_to_id_v = type_to_id<T>::value;

  template <auto TypeId>
  using id_to_type_t = typename id_to_type<TypeId>::type;

  template <typename T> auto get_id(const T&) noexcept;

  template <typename To>
  decltype(auto) cast(const auto& src) noexcept
    requires (static_convertible<std::remove_cvref_t<decltype(src)>, To>)
  {
    return static_cast<const To&>(src);
  }

  template <typename To>
  decltype(auto) cast(auto& src) noexcept
    requires (!std::is_const_v<std::remove_reference_t<decltype(src)>>)
  {
    return const_cast<To&>(cast<To>(std::as_const(src)));
  }

  template <auto Id>
  decltype(auto) cast(const auto& src) noexcept
  {
    return cast<id_to_type_t<Id>>(src);
  }

  template <auto Id>
  decltype(auto) cast(auto& src) noexcept
  {
    return cast<id_to_type_t<Id>>(src);
  }

  template <typename To>
  auto try_cast(const auto* src) noexcept
  {
    using res_t = const To*;
    using utils::get_id;
    if (!src || get_id(*src) != type_to_id_v<To>)
      return res_t{};

    return &cast<To>(*src);
  }

  template <typename To>
  auto try_cast(auto* src) noexcept
  {
    using from_type = std::remove_pointer_t<decltype(src)>;
    return const_cast<To*>(try_cast<To>(static_cast<const from_type*>(src)));
  }

  template <auto Id>
  auto try_cast(const auto* src) noexcept
  {
    return try_cast<id_to_type_t<Id>>(src);
  }

  template <auto Id>
  auto try_cast(auto* src) noexcept
  {
    return try_cast<id_to_type_t<Id>>(src);
  }
}