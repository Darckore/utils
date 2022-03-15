#pragma once

namespace utils
{
  //
  // Matrix of arbitrary dimensions
  //
  template <detail::coordinate T, std::size_t W, std::size_t H>
  class matrix
  {
  private:
    template <detail::coordinate C, std::size_t N>
    using rt_helper = vector<C, N>;

  public:
    static constexpr auto width  = W;
    static constexpr auto height = H;

    using value_type = T;
    using row_type = rt_helper<T, width>;
    using storage_type = std::array<row_type, height>;
    using size_type = storage_type::size_type;

  public:
    CLASS_SPECIALS_ALL(matrix);

    template <typename... R> requires detail::homogenous_pack<row_type, R...>
    constexpr matrix(row_type first, R... rows) noexcept :
      m_data{ first, rows... }
    { }

    template <typename First, typename... Rest> requires (sizeof...(Rest) <= height - 1)
    constexpr matrix(First first, Rest ...values) noexcept :
      m_data{ static_cast<row_type>(first), static_cast<row_type>(values)... }
    { }

    template <detail::coordinate U, size_type NC, size_type NR>
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
      return utils::mutate(std::as_const(*this).operator[](idx));
    }

    constexpr auto operator-() const noexcept
    {
      return get_scaled(value_type{ -1 });
    }

    template <detail::coordinate U>
    constexpr auto operator*(const vector<U, width>& vec) const noexcept
    {
      using ct = std::common_type_t<U, value_type>;
      vector<ct, width> result;
      std::transform(begin(), end(), result.begin(),
                     [&vec](const auto& val)
                     {
                       return vec * val;
                     });
      return result;
    }

  public:
    constexpr auto get_scaled(detail::coordinate auto scalar) const noexcept
    {
      matrix result;
      std::transform(begin(), end(), result.begin(),
                     [scalar](const auto& cur)
                     {
                       return cur * scalar;
                     });
      return result;
    }
    constexpr auto& scale(detail::coordinate auto scalar) noexcept
    {
      *this = get_scaled(scalar);
      return *this;
    }

    template <size_type R, size_type C> requires (R < height && C < width)
    constexpr const auto& get() const noexcept
    {
      return m_data[R][C];
    }
    template <size_type R, size_type C> requires (R < height && C < width)
    constexpr auto& get() noexcept
    {
      return mutate(std::as_const(*this).get<R, C>());
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
    template <typename U>
    constexpr bool eq(const matrix<U, width, height>& other) const noexcept
    {
      using ct = std::common_type_t<U, value_type>;
      using target_t = rt_helper<ct, width>;
      auto otherIt = other.begin();
      for (auto it = begin(); it != end(); ++it, ++otherIt)
      {
        if (!utils::eq(static_cast<target_t>(*it), static_cast<target_t>(*otherIt)))
          return false;
      }
      return true;
    }

  private:
    storage_type m_data;
  };

  template <typename ValT, std::size_t W, typename... Rest>
  matrix(vector<ValT, W>, Rest...)->matrix<ValT, W, sizeof...(Rest) + 1>;
}