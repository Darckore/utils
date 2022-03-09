#pragma once

namespace utils
{
  //
  // A simple way to shut up the compiler when something is unused.
  // Just a convenience for debugging
  //
  template <typename ...Args>
  constexpr void unused(Args&&...) noexcept {}

  namespace detail
  {
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
  }

  //
  // Checks whether a value is in the specified range
  //
  template <detail::comparable T1, detail::comparable T2, detail::comparable T3> requires (T1{} >= T2{} && T1{} <= T3{})
  inline constexpr bool in_range(T1&& val, T2&& low, T3&& high) noexcept
  {
    return val >= low && val <= high;
  }

  //
  // Checks whether the first arg equals ALL of subsequent args
  //
  template <typename T1, typename T2, typename ...Ts> requires detail::equ_comparable<T1, T2, Ts... >
  inline constexpr bool eq_all(T1&& val, T2&& arg1, Ts&& ...args) noexcept
  {
    return ((val == arg1) && ... && (val == args));
  }

  //
  // Checks whether the first arg equals ANY of subsequent args
  //
  template <typename T1, typename T2, typename ...Ts> requires detail::equ_comparable<T1, T2, Ts... >
  inline constexpr bool eq_any(T1&& val, T2&& arg1, Ts&& ...args) noexcept
  {
    return ((val == arg1) || ... || (val == args));
  }

  //
  // Checks whether the first arg equals NONE of subsequent args
  //
  template <typename T1, typename T2, typename ...Ts> requires detail::equ_comparable<T1, T2, Ts... >
  inline constexpr bool eq_none(T1&& val, T2&& arg1, Ts&& ...args) noexcept
  {
    return ((val != arg1) && ... && (val != args));
  }

  //
  // This is needed for const-overloads of class members to avoid code duplication
  // Like so:
  //  
  //  const T* some_method() const { /*...*/ }
  //        T* some_method()
  //        {
  //          return utils::mutate(std::as_const(*this).some_method());
  //        }
  //
  template <typename T>
  inline constexpr decltype(auto) mutate(T&& val) noexcept
    requires (!std::is_rvalue_reference_v<decltype(val)>
            || std::is_pointer_v<std::remove_reference_t<T>>)
  {
    using noref_t = std::remove_reference_t<T>;
    using noconst_t = std::remove_const_t<std::remove_pointer_t<noref_t>>;
    if constexpr (std::is_pointer_v<noref_t>)
    {
      return const_cast<std::add_pointer_t<noconst_t>>(val);
    }
    else
    {
      return const_cast<std::add_lvalue_reference_t<noconst_t>>(val);
    }
  }
}