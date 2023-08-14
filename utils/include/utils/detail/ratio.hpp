#pragma once

namespace utils
{
  //
  // A ratio (ordinary fraction)
  // Supports all basic math operations
  //
  template <integer T>
  class ratio
  {
  public:
    using value_type = T;
    using sign_t     = std::int8_t;

  public:
    CLASS_SPECIALS_NODEFAULT(ratio);

    explicit constexpr ratio(value_type num) noexcept :
      ratio{ num, 1, 0 }
    {}

    constexpr ratio(value_type num, value_type den) noexcept :
      ratio{ num, den, 0 }
    {}

    constexpr ratio(value_type num, value_type den, sign_t sgn) noexcept :
      m_num{ num },
      m_den{ den },
      m_sign{ sgn }
    {
      normalise();
    }

  public:
    explicit operator bool() const noexcept
    {
      return num() != 0;
    }

    constexpr auto operator-() const noexcept
    {
      return ratio{ num(), denom(), static_cast<sign_t>(-sign()) };
    }
    constexpr auto operator+() const noexcept
    {
      return *this;
    }

    template <integer I>
    constexpr auto to() const noexcept
    {
      return ratio<I>{ static_cast<I>(num()), static_cast<I>(denom()), sign() };
    }

    template <real R>
    constexpr auto to() const noexcept
    {
      return sign() * (is_infinity()
        ? std::numeric_limits<R>::infinity()
        : static_cast<R>(num()) / denom());
    }

    template <integer I>
    constexpr auto add(const ratio<I>& other) const noexcept
    {
      using common_t = std::common_type_t<value_type, I>;
      using common_signed_t = std::make_signed_t<common_t>;
      const auto l = get_simplified().template to<common_t>();
      const auto r = other.get_simplified().template to<common_t>();
      const auto res_denom = l.denom() * r.denom();
      const auto lnum = l.sign() * static_cast<common_signed_t>(l.num() * r.denom());
      const auto rnum = r.sign() * static_cast<common_signed_t>(r.num() * l.denom());
      const auto res_num = lnum + rnum;
      return ratio<common_t>
      {
        static_cast<common_t>(utils::abs(res_num)),
          res_denom,
          static_cast<sign_t>(utils::sign(res_num))
      }.simplify();
    }
    template <integer I>
    constexpr auto sub(const ratio<I>& other) const noexcept
    {
      return add(-other);
    }
    template <integer I>
    constexpr auto mul(const ratio<I>& other) const noexcept
    {
      using common_t = std::common_type_t<value_type, I>;
      const auto l = get_simplified().template to<common_t>();
      const auto r = other.get_simplified().template to<common_t>();
      return ratio<common_t>
      {
        l.num() * r.num(),
        l.denom() * r.denom(),
        static_cast<sign_t>(l.sign() * r.sign())
      }.get_simplified();
    }
    template <integer I>
    constexpr auto div(const ratio<I>& other) const noexcept
    {
      return mul(other.get_flipped());
    }

    template <integer I>
    constexpr ratio& operator+=(const ratio<I>& other) noexcept
    {
      *this = (*this + other).template to<value_type>();
      return *this;
    }
    template <integer I>
    constexpr ratio& operator+=(const I& other) noexcept
    {
      *this = (*this + other).template to<value_type>();
      return *this;
    }

    template <integer I>
    constexpr ratio& operator-=(const ratio<I>& other) noexcept
    {
      *this = (*this - other).template to<value_type>();
      return *this;
    }
    template <integer I>
    constexpr ratio& operator-=(const I& other) noexcept
    {
      *this = (*this - other).template to<value_type>();
      return *this;
    }

    template <integer I>
    constexpr ratio& operator*=(const ratio<I>& other) noexcept
    {
      *this = (*this * other).template to<value_type>();
      return *this;
    }
    template <integer I>
    constexpr ratio& operator*=(const I& other) noexcept
    {
      *this = (*this * other).template to<value_type>();
      return *this;
    }

    template <integer I>
    constexpr ratio& operator/=(const ratio<I>& other) noexcept
    {
      *this = (*this / other).template to<value_type>();
      return *this;
    }
    template <integer I>
    constexpr ratio& operator/=(const I& other) noexcept
    {
      *this = (*this / other).template to<value_type>();
      return *this;
    }

    constexpr ratio  get_simplified() const noexcept
    {
      const auto gcd = std::gcd(num(), denom());
      return ratio{ num() / gcd, denom() / gcd, sign() };
    }
    constexpr ratio& simplify() noexcept
    {
      *this = get_simplified();
      return *this;
    }
    constexpr ratio  get_flipped() const noexcept
    {
      return ratio{ denom(), num(), sign() };
    }
    constexpr ratio& flip() noexcept
    {
      *this = get_flipped();
      return *this;
    }

  public:
    constexpr bool is_infinity() const noexcept
    {
      return denom() == 0;
    }
    constexpr value_type num() const noexcept
    {
      return m_num;
    }
    constexpr value_type denom() const noexcept
    {
      return m_den;
    }
    constexpr sign_t sign() const noexcept
    {
      return m_sign;
    }

  private:
    constexpr void normalise() noexcept
    {
      const auto num_s = static_cast<sign_t>(utils::sign(num()));
      const auto den_s = static_cast<sign_t>(utils::sign(denom()));
      m_num = utils::abs(num());
      m_den = utils::abs(denom());
      const auto res_sign = static_cast<sign_t>(num_s * den_s);

      if (is_infinity())
      {
        m_sign *= num_s;
        if (m_sign == 0)
          m_sign = num_s;
        return;
      }

      if (m_sign == 0)
        m_sign = res_sign;
      else
        m_sign *= res_sign;
    }

  private:
    value_type m_num{};
    value_type m_den{};
    sign_t m_sign{};
  };

  template <integer I>
  constexpr auto sign(ratio<I> r) noexcept
  {
    return r.sign();
  }

  template <integer I>
  constexpr auto abs(ratio<I> r) noexcept
  {
    return ratio<I>{ r.num(), r.denom() };
  }

  template <integer I>
  constexpr auto inv(ratio<I> r) noexcept
  {
    return r.get_flipped();
  }

  template <integer L, integer R>
  constexpr bool operator==(const ratio<L>& lhs, const ratio<R>& rhs) noexcept
  {
    if (lhs.sign() != rhs.sign())
      return false;

    using common_t = std::common_type_t<L, R>;
    const auto l = lhs.get_simplified().template to<common_t>();
    const auto r = rhs.get_simplified().template to<common_t>();

    return l.sign()  == r.sign()
        && l.num()   == r.num()
        && l.denom() == r.denom();
  }
  template <integer L, integer R>
  constexpr bool operator==(const ratio<L>& lhs, const R& rhs) noexcept
  {
    return lhs == ratio<R>{ rhs };
  }
  template <integer L, real R>
  constexpr bool operator==(const ratio<L>& lhs, const R& rhs) noexcept
  {
    return eq(lhs.template to<R>(), rhs);
  }

  template <integer L, integer R>
  constexpr std::strong_ordering operator<=>(const ratio<L>& lhs, const ratio<R>& rhs) noexcept
  {
    if (const auto cmp = lhs.sign() <=> rhs.sign(); cmp != 0)
      return cmp;

    using common_t = std::common_type_t<L, R>;
    const auto l = lhs.get_simplified().template to<common_t>();
    const auto r = rhs.get_simplified().template to<common_t>();
    return (l.num() * r.denom()) <=> (r.num() * l.denom());
  }
  template <integer L, integer R>
  constexpr std::strong_ordering operator<=>(const ratio<L>& lhs, const R& rhs) noexcept
  {
    if (const auto lsign = lhs.sign(); rhs == 0)
      return lsign <=> static_cast<decltype(lsign)>(sign(rhs));

    return lhs <=> ratio<R>{ rhs };
  }
  template <integer L, real R>
  constexpr auto operator<=>(const ratio<L>& lhs, const R& rhs) noexcept
  {
    return lhs.template to<R>() <=> rhs;
  }

  template <integer L, integer R>
  constexpr auto operator+(const ratio<L>& lhs, const ratio<R>& rhs) noexcept
  {
    return lhs.add(rhs);
  }
  template <integer L, integer R>
  constexpr auto operator+(const ratio<L>& lhs, const R& rhs) noexcept
  {
    return lhs.add(ratio<R>{ rhs });
  }
  template <integer L, integer R>
  constexpr auto operator+(const L& lhs, const ratio<R>& rhs) noexcept
  {
    return ratio<L>{ lhs }.add(rhs);
  }
  template <integer L, real R>
  constexpr auto operator+(const ratio<L>& lhs, const R& rhs) noexcept
  {
    return lhs.template to<R>() + rhs;
  }
  template <real L, integer R>
  constexpr auto operator+(const L& lhs, const ratio<R>& rhs) noexcept
  {
    return lhs + rhs.template to<L>();
  }

  template <integer L, integer R>
  constexpr auto operator-(const ratio<L>& lhs, const ratio<R>& rhs) noexcept
  {
    return lhs.sub(rhs);
  }
  template <integer L, integer R>
  constexpr auto operator-(const ratio<L>& lhs, const R& rhs) noexcept
  {
    return lhs.sub(ratio<R>{ rhs });
  }
  template <integer L, integer R>
  constexpr auto operator-(const L& lhs, const ratio<R>& rhs) noexcept
  {
    return ratio<L>{ lhs }.sub(rhs);
  }
  template <integer L, real R>
  constexpr auto operator-(const ratio<L>& lhs, const R& rhs) noexcept
  {
    return lhs.template to<R>() - rhs;
  }
  template <real L, integer R>
  constexpr auto operator-(const L& lhs, const ratio<R>& rhs) noexcept
  {
    return lhs - rhs.template to<L>();
  }

  template <integer L, integer R>
  constexpr auto operator*(const ratio<L>& lhs, const ratio<R>& rhs) noexcept
  {
    return lhs.mul(rhs);
  }
  template <integer L, integer R>
  constexpr auto operator*(const ratio<L>& lhs, const R& rhs) noexcept
  {
    return lhs.mul(ratio<R>{ rhs });
  }
  template <integer L, integer R>
  constexpr auto operator*(const L& lhs, const ratio<R>& rhs) noexcept
  {
    return ratio<L>{ lhs }.mul(rhs);
  }
  template <integer L, real R>
  constexpr auto operator*(const ratio<L>& lhs, const R& rhs) noexcept
  {
    return lhs.template to<R>() * rhs;
  }
  template <real L, integer R>
  constexpr auto operator*(const L& lhs, const ratio<R>& rhs) noexcept
  {
    return lhs * rhs.template to<L>();
  }

  template <integer L, integer R>
  constexpr auto operator/(const ratio<L>& lhs, const ratio<R>& rhs) noexcept
  {
    return lhs.div(rhs);
  }
  template <integer L, integer R>
  constexpr auto operator/(const ratio<L>& lhs, const R& rhs) noexcept
  {
    return lhs.div(ratio<R>{ rhs });
  }
  template <integer L, integer R>
  constexpr auto operator/(const L& lhs, const ratio<R>& rhs) noexcept
  {
    return ratio<L>{ lhs }.div(rhs);
  }
  template <integer L, real R>
  constexpr auto operator/(const ratio<L>& lhs, const R& rhs) noexcept
  {
    return lhs.template to<R>() / rhs;
  }
  template <real L, integer R>
  constexpr auto operator/(const L& lhs, const ratio<R>& rhs) noexcept
  {
    return lhs / rhs.template to<L>();
  }
}