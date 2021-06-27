#pragma once

namespace utils
{
  namespace detail
  {
    //
    // Basic numerics and the like
    //
    using max_real_t = long double;
    using max_int_t = std::uintmax_t;

    template <typename T>
    concept integer = std::is_integral_v<T> && !std::is_same_v<T, bool>;
    template <typename T>
    concept real = std::is_floating_point_v<T>;

    //
    // For Quake's fast inverse square root
    //
    template <real T>
    struct sqrt_magic;

    template <>
    struct sqrt_magic<float>
    {
      static constexpr auto value = 0x5f3759df;
      using type = std::uint32_t;
    };
    template <>
    struct sqrt_magic<double>
    {
      static constexpr auto value = 0x5fe6eb50c7b537a9;
      using type = std::uint64_t;
    };

    template <real T>
    inline constexpr auto sqrt_magic_v = sqrt_magic<T>::value;

    template <real T>
    using sqrt_magic_t = typename sqrt_magic<T>::type;
  } 
   
  //
  // Quake's fast inverse square root
  //
  template <detail::real T>
  constexpr auto inv_sqrt(T number) noexcept
  {
    using detail::sqrt_magic_t;
    using detail::sqrt_magic_v;
    using std::bit_cast;
    using result_type = T;
    using intrm_type = sqrt_magic_t<result_type>;

    constexpr auto magic = sqrt_magic_v<result_type>;
    constexpr auto threehalfs = result_type{ 1.5 };
    constexpr auto half = result_type{ 0.5 };

    const auto intermediate = bit_cast<intrm_type>(number);
    const auto res = bit_cast<result_type>(magic - (intermediate >> 1));
    return res * (threehalfs - (number * half * res * res));
  }

  //
  // A constexpr abs version
  //
  template <typename T>
  constexpr auto abs(T val) noexcept
    requires std::is_arithmetic_v<T>
  {
    if constexpr (std::is_unsigned_v<T>)
      return val;
    else
      return val < 0 ? -val : val;
  }

  //
  // Checks the sign
  //
  template <typename T>
  constexpr auto sign(T val) noexcept
    requires std::is_arithmetic_v<T>
  {
    if constexpr (std::is_unsigned_v<T>)
      return val != 0 ? T{ 1 } : T{ 0 };
    else
      return val < 0
            ? T{ -1 }
            : (val > 0 ? T{ 1 } : T{ 0 });
  }

  //
  // This is mostly for floats
  // To avoid dealing with the epsilon bs on comparing them
  //
  template <detail::comparable T>
  constexpr bool eq(T left, T right) noexcept
    requires std::is_arithmetic_v<T>
  {
    constexpr auto max_diff = std::numeric_limits<T>::epsilon();
    return abs(left - right) <= max_diff;
  }

  //
  // Converts degrees to radians
  //
  template <detail::real Angle>
  constexpr auto deg_to_rad(Angle angle) noexcept
  {
    return angle * std::numbers::pi_v<Angle> / 180;
  }

  //
  // Converts radians to degrees
  //
  template <detail::real Angle>
  constexpr auto rad_to_deg(Angle angle) noexcept
  {
    return angle * 180 / std::numbers::pi_v<Angle>;
  }

}

#include "ratio.hpp"