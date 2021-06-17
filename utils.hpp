#pragma once
/* 
 * This thing is to be included somewhere.
 * Probably, the "somewhere" in question has precompiled headers and stuff.
 * std includes are intentionally absent here, I assume they come from someplace
 * and are perfectly accessible on compilation.
*/

#define CLASS_SPECIALS_ALL(clName)\
  clName() = default;\
  clName(const clName&) = default;\
  clName& operator=(const clName&) = default;\
  clName(clName&&) = default;\
  clName& operator=(clName&&) = default;\
  ~clName() = default

#define CLASS_SPECIALS_NOCOPY(clName)\
  clName() = default;\
  clName(const clName&) = delete;\
  clName& operator=(const clName&) = delete;\
  clName(clName&&) = default;\
  clName& operator=(clName&&) = default;\
  ~clName() = default

#define CLASS_SPECIALS_NODEFAULT(clName)\
  clName() = delete;\
  clName(const clName&) = default;\
  clName& operator=(const clName&) = default;\
  clName(clName&&) = default;\
  clName& operator=(clName&&) = default;\
  ~clName() = default

#define CLASS_SPECIALS_NODEFAULT_NOCOPY(clName)\
  clName() = delete;\
  clName(const clName&) = delete;\
  clName& operator=(const clName&) = delete;\
  clName(clName&&) = default;\
  clName& operator=(clName&&) = default;\
  ~clName() = default

#define CLASS_SPECIALS_NONE(clName)\
  clName() = delete;\
  clName(const clName&) = delete;\
  clName& operator=(const clName&) = delete;\
  clName(clName&&) = delete;\
  clName& operator=(clName&&) = delete;\
  ~clName() = default

// This is for Visual Studio only
#if !defined(NDEBUG) && defined(_WIN32) && defined(_MSC_VER)
  #define BREAK(cond) (void)((!(cond)) || ((__debugbreak()), 0))
#else
  #define BREAK(cond) static_assert(false, "You can only use this in MSVC under debug")
#endif

namespace utils
{
  //
  // A simple way to shut up the compiler when something is unused.
  // Just a convenience for debugging
  //
  template <typename ...Args>
  void unused(Args&&...) noexcept
  {
    static_assert(
#ifndef NDEBUG
      true
#else
      false
#endif
      , "This is for debugging only");
  }

  namespace detail
  {
    //
    // Checks whether types can be comared by value
    //
    template <typename T1, typename T2, typename = std::void_t<>>
    struct is_eq_comparable_pair : std::false_type {};

    template <typename T1, typename T2>
    struct is_eq_comparable_pair<T1, T2,
      std::void_t<decltype(std::declval<T1>() == std::declval<T2>())>> : std::true_type {};

    template <typename T1, typename ...Ts>
    struct is_eq_comparable : std::bool_constant <
      std::conjunction_v<is_eq_comparable_pair<T1, Ts>...> > {};

    template <typename T1, typename ...Ts>
    inline constexpr bool is_eq_comparable_v = is_eq_comparable<T1, Ts...>::value;

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
    // A constexpr abs version
    //
    template <typename T>
    constexpr auto abs(T val) noexcept requires std::is_arithmetic_v<T>
    {
      return val < 0 ? -val : val;
    }
  }

  //
  // This is mostly for floats
  // To avoid dealing with the epsilon bs on comparing them
  //
  template <detail::comparable T>
  constexpr bool eq(T left, T right) noexcept requires std::is_arithmetic_v<T>
  {
    constexpr auto min_diff = std::numeric_limits<T>::epsilon();
    return detail::abs(left - right) <= min_diff;
  }

  //
  // Checks whether a value is in the specified range
  //
  template <detail::comparable T1, detail::comparable T2, detail::comparable T3>
  inline constexpr bool in_range(T1&& val, T2&& low, T3&& high) noexcept
    requires (T1{} >= T2{} && T1{} <= T3{})
  {
    return val >= low && val <= high;
  }
  
  //
  // Checks whether the first arg equals ALL of subsequent args
  //
  template <typename T1, typename T2, typename ...Ts>
  inline constexpr bool eq_all(T1&& val, T2&& arg1, Ts&& ...args) noexcept
    requires detail::is_eq_comparable_v<T1, T2, Ts... >
  {
    return ((val == arg1) && ... && (val == args));
  }

  //
  // Checks whether the first arg equals ANY of subsequent args
  //
  template <typename T1, typename T2, typename ...Ts>
  inline constexpr bool eq_any(T1&& val, T2&& arg1, Ts&& ...args) noexcept
    requires detail::is_eq_comparable_v<T1, T2, Ts... >
  {
    return ((val == arg1) || ... || (val == args));
  }

  //
  // Checks whether the first arg equals NONE of subsequent args
  //
  template <typename T1, typename T2, typename ...Ts>
  inline constexpr bool eq_none(T1&& val, T2&& arg1, Ts&& ...args) noexcept
    requires detail::is_eq_comparable_v<T1, T2, Ts... >
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
    requires (  !std::is_rvalue_reference_v<decltype(val)>
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