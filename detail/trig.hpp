#pragma once

namespace utils
{
  namespace detail
  {

  }

  template <detail::real R>
  inline constexpr auto pi = std::numbers::pi_v<R>;

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


}