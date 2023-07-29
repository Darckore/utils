#pragma once

namespace utils
{
  //
  // A visitor for std::variant and the like
  // 
  // Usage:
  // utils::visitor v{
  //   [](type1 t1) { .... },
  //   [](type2 t2) { .... }.
  //   ....
  // };
  // 
  // std::visit(&var, v);
  //
  template<typename... Bases>
  struct visitor : Bases...
  {
    using Bases::operator()...;
  };


  //
  // A simple way to shut up the compiler when something is unused.
  // Just a convenience for debugging
  //
  template <typename ...Args>
  constexpr void unused(Args&&...) noexcept {}

  namespace detail
  {
    using std::begin;
    using std::end;
    using std::rbegin;
    using std::rend;

    //
    // Checks whether we've got an iterable range
    //
    template <typename T>
    concept iterable = requires(T a)
    {
      begin(a);
      end(a);
    };

    //
    // Checks whether we've got a reverse iterable range
    //
    template <typename T>
    concept r_iterable = requires(T a)
    {
      rbegin(a);
      rend(a);
    };

    //
    // Checks whether a range can be hashed
    //
    template <typename T>
    concept hashable_range = requires(T rng)
    {
      requires iterable<T>;
      static_cast<unsigned char>(*begin(rng));
    };

    //
    // Checks whether we've got an iterator from a hashable range
    //
    template <typename I>
    concept hashable_it = requires(I it)
    {
      requires std::forward_iterator<I>;
      static_cast<unsigned char>(std::declval<typename std::iterator_traits<I>::value_type>());
    };

    inline constexpr auto hash_prime = 1099511628211ull;
    inline constexpr auto hash_off   = 14695981039346656037ull;
  }

  //
  // Checks whether a value is in the specified range
  //
  template <comparable T1, comparable T2, comparable T3>
  inline constexpr bool in_range(T1&& val, T2&& low, T3&& high) noexcept
  {
    return val >= low && val <= high;
  }

  //
  // Checks whether the first arg equals ALL of subsequent args
  //
  template <typename T1, typename T2, typename ...Ts> requires equ_comparable<T1, T2, Ts... >
  inline constexpr bool eq_all(T1&& val, T2&& arg1, Ts&& ...args) noexcept
  {
    return ((val == arg1) && ... && (val == args));
  }

  //
  // Checks whether the first arg equals ANY of subsequent args
  //
  template <typename T1, typename T2, typename ...Ts> requires equ_comparable<T1, T2, Ts... >
  inline constexpr bool eq_any(T1&& val, T2&& arg1, Ts&& ...args) noexcept
  {
    return ((val == arg1) || ... || (val == args));
  }

  //
  // Checks whether the first arg equals NONE of subsequent args
  //
  template <typename T1, typename T2, typename ...Ts> requires equ_comparable<T1, T2, Ts... >
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

  //
  // Hashes a range of bytes via some fnv magic
  //
  template <detail::hashable_it Beg, typename End>
  constexpr auto hash(Beg first, End last) noexcept
  {
    constexpr auto prime  = detail::hash_prime;
    constexpr auto offset = detail::hash_off;
    using hash_type = std::remove_const_t<decltype(offset)>;
    using byte_type = std::uint8_t;

    constexpr auto cast = [](auto cur)
    {
      return static_cast<hash_type>(static_cast<byte_type>(cur));
    };

    auto hash = offset;
    while (first != last)
    {
      hash = (hash ^ cast(*first++)) * prime;
    }

    return hash;
  }

  //
  // Hashes a range of bytes via some fnv magic
  //
  template <detail::hashable_range R>
  constexpr auto hash(R&& range) noexcept
  {
    using std::begin;
    using std::end;
    return hash(begin(range), end(range));
  }

  //
  // Hashes any arithmetic type
  //
  template <typename T> requires (std::is_arithmetic_v<T>)
  auto hash(T val) noexcept
  {
    auto ptr = reinterpret_cast<const std::uint8_t*>(&val);
    return hash(ptr, ptr + sizeof(val));
  }

  //
  // Hashes any enum type
  //
  template <typename T> requires (std::is_enum_v<T>)
  auto hash(T val) noexcept
  {
    return hash(static_cast<std::underlying_type_t<T>>(val));
  }
}