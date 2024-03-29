#pragma once

namespace utils
{
  //
  // Matrix of arbitrary dimensions
  //
  template <coordinate T, std::size_t W, std::size_t H>
  class matrix
  {
  private:
    template <coordinate C, std::size_t N>
    using rc_helper = vector<C, N>;

  public:
    static constexpr auto width  = W;
    static constexpr auto height = H;

    using value_type   = T;
    using row_type     = rc_helper<value_type, width>;
    using col_type     = rc_helper<value_type, height>;
    using storage_type = std::array<row_type, height>;
    using size_type    = storage_type::size_type;

  private:
    using idx_w = idx_gen<width>;
    using idx_h = idx_gen<height>;

  private:
    template <typename Int, Int... Seq>
    static consteval auto identity_impl(idx_seq<Int, Seq...>) noexcept
    {
      return matrix{ row_type::template axis_norm<Seq>()... };
    }

  public:
    static consteval auto is_square() noexcept
    {
      return width == height;
    }

    static consteval auto identity() noexcept
      requires (width == height)
    {
      return identity_impl(idx_w{});
    }

    // 2D rotation matrix
    static constexpr auto rotation(coordinate auto angle) noexcept
      requires (width == 2 && height == 2)
    {
      const auto sinA = sin(angle);
      const auto cosA = cos(angle);
      return matrix{
        row_type{ cosA, -sinA },
        row_type{ sinA,  cosA } };
    }

  public:
    CLASS_SPECIALS_ALL(matrix);

    template <math_vec... R> requires homogenous_pack<row_type, R...>
    constexpr matrix(row_type first, R... rows) noexcept :
      m_data{ first, rows... }
    { }

    template <math_vec First, math_vec... Rest> requires (sizeof...(Rest) <= height - 1)
    constexpr matrix(First first, Rest ...values) noexcept :
      m_data{ static_cast<row_type>(first), static_cast<row_type>(values)... }
    { }

    template <coordinate U, size_type NC, size_type NR>
    constexpr bool operator==(const matrix<U, NC, NR>& other) const noexcept
    {
      if constexpr (NC != width || NR != height)
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
      return m_data[idx];
    }
    constexpr auto& operator[](size_type idx) noexcept
    {
      return FROM_CONST(operator[], idx);
    }

    constexpr auto operator-() const noexcept
    {
      return get_scaled(value_type{ -1 });
    }

    template <coordinate U, size_type C, size_type R>
    constexpr auto operator+(const matrix<U, C, R>& other) const noexcept
      requires (C == width && R == height)
    {
      using ct = std::common_type_t<U, value_type>;
      matrix<ct, width, height> dest;
      auto thisIt  = begin();
      auto otherIt = other.begin();
      std::transform(dest.begin(), dest.end(), dest.begin(),
                     [&thisIt, &otherIt](const auto&)
                     {
                       return *(thisIt++) + *(otherIt++);
                     });
      return dest;
    }

    template <coordinate U, size_type C, size_type R>
      requires (std::same_as<U, value_type> && C == width && R == height)
    constexpr auto& operator+=(const matrix<U, C, R>& other) noexcept
    {
      *this = *this + other;
      return *this;
    }

    template <coordinate U, size_type C, size_type R>
    constexpr auto operator-(const matrix<U, C, R>& other) const noexcept
      requires (C == width && R == height)
    {
      using ct = std::common_type_t<U, value_type>;
      matrix<ct, width, height> dest;
      auto thisIt  = begin();
      auto otherIt = other.begin();
      std::transform(dest.begin(), dest.end(), dest.begin(),
                     [&thisIt, &otherIt](const auto&)
                     {
                       return *(thisIt++) - *(otherIt++);
                     });
      return dest;
    }

    template <coordinate U, size_type C, size_type R>
      requires (std::same_as<U, value_type> && C == width && R == height)
    constexpr auto& operator-=(const matrix<U, C, R>& other) noexcept
    {
      *this = *this - other;
      return *this;
    }

    template <coordinate U, size_type D>
    constexpr auto operator*(const vector<U, D>& vec) const noexcept
      requires (D == width)
    {
      using ct = std::common_type_t<U, value_type>;
      vector<ct, height> result;
      std::transform(begin(), end(), result.begin(),
                     [&vec](const auto& val)
                     {
                       return vec * val;
                     });
      return result;
    }

    constexpr auto& operator*=(coordinate auto scalar) noexcept
    {
      return scale(scalar);
    }
    constexpr auto& operator/=(coordinate auto scalar) noexcept
    {
      return scale_inv(scalar);
    }

    template <coordinate U, size_type C, size_type R> requires (C == height && R == width)
    constexpr auto operator*(const matrix<U, C, R>& other) const noexcept
    {
      using ct = std::common_type_t<value_type, U>;
      using rt = matrix<ct, height, other.width>;
      rt dest;

      for (size_type rIdx{}; rIdx < height; ++rIdx)
      {
        for (size_type cIdx{}; cIdx < other.width; ++cIdx)
        {
          dest[rIdx][cIdx] = (*this)[rIdx] * other.column(cIdx);
        }
      }

      return dest;
    }

    template <coordinate U, size_type C, size_type R>
    constexpr auto to() const noexcept
    {
      if constexpr (C == width && R == height && std::same_as<value_type, U>)
      {
        return *this;
      }
      else
      {
        matrix<U, C, R> dest;
        auto destIt = dest.begin();
        ranges::for_each_n(begin(), std::min(height, R), [&destIt](const auto& val)
                           {
                             *(destIt++) = val.template to<U, C>();
                           });
        return dest;
      }
    }

    template <coordinate U, size_type C, size_type R>
    constexpr operator matrix<U, C, R>() const noexcept
    {
      return to<U, C, R>();
    }

  public:
    constexpr auto get_scaled(coordinate auto scalar) const noexcept
    {
      matrix result;
      std::transform(begin(), end(), result.begin(),
                     [scalar](const auto& cur)
                     {
                       return cur * scalar;
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
      matrix result;
      std::transform(begin(), end(), result.begin(),
                     [scalar](const auto& cur)
                     {
                       return cur / scalar;
                     });
      return result;
    }
    constexpr auto& scale_inv(coordinate auto scalar) noexcept
    {
      *this = get_scaled_inv(scalar);
      return *this;
    }

    template <size_type R> requires (R < height)
    constexpr const auto& get() const noexcept
    {
      return m_data[R];
    }
    template <size_type R> requires (R < height)
    constexpr auto& get() noexcept
    {
      return mutate(std::as_const(*this).template get<R>());
    }

    template <size_type R, size_type C> requires (R < height && C < width)
    constexpr const auto& get() const noexcept
    {
      return get<R>().template get<C>();
    }
    template <size_type R, size_type C> requires (R < height && C < width)
    constexpr auto& get() noexcept
    {
      return mutate(std::as_const(*this).template get<R, C>());
    }

  private:
    template <typename Int, Int... Seq>
    constexpr auto column_impl(size_type c, idx_seq<Int, Seq...>) const noexcept
    {
      return col_type{ ((*this)[Seq][c])... };
    }

  public:
    //
    // Doesn't check the bounds, be careful
    //
    constexpr auto column(size_type c) const noexcept
    {
      return column_impl(c, idx_h{});
    }

    template <size_type C> requires (C < width)
    constexpr auto column() const noexcept
    {
      return column(C);
    }

    constexpr auto begin() const noexcept
    {
      return m_data.begin();
    }
    constexpr auto end() const noexcept
    {
      return m_data.end();
    }
    constexpr auto begin() noexcept
    {
      return m_data.begin();
    }
    constexpr auto end() noexcept
    {
      return m_data.end();
    }

  private:
    template <typename Int, Int... Seq>
    constexpr auto transpose_impl(idx_seq<Int, Seq...>) const noexcept
    {
      return matrix<value_type, height, width>{
        column<Seq>()...
      };
    }

  public:
    constexpr auto transpose() const noexcept
    {
      return transpose_impl(idx_w{});
    }

  private:
    template <typename U>
    constexpr bool eq(const matrix<U, width, height>& other) const noexcept
    {
      using ct = std::common_type_t<U, value_type>;
      using target_t = rc_helper<ct, width>;
      auto otherIt = other.begin();
      for (auto it = begin(); it != end(); ++it, ++otherIt)
      {
        if (!utils::eq(*it, *otherIt))
          return false;
      }
      return true;
    }

  private:
    storage_type m_data{};
  };

  constexpr bool eq(const math_matr auto& m1, const math_matr auto& m2) noexcept
  {
    return m1 == m2;
  }

  template <coordinate T, std::size_t C, std::size_t R, coordinate S>
  constexpr auto operator*(const matrix<T, C, R>& mat, const S& scalar) noexcept
  {
    return mat.get_scaled(scalar);
  }
  template <coordinate T, std::size_t C, std::size_t R, coordinate S>
  constexpr auto operator*(const S& scalar, const matrix<T, C, R>& mat) noexcept
  {
    return mat * scalar;
  }
  template <coordinate T, std::size_t C, std::size_t R, coordinate S>
  constexpr auto operator/(const matrix<T, C, R>& mat, const S& scalar) noexcept
  {
    return mat.get_scaled_inv(scalar);
  }

  template <typename ValT, std::size_t W, typename... Rest>
  matrix(vector<ValT, W>, Rest...)->matrix<ValT, W, sizeof...(Rest) + 1>;

  template <coordinate T>
  using matr2 = matrix<T, 2, 2>;

  template <coordinate T>
  using matr3 = matrix<T, 3, 3>;

  using matrf2 = matr2<float>;
  using matrf3 = matr3<float>;
  using matrd2 = matr2<double>;
  using matrd3 = matr3<double>;
  using matri2 = matr2<int>;
  using matri3 = matr3<int>;
}

template <typename T, std::size_t W, std::size_t H>
struct std::tuple_size<utils::matrix<T, W, H>>
{
  static constexpr auto value = H;
};

template <std::size_t I, typename T, std::size_t W, std::size_t H>
struct std::tuple_element<I, utils::matrix<T, W, H>>
{
  using type = typename utils::matrix<T, W, H>::row_type;
};