#pragma once

namespace utils
{
  namespace detail
  {
    //
    // Taylor expansion for sine
    //
    template<real T>
    struct taylor_gen
    {
    private:
      static constexpr auto size = detail::max_factorial_v<sizeof(detail::max_int_t)> / 2;
      using array_type = std::array<T, size>;
      using size_type = array_type::size_type;
      using iter = array_type::const_iterator;

      static constexpr auto coeff(size_type n) noexcept
      {
        const auto multiplier = n % 2 ? 1 : -1;
        const auto fact = static_cast<T>(factorial((2 * n) - 1));
        return multiplier * inv(fact);
      }

      template <typename T, T... Seq>
      static constexpr auto calc_coeffs(std::integer_sequence<T, Seq...>)
      {
        return array_type{ coeff(Seq + 1)... };
      }

      static constexpr auto data = calc_coeffs(std::make_index_sequence<size>{});

    public:
      static constexpr iter begin()
      {
        return data.begin();
      }
      static constexpr iter end()
      {
        return data.end();
      }
    };

  }

  template <detail::real R>
  inline constexpr auto pi = std::numbers::pi_v<R>;

  template <detail::real R>
  inline constexpr auto piHalf = pi<R> / 2;

  template <detail::real R>
  inline constexpr auto piQuarter = pi<R> / 4;

  template <detail::real R>
  inline constexpr auto piDouble = pi<R> * 2;

  //
  // Converts degrees to radians
  //
  template <detail::real Angle>
  constexpr auto deg_to_rad(Angle angle) noexcept
  {
    return angle * pi<Angle> / 180;
  }

  //
  // Converts radians to degrees
  //
  template <detail::real Angle>
  constexpr auto rad_to_deg(Angle angle) noexcept
  {
    return angle * 180 / pi<Angle>;
  }

  //
  // Calculates sine
  //
  template <detail::real Angle>
  constexpr auto sin(Angle angle) noexcept
  {
    auto pow = angle;
    auto result = Angle{};
  
    const auto xSq = pow * pow;
    for (auto cur : detail::taylor_gen<Angle>{})
    {
      result += cur * pow;
      pow *= xSq;
    }
  
    return result;
  }

  //
  // Calculates cosine
  //
  template <detail::real Angle>
  constexpr auto cos(Angle angle) noexcept
  {
    return sin(piHalf<Angle> - angle);
  }

}