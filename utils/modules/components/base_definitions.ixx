module;

#include <type_traits>
#include <concepts>
#include <iterator>

export module utils:definitions;

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