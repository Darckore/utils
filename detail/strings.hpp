#pragma once

namespace utils
{
  namespace detail
  {
    //
    // Converts a range into a string_view
    //
    template <rng::range R>
    constexpr auto to_string_view(R range) noexcept
      requires std::is_convertible_v<decltype(*range.begin()), std::string_view::value_type>
    {
      using size_type = std::string_view::size_type;
      return std::string_view{ &*range.begin(), static_cast<size_type>(rng::distance(range)) };
    }
  }

  //
  // Splits a string_view into an iterable range
  //
  constexpr auto split(std::string_view str, std::string_view pattern) noexcept
  {
    return str | rvi::split(pattern) | rvi::transform([](auto&& part)
      {
        return detail::to_string_view(part);
      });
  }
}