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

    constexpr explicit vector(const storage_type& arr) :
      m_coords{ arr }
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
      if constexpr (std::is_same_v<value_type, U>)
      {
        storage_type dest{};
        auto thisIt  = m_coords.begin();
        auto otherIt = other.m_coords.begin();
        std::transform(dest.begin(), dest.end(), dest.begin(),
                       [&thisIt, &otherIt](auto)
                       {
                         return *(thisIt++) + *(otherIt++);
                       });
        return vector<value_type, dimensions>{ dest };
      }
      else
      {
        using ct = std::common_type_t<U, value_type>;
        return to<ct, dimensions>() + other.template to<ct, dimensions>();
      }
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
      if constexpr (std::is_same_v<value_type, U>)
      {
        storage_type dest{};
        auto thisIt  = m_coords.begin();
        auto otherIt = other.m_coords.begin();
        std::transform(dest.begin(), dest.end(), dest.begin(),
                       [&thisIt, &otherIt](auto)
                       {
                         return *(thisIt++) - *(otherIt++);
                       });
        return vector<value_type, dimensions>{ dest };
      }
      else
      {
        using ct = std::common_type_t<U, value_type>;
        return to<ct, dimensions>() - other.template to<ct, dimensions>();
      }
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
                               return acc + static_cast<ct>(cur * *(it++));
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
        std::array<U, N> dest{};
        auto destIt = dest.begin();
        ranges::for_each_n(m_coords.begin(), std::min(dimensions, N), [&destIt](auto val)
                           {
                             *(destIt++) = static_cast<U>(val);
                           });

        return vector<U, N>{ dest };
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
      return std::accumulate(m_coords.begin(), m_coords.end(), zero_coord,
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
      std::transform(m_coords.begin(), m_coords.end(), result.m_coords.begin(),
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

    constexpr auto get_normalised() const noexcept
    {
      return *this / len();
    }
    constexpr auto& normalise() noexcept
    {
      *this = get_normalised();
      return *this;
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

  private:
    template <typename U>
    constexpr bool eq(const vector<U, dimensions>& other) const noexcept
    {
      if constexpr (std::is_same_v<U, value_type>)
      {
        for (auto i1 = m_coords.begin(), i2 = other.m_coords.begin(); i1 != m_coords.end(); ++i1, ++i2)
        {
          if (!utils::eq(*i1, *i2))
            return false;
        }
        return true;
      }
      else
      {
        using ct = std::common_type_t<U, value_type>;
        return to<ct, dimensions>() == other.template to<ct, dimensions>();
      }
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