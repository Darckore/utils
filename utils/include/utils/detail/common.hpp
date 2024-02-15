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

  //
  // Returns the number of bits in a byte
  //
  consteval auto byte_size() noexcept
  {
    return std::numeric_limits<std::underlying_type_t<std::byte>>::digits;
  }

  //
  // Returns the given byte shifted left to the given position
  // and cast to the specified type
  //
  template <integer Res, std::size_t Pos>
  constexpr auto shift_byte(std::byte value) noexcept
  {
    using ures = std::make_unsigned_t<Res>;
    static_assert(Pos < sizeof(ures));
    return std::bit_cast<Res>(static_cast<ures>(value) << (Pos * byte_size()));
  }


  //
  // Holds an obfuscated pointer which can be used to store some data
  // in the most significant byte (safe in user space)
  // 
  // Also provides a pointer-like interface into the stored ptr
  // WILL NOT check for nullptr dereferences, this is on the user
  //
  template <typename T>
  class mangled_ptr final
  {
  public:
    using value_type      = T;
    using pointer         = value_type*;
    using const_pointer   = const value_type*;
    using reference       = value_type&;
    using const_reference = const value_type&;
    using stored_type     = std::uintptr_t;
    using byte_type       = std::byte;

  private:
    static consteval auto filled_byte() noexcept
    {
      return byte_type{ 0xFF };
    }
    static consteval auto msb_idx() noexcept
    {
      return sizeof(stored_type) - 1;
    }
    static constexpr auto shifted_byte(byte_type msb) noexcept
    {
      return shift_byte<stored_type, msb_idx()>(msb);
    }
    static constexpr auto baseline() noexcept
    {
      return shifted_byte(filled_byte());
    }
    static constexpr auto inv_baseline() noexcept
    {
      return ~baseline();
    }

  private:
    static constexpr auto from_ptr(pointer ptr) noexcept
    {
      return reinterpret_cast<stored_type>(ptr);
    }
    static constexpr auto to_ptr(stored_type val) noexcept
    {
      return reinterpret_cast<pointer>(val);
    }

    static constexpr auto drop_msb(stored_type val) noexcept
    {
      return val & inv_baseline();
    }
    static constexpr auto apply_msb(stored_type val, byte_type msb) noexcept
    {
      return val | shifted_byte(msb);
    }

    static constexpr auto mangle(pointer ptr, byte_type msb) noexcept
    {
      return apply_msb(from_ptr(ptr), msb);
    }
    static constexpr auto demangle(stored_type val) noexcept
    {
      return to_ptr(drop_msb(val));
    }
    static constexpr auto get_msb(stored_type val) noexcept
    {
      return static_cast<byte_type>((val & baseline()) >> (msb_idx() * byte_size()));
    }

  public:
    CLASS_SPECIALS_ALL(mangled_ptr);

    constexpr mangled_ptr(pointer ptr, byte_type msb) noexcept :
      m_value{ mangle(ptr, msb) }
    {}

    explicit constexpr mangled_ptr(pointer ptr) noexcept :
      mangled_ptr{ ptr, {} }
    {}

    CLASS_DEFAULT_EQ(mangled_ptr);

    constexpr mangled_ptr& operator=(pointer ptr) noexcept
    {
      const auto msb = get_msb();
      return reset(ptr, msb);
    }

    friend constexpr bool operator==(const mangled_ptr& ptr, const_pointer other) noexcept
    {
      return ptr.get() == other;
    }

  public:
    constexpr stored_type stored() const noexcept
    {
      return m_value;
    }
    constexpr byte_type get_msb() const noexcept
    {
      return get_msb(stored());
    }
    constexpr auto is_mangled() const noexcept
    {
      return get_msb() != byte_type{};
    }
    constexpr const_pointer get() const noexcept
    {
      return demangle(stored());
    }
    constexpr pointer get() noexcept
    {
      return FROM_CONST(get);
    }

    explicit constexpr operator bool() const noexcept
    {
      return static_cast<bool>(get());
    }
    constexpr const_reference operator*() const noexcept
    {
      UTILS_ASSERT(operator bool());
      return *get();
    }
    constexpr reference operator*() noexcept
    {
      return FROM_CONST(operator*);
    }
    constexpr const_pointer operator->() const noexcept
    {
      UTILS_ASSERT(operator bool());
      return get();
    }
    constexpr pointer operator->() noexcept
    {
      return FROM_CONST(operator->);
    }

    auto replace(pointer ptr, byte_type msb) const noexcept
    {
      return mangled_ptr{ ptr, msb };
    }
    auto replace(pointer ptr) const noexcept
    {
      return mangled_ptr{ ptr };
    }

    mangled_ptr& reset(pointer ptr, byte_type msb) noexcept
    {
      *this = replace(ptr, msb);
      return *this;
    }
    mangled_ptr& reset(pointer ptr) noexcept
    {
      *this = replace(ptr);
      return *this;
    }
    mangled_ptr& reset() noexcept
    {
      return reset({});
    }

  private:
    stored_type m_value{};
  };

  template <typename P, typename B>
  mangled_ptr(P*, B) -> mangled_ptr<P>;
}


template <typename T>
struct std::hash<utils::mangled_ptr<T>>
{
  constexpr auto operator()(const utils::mangled_ptr<T>& mp) const noexcept
  {
    return mp.stored();
  }
};