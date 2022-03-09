#pragma once

namespace utils
{
  namespace detail
  {
    template <typename T>
    concept coordinate = integer<T> || real<T>;
  }

  //
  // A vector of arbitrary dimensions
  // This is the math vector representing
  // a point in space relative to the origin
  //
  template <detail::coordinate T, std::size_t D>
  class vector
  {
  public:
    using value_type = T;
    static constexpr auto dimensions = D;
    using storage_type = std::array<value_type, dimensions>;
    using size_type = storage_type::size_type;

    static constexpr auto zero_coord = value_type{ 0 };

    template <size_type Axis> requires (Axis < dimensions)
    static consteval auto axis_norm() noexcept
    {
      vector <value_type, dimensions> res;
      res.get<Axis>() = value_type{ 1 };
      return res;
    }

  public:
    CLASS_SPECIALS_ALL(vector);

    template <detail::coordinate... V> requires detail::homogenous_pack<value_type, V...>
    constexpr vector(value_type first, V... values) noexcept :
      m_coords{ first, values... }
    { }

    template <detail::coordinate First, detail::coordinate... Rest> requires (sizeof...(Rest) <= dimensions - 1)
    constexpr vector(First first, Rest ...values) noexcept :
      m_coords{ static_cast<value_type>(first), static_cast<value_type>(values)... }
    { }

    template <detail::coordinate U, size_type N>
    constexpr bool operator==(const vector<U, N>& other) const noexcept
    {
      if constexpr (N != dimensions)
      {
        return false;
      }
      else
      {
        return eq(other);
      }
    }

    constexpr auto operator-() const noexcept
    {
      vector result;
      std::transform(m_coords.begin(), m_coords.end(), result.m_coords.begin(), std::negate<>{});
      return result;
    }

    template <detail::coordinate U>
    constexpr auto operator+(const vector<U, dimensions>& other) const noexcept
    {
      using ct = std::common_type_t<U, value_type>;
      vector<ct, dimensions> dest;
      auto thisIt  = begin();
      auto otherIt = other.begin();
      std::transform(dest.begin(), dest.end(), dest.begin(),
                     [&thisIt, &otherIt](auto)
                     {
                       return ct{ *(thisIt++) + *(otherIt++) };
                     });
      return dest;
    }

    template <detail::coordinate U, size_type N>
      requires (std::is_same_v<U, value_type> && N == dimensions)
    constexpr auto& operator+=(const vector<U, N>& other) noexcept
    {
      *this = *this + other;
      return *this;
    }

    template <detail::coordinate U>
    constexpr auto operator-(const vector<U, dimensions>& other) const noexcept
    {
      using ct = std::common_type_t<U, value_type>;
      vector<ct, dimensions> dest;
      auto thisIt  = begin();
      auto otherIt = other.begin();
      std::transform(dest.begin(), dest.end(), dest.begin(),
                     [&thisIt, &otherIt](auto)
                     {
                       return ct{ *(thisIt++) - *(otherIt++) };
                     });
      return dest;
    }

    template <detail::coordinate U, size_type N>
      requires (std::is_same_v<U, value_type> && N == dimensions)
    constexpr auto& operator-=(const vector<U, N>& other) noexcept
    {
      *this = *this - other;
      return *this;
    }

    template <detail::coordinate U>
    constexpr auto operator*(const vector<U, dimensions>& other) const noexcept
    {
      using ct = std::common_type_t<U, value_type>;
      auto it = other.begin();
      return std::accumulate(m_coords.begin(), m_coords.end(), ct{},
                             [&it](auto acc, auto cur)
                             {
                               return acc + ct{ cur * *(it++) };
                             });
    }

    constexpr auto& operator*=(detail::coordinate auto scalar) noexcept
    {
      return scale(scalar);
    }
    constexpr auto& operator/=(detail::coordinate auto scalar) noexcept
    {
      return scale(value_type{ 1 } / scalar);
    }

    template <detail::coordinate U, size_type N>
    constexpr auto to() const noexcept
    {
      if constexpr (N == dimensions && std::is_same_v<value_type, U>)
      {
        return *this;
      }
      else
      {
        vector<U, N> dest;
        auto destIt = dest.begin();
        ranges::for_each_n(begin(), std::min(dimensions, N), [&destIt](auto val)
                           {
                             *(destIt++) = static_cast<U>(val);
                           });

        return dest;
      }
    }

    template <detail::coordinate U, size_type N>
    constexpr operator vector<U, N>() const noexcept
    {
      return to<U, N>();
    }

  public:
    constexpr auto len_sq() const noexcept
    {
      return std::accumulate(begin(), end(), zero_coord,
                             [](auto acc, auto cur)
                             {
                               return acc + cur * cur;
                             });
    }
    constexpr auto len() const noexcept
    {
      const auto lsq = len_sq();
      return lsq * inv_sqrt(lsq);
    }

    constexpr auto get_scaled(detail::coordinate auto scalar) const noexcept
    {
      vector result;
      std::transform(begin(), end(), result.begin(),
                     [scalar](auto cur)
                     {
                       return static_cast<value_type>(cur * scalar);
                     });
      return result;
    }
    constexpr auto& scale(detail::coordinate auto scalar) noexcept
    {
      *this = get_scaled(scalar);
      return *this;
    }

    constexpr auto get_rotated(detail::real auto angle) const noexcept requires (dimensions >= 2)
    {
      vector<value_type, dimensions> res;
      const auto cosA = cos(angle);
      const auto sinA = sin(angle);

      // Special case for 2D
      if constexpr (dimensions == 2)
      {
        auto&& x = get<0>();
        auto&& y = get<1>();
        res.template get<0>() = x * cosA - y * sinA;
        res.template get<1>() = x * sinA + y * cosA;
        return res;
      }
      else
      {
        // Todo: higher dimensions
        unused(angle, res);
        return *this;
      }
    }
    constexpr auto& rotate(detail::real auto angle) noexcept
    {
      *this = get_rotated(angle);
      return *this;
    }

    constexpr auto get_normalised() const noexcept
    {
      return *this / len();
    }
    constexpr auto& normalise() noexcept
    {
      *this = get_normalised();
      return *this;
    }

    //
    // Cross product
    // Only supported in 2 and 3 dimensions
    //
    template <detail::coordinate U> requires (eq_any(dimensions, 2, 3))
    constexpr auto cross(const vector<U, dimensions>& other) const noexcept
    {
      // Special case for 2D - we get a number
      if constexpr (dimensions == 2)
      {
        return get<0>() * other.template get<1>() - get<1>() * other.template get<0>();
      }
      else
      {
        using ct = std::common_type_t<U, value_type>;
        const auto c0 = ct{ get<1>() * other.template get<2>() - get<2>() * other.template get<1>() };
        const auto c1 = ct{ get<2>() * other.template get<0>() - get<0>() * other.template get<2>() };
        const auto c2 = ct{ get<0>() * other.template get<1>() - get<1>() * other.template get<0>() };
        return vector<ct, dimensions>{ c0, c1, c2 };
      }
    }

    template <size_type N> requires (N < dimensions)
    constexpr const auto& get() const noexcept
    {
      return m_coords[N];
    }
    template <size_type N> requires (N < dimensions)
    constexpr auto& get() noexcept
    {
      return mutate(std::as_const(*this).get<N>());
    }

    constexpr auto begin() const noexcept
    {
      return m_coords.begin();
    }
    constexpr auto end() const noexcept
    {
      return m_coords.end();
    }
    constexpr auto begin() noexcept
    {
      return m_coords.begin();
    }
    constexpr auto end() noexcept
    {
      return m_coords.end();
    }

  private:
    template <typename U>
    constexpr bool eq(const vector<U, dimensions>& other) const noexcept
    {
      using ct = std::common_type_t<U, value_type>;
      auto otherIt = other.begin();
      for (auto it = begin(); it != end(); ++it, ++otherIt)
      {
        if (!utils::eq(static_cast<ct>(*it), static_cast<ct>(*otherIt)))
          return false;
      }
      return true;
    }

  private:
    storage_type m_coords{};
  };

  template <detail::coordinate T, std::size_t N, detail::coordinate S>
  constexpr auto operator*(const vector<T, N>& vec, const S& scalar) noexcept
  {
    return vec.get_scaled(scalar);
  }
  template <detail::coordinate T, std::size_t N, detail::coordinate S>
  constexpr auto operator*(const S& scalar, const vector<T, N>& vec) noexcept
  {
    return vec * scalar;
  }
  template <detail::coordinate T, std::size_t N, detail::coordinate S>
  constexpr auto operator/(const vector<T, N>& vec, const S& scalar) noexcept
  {
    return vec.get_scaled(T{ 1 } / scalar);
  }

  template <detail::coordinate First, detail::coordinate... Rest>
  vector(First, Rest...)->vector<First, sizeof...(Rest) + 1>;

  template <detail::coordinate T, std::size_t N>
  vector(std::array<T, N>)->vector<T, N>;

  template <detail::coordinate T>
  using vec2 = vector<T, 2>;

  template <detail::coordinate T>
  using vec3 = vector<T, 3>;

  using vecf2 = vec2<float>;
  using vecf3 = vec3<float>;
  using vecd2 = vec2<double>;
  using vecd3 = vec3<double>;

  using point2d = vec2<int>;
  using point3d = vec3<int>;
}