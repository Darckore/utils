#pragma once

namespace utils
{
  //
  // Vector of arbitrary dimensions
  // This is the math vector representing
  // a point in space relative to the origin
  //
  template <coordinate T, std::size_t D>
  class vector
  {
  public:
    static constexpr auto dimensions = D;

    using value_type = T;
    using storage_type = std::array<value_type, dimensions>;
    using size_type = storage_type::size_type;

    static constexpr auto zero_coord = value_type{ 0 };
    static constexpr auto unit_coord = value_type{ 1 };

    template <size_type Axis> requires (Axis < dimensions)
    static consteval auto axis_norm() noexcept
    {
      vector <value_type, dimensions> res;
      res.template get<Axis>() = unit_coord;
      return res;
    }

  public:
    CLASS_SPECIALS_ALL(vector);

    template <coordinate... V> requires homogenous_pack<value_type, V...>
    constexpr vector(value_type first, V... values) noexcept :
      m_coords{ first, values... }
    { }

    template <coordinate First, coordinate... Rest> requires (sizeof...(Rest) <= dimensions - 1)
    constexpr vector(First first, Rest ...values) noexcept :
      m_coords{ static_cast<value_type>(first), static_cast<value_type>(values)... }
    { }

    template <coordinate U, size_type N>
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

    //
    // Doesn't check the bounds, be careful
    //
    constexpr auto& operator[](size_type idx) const noexcept
    {
      return m_coords[idx];
    }
    constexpr auto& operator[](size_type idx) noexcept
    {
      return utils::mutate(std::as_const(*this).operator[](idx));
    }

    constexpr auto operator-() const noexcept
    {
      return get_scaled(value_type{ -1 });
    }

    template <coordinate U, size_type N>
    constexpr auto operator+(const vector<U, N>& other) const noexcept
      requires (N == dimensions)
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

    template <coordinate U, size_type N>
      requires (std::is_same_v<U, value_type> && N == dimensions)
    constexpr auto& operator+=(const vector<U, N>& other) noexcept
    {
      *this = *this + other;
      return *this;
    }

    template <coordinate U, size_type N>
    constexpr auto operator-(const vector<U, N>& other) const noexcept
      requires (N == dimensions)
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

    template <coordinate U, size_type N>
      requires (std::is_same_v<U, value_type> && N == dimensions)
    constexpr auto& operator-=(const vector<U, N>& other) noexcept
    {
      *this = *this - other;
      return *this;
    }

    template <coordinate U, size_type N>
    constexpr auto operator*(const vector<U, N>& other) const noexcept
      requires (N == dimensions)
    {
      using ct = std::common_type_t<U, value_type>;
      auto it = other.begin();
      return std::accumulate(begin(), end(), ct{},
                             [&it](auto acc, auto cur)
                             {
                               return acc + ct{ cur * *(it++) };
                             });
    }

    constexpr auto& operator*=(coordinate auto scalar) noexcept
    {
      return scale(scalar);
    }
    constexpr auto& operator/=(coordinate auto scalar) noexcept
    {
      return scale_inv(scalar);
    }

    template <coordinate U, size_type N>
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

    template <coordinate U, size_type N>
    constexpr operator vector<U, N>() const noexcept
    {
      return to<U, N>();
    }

  public:
    constexpr auto is_normalised() const noexcept
    {
      return utils::eq(len_sq(), unit_coord);
    }

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

    constexpr auto get_scaled(coordinate auto scalar) const noexcept
    {
      vector result;
      std::transform(begin(), end(), result.begin(),
                     [scalar](auto cur)
                     {
                       return static_cast<value_type>(scalar) * cur;
                     });
      return result;
    }
    constexpr auto& scale(coordinate auto scalar) noexcept
    {
      *this = get_scaled(scalar);
      return *this;
    }
    constexpr auto get_scaled_inv(coordinate auto scalar) const noexcept
    {
      vector result;
      std::transform(begin(), end(), result.begin(),
                     [scalar](auto cur)
                     {
                       return cur / static_cast<value_type>(scalar);
                     });
      return result;
    }
    constexpr auto& scale_inv(coordinate auto scalar) noexcept
    {
      *this = get_scaled_inv(scalar);
      return *this;
    }

    // 2D rotation
    constexpr auto get_rotated(real auto angle) const noexcept
      requires (dimensions == 2)
    {
      vector<value_type, dimensions> res;
      const auto cosA = cos(angle);
      const auto sinA = sin(angle);
      auto&& x = get<0>();
      auto&& y = get<1>();
      res.template get<0>() = x * cosA - y * sinA;
      res.template get<1>() = x * sinA + y * cosA;
      return res;
    }
    constexpr auto& rotate(real auto angle) noexcept
      requires (dimensions == 2)
    {
      *this = get_rotated(angle);
      return *this;
    }

    // 3D rotation around an axis (don't have to normalise that)
    template <coordinate U, size_type N>
    constexpr auto get_rotated(real auto angle, const vector<U, N>& axis) const noexcept
      requires (N == dimensions && N == 3)
    {
      auto aNorm = axis.get_normalised();
      const auto cosA = cos(angle);
      const auto sinA = sin(angle);
      const auto& self = *this;
      return (self * cosA)
           + (aNorm.cross(self) * sinA)
           + (aNorm * (aNorm * self) * (unit_coord - cosA));
    }
    template <coordinate U, size_type N>
    constexpr auto& rotate(real auto angle, const vector<U, N>& axis) noexcept
      requires (N == dimensions && N == 3)
    {
      *this = get_rotated(angle, axis);
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
    template <coordinate U, size_type N>
    constexpr auto cross(const vector<U, N>& other) const noexcept
      requires (N == dimensions && eq_any(dimensions, 2, 3))
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
      return mutate(std::as_const(*this).template get<N>());
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

  constexpr bool eq(const math_vec auto& v1, const math_vec auto& v2) noexcept
  {
    return v1 == v2;
  }

  template <coordinate T, std::size_t N, coordinate S>
  constexpr auto operator*(const vector<T, N>& vec, const S& scalar) noexcept
  {
    return vec.get_scaled(scalar);
  }
  template <coordinate T, std::size_t N, coordinate S>
  constexpr auto operator*(const S& scalar, const vector<T, N>& vec) noexcept
  {
    return vec * scalar;
  }
  template <coordinate T, std::size_t N, coordinate S>
  constexpr auto operator/(const vector<T, N>& vec, const S& scalar) noexcept
  {
    return vec.get_scaled_inv(scalar);
  }

  template <coordinate First, coordinate... Rest>
  vector(First, Rest...)->vector<First, sizeof...(Rest) + 1>;

  template <coordinate T>
  using vec2 = vector<T, 2>;

  template <coordinate T>
  using vec3 = vector<T, 3>;

  using vecf2 = vec2<float>;
  using vecf3 = vec3<float>;
  using vecd2 = vec2<double>;
  using vecd3 = vec3<double>;

  using point2d = vec2<int>;
  using point3d = vec3<int>;
}

template <typename T, std::size_t N>
struct std::tuple_size<utils::vector<T, N>>
{
  static constexpr auto value = N;
};

template <std::size_t I, typename T, std::size_t N>
struct std::tuple_element<I, utils::vector<T, N>>
{
  using type = typename utils::vector<T, N>::value_type;
};