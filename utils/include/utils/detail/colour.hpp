#pragma once

namespace utils
{
  //
  // RGBA colour
  //
  class colour
  {
  public:
    using value_type = std::uint32_t;
    using byte_type  = std::uint8_t;
    using float_type = float;
    using cmp_array  = std::array<float_type, 4>;

    enum class component : byte_type
    {
      cR = 0,
      cG = 8,
      cB = 16,
      cA = 24
    };

    using enum component;

  public:
    CLASS_SPECIALS_ALL(colour);

    constexpr colour(byte_type r, byte_type g, byte_type b, byte_type a = 255) noexcept :
      colour{ from_rgba(r, g, b, a) }
    {}

    constexpr explicit colour(value_type c) noexcept :
      m_val{ c }
    { }
    
    constexpr colour& operator=(value_type c) noexcept
    {
      m_val = c;
      return *this;
    }
    constexpr operator value_type() const noexcept
    {
      return m_val;
    }

    bool operator==(const colour& other) const noexcept = default;

    friend constexpr colour operator&(colour l, colour r) noexcept
    {
      return l & r.m_val;
    }
    friend constexpr colour operator&(colour l, colour::value_type r) noexcept
    {
      return colour{ l.m_val & r };
    }
    friend constexpr colour operator|(colour l, colour r) noexcept
    {
      return l | r.m_val;
    }
    friend constexpr colour operator|(colour l, colour::value_type r) noexcept
    {
      return colour{ l.m_val | r };
    }
    friend constexpr colour operator^(colour l, colour r) noexcept
    {
      return l ^ r.m_val;
    }
    friend constexpr colour operator^(colour l, colour::value_type r) noexcept
    {
      return colour{ l.m_val ^ r };
    }

    constexpr colour& operator&=(value_type other) noexcept
    {
      *this = other & m_val;
      return *this;
    }
    constexpr colour& operator|=(value_type other) noexcept
    {
      *this = other | m_val;
      return *this;
    }
    constexpr colour& operator^=(value_type other) noexcept
    {
      *this = other ^ m_val;
      return *this;
    }

    constexpr colour operator~() const noexcept
    {
      return colour{ ~m_val };
    }

  public:
    template <component CMP>
    constexpr auto get() const noexcept
    {
      constexpr auto shift = static_cast<byte_type>(CMP);
      constexpr auto mask = value_type{ 0xFF }  << shift;
      return static_cast<byte_type>((m_val & mask) >> shift);
    }
    constexpr auto red() const noexcept
    {
      return get<cR>();
    }
    constexpr auto green() const noexcept
    {
      return get<cG>();
    }
    constexpr auto blue() const noexcept
    {
      return get<cB>();
    }
    constexpr auto alpha() const noexcept
    {
      return get<cA>();
    }

    template <component CMP>
    constexpr auto norm() const noexcept
    {
      return static_cast<float_type>(get<CMP>()) / 0xFF;
    }

    template <component CMP>
    constexpr colour& set(byte_type c) noexcept
    {
      constexpr auto shift   = static_cast<uint8_t>(CMP);
      constexpr auto ones    = value_type{ 0xFFFFFFFF };
      constexpr auto cmpFlag = value_type{ 0x000000FF };
      constexpr auto mask = ~(ones & (cmpFlag << shift));
      m_val &= mask;
      m_val |= static_cast<value_type>(c) << shift;
      return *this;
    }
    constexpr colour& set_red(byte_type c) noexcept
    {
      return set<cR>(c);
    }
    constexpr colour& set_green(byte_type c) noexcept
    {
      return set<cG>(c);
    }
    constexpr colour& set_blue(byte_type c) noexcept
    {
      return set<cB>(c);
    }
    constexpr colour& set_alpha(byte_type c) noexcept
    {
      return set<cA>(c);
    }

    constexpr auto to_float_norm() const noexcept
    {
      return std::array { norm<cR>(), norm<cG>(), norm<cB>(), norm<cA>() };
    }

  public:
    static constexpr colour from_rgba(byte_type r, byte_type g, byte_type b, byte_type a) noexcept
    {
      value_type res{};
      res |= static_cast<value_type>(r) << static_cast<byte_type>(cR);
      res |= static_cast<value_type>(g) << static_cast<byte_type>(cG);
      res |= static_cast<value_type>(b) << static_cast<byte_type>(cB);
      res |= static_cast<value_type>(a) << static_cast<byte_type>(cA);
      return colour{ res };
    }
    static constexpr colour from_rgba(float_type r, float_type g, float_type b, float_type a) noexcept
    {
      constexpr auto denorm = [](float_type cmp) noexcept
      {
        return static_cast<byte_type>(cmp * 0xFF);
      };

      return colour{ denorm(r), denorm(g), denorm(b), denorm(a) };
    }
    static constexpr colour from_norm(cmp_array components) noexcept
    {
      return from_rgba(components[0], components[1], components[2], components[3]);
    }

  private:
    value_type m_val{};
  };

  namespace colours
  {
    inline constexpr auto white     = colour{ 255, 255, 255 };
    inline constexpr auto black     = colour{   0,   0,   0 };
    inline constexpr auto gray      = colour{ 128, 128, 128 };
    inline constexpr auto silver    = colour{ 192, 192, 192 };
    inline constexpr auto red       = colour{ 255,   0,   0 };
    inline constexpr auto orange    = colour{ 255, 165,   0 };
    inline constexpr auto lime      = colour{   0, 255,   0 };
    inline constexpr auto green     = colour{   0, 128,   0 };
    inline constexpr auto olive     = colour{ 128, 128,   0 };
    inline constexpr auto blue      = colour{   0,   0, 255 };
    inline constexpr auto darkBlue  = colour{   0,   0, 160 };
    inline constexpr auto yellow    = colour{ 255, 255,   0 };
    inline constexpr auto purple    = colour{ 128,   0, 128 };

    inline constexpr auto magenta   = colour{ 255,   0, 255 };
    inline constexpr auto chromakey = magenta;
  }
}