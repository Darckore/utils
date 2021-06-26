#pragma once

#define CLASS_SPECIALS_ALL(clName)\
  clName() = default;\
  clName(const clName&) = default;\
  clName& operator=(const clName&) = default;\
  clName(clName&&) = default;\
  clName& operator=(clName&&) = default

#define CLASS_SPECIALS_ALL_CUSTOM(clName)\
  clName(const clName&) = default;\
  clName& operator=(const clName&) = default;\
  clName(clName&&) = default;\
  clName& operator=(clName&&) = default

#define CLASS_SPECIALS_NOCOPY(clName)\
  clName() = default;\
  clName(const clName&) = delete;\
  clName& operator=(const clName&) = delete;\
  clName(clName&&) = default;\
  clName& operator=(clName&&) = default

#define CLASS_SPECIALS_NOCOPY_CUSTOM(clName)\
  clName(const clName&) = delete;\
  clName& operator=(const clName&) = delete;\
  clName(clName&&) = default;\
  clName& operator=(clName&&) = default

#define CLASS_SPECIALS_NODEFAULT(clName)\
  clName() = delete;\
  clName(const clName&) = default;\
  clName& operator=(const clName&) = default;\
  clName(clName&&) = default;\
  clName& operator=(clName&&) = default

#define CLASS_SPECIALS_NODEFAULT_NOCOPY(clName)\
  clName() = delete;\
  clName(const clName&) = delete;\
  clName& operator=(const clName&) = delete;\
  clName(clName&&) = default;\
  clName& operator=(clName&&) = default

#define CLASS_SPECIALS_NONE(clName)\
  clName() = delete;\
  clName(const clName&) = delete;\
  clName& operator=(const clName&) = delete;\
  clName(clName&&) = delete;\
  clName& operator=(clName&&) = delete

#define CLASS_SPECIALS_NONE_CUSTOM(clName)\
  clName(const clName&) = delete;\
  clName& operator=(const clName&) = delete;\
  clName(clName&&) = delete;\
  clName& operator=(clName&&) = delete

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
  void unused(Args&&...) noexcept {}

  namespace detail
  {
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
    requires detail::equ_comparable<T1, T2, Ts... >
  {
    return ((val == arg1) && ... && (val == args));
  }

  //
  // Checks whether the first arg equals ANY of subsequent args
  //
  template <typename T1, typename T2, typename ...Ts>
  inline constexpr bool eq_any(T1&& val, T2&& arg1, Ts&& ...args) noexcept
    requires detail::equ_comparable<T1, T2, Ts... >
  {
    return ((val == arg1) || ... || (val == args));
  }

  //
  // Checks whether the first arg equals NONE of subsequent args
  //
  template <typename T1, typename T2, typename ...Ts>
  inline constexpr bool eq_none(T1&& val, T2&& arg1, Ts&& ...args) noexcept
    requires detail::equ_comparable<T1, T2, Ts... >
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