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

  //
  // Trims a string_view from the left
  // Takes another string_view which represents the prefix to remove
  //
  constexpr auto ltrim(std::string_view str, std::string_view pattern) noexcept
  {
    while (str.starts_with(pattern))
    {
      str.remove_prefix(pattern.length());
    }
    return str;
  }

  //
  // Trims a string_view from the right
  // Takes another string_view which represents the prefix to remove
  //
  constexpr auto rtrim(std::string_view str, std::string_view pattern) noexcept
  {
    while (str.ends_with(pattern))
    {
      str.remove_suffix(pattern.length());
    }
    return str;
  }

  //
  // Trims a string_view from both sides
  // Takes another string_view which represents the prefix to remove
  //
  constexpr auto trim(std::string_view str, std::string_view pattern) noexcept
  {
    return rtrim(ltrim(str, pattern), pattern);
  }
}