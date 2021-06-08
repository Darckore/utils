#pragma once
/* 
 * This thing is to be included somewhere.
 * Probably, the "somewhere" in question has precompiled headers and stuff.
 * std includes are intentionally absent here, I assume they come from someplace
 * and are perfectly accessible on compilation.
*/

#pragma region Macros for special class members
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
#pragma endregion

#pragma region Debug macros
// This is for Visual Studio only
#if !defined(NDEBUG) && defined(_WIN32)
  #define BREAK(cond) (void)((!(cond)) || ((__debugbreak()), 0))
#else
  #define BREAK(cond) static_assert(false, "You can only use this in MSVC under debug")
#endif
#pragma endregion

namespace utils
{
  //
  // A simple way to shut up the compiler when something is unused.
  // Just a convenience for debuggin
  //
  template <typename ...Args>
  void unused(Args&&...) noexcept {}

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
  decltype(auto) mutate(T&& val) noexcept
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