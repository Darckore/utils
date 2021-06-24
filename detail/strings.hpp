#pragma once

namespace utils
{
  namespace detail
  {
    //
    // Converts a range into a string_view
    //
    template <ranges::range R>
    constexpr auto to_string_view(R range) noexcept
      requires std::is_convertible_v<decltype(*range.begin()), std::string_view::value_type>
    {
      using size_type = std::string_view::size_type;
      return std::string_view{ &*range.begin(), static_cast<size_type>(ranges::distance(range)) };
    }
  }

  //
  // Splits a string_view into an iterable range
  //
  constexpr auto split(std::string_view str, std::string_view pattern) noexcept
  {
    return str | views::split(pattern) | views::transform([](auto&& part)
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
  // Takes another string_view which represents the postfix to remove
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
  // Takes another string_view which represents the prefix and postfix to remove
  //
  constexpr auto trim(std::string_view str, std::string_view pattern) noexcept
  {
    return rtrim(ltrim(str, pattern), pattern);
  }

  //
  // Trims a string_view from the left
  // Takes a number of characters (or value convertible to them)
  // which will be removed if encountered in no specific order
  //
  template <typename C1, typename ...CN>
  constexpr auto ltrim(std::string_view str, C1&& c1, CN&& ...cn) noexcept
    requires detail::all_convertible<std::string_view::value_type, C1, CN...>
  {
    auto offset = 0ull;
    for (auto c : str)
    {
      if (eq_none(c, std::forward<C1>(c1), std::forward<CN>(cn)...))
        break;

      ++offset;
    }
    str.remove_prefix(offset);
    return str;
  }

  //
  // Trims a string_view from the right
  // Takes a number of characters (or value convertible to them)
  // which will be removed if encountered in no specific order
  //
  template <typename C1, typename ...CN>
  constexpr auto rtrim(std::string_view str, C1&& c1, CN&& ...cn) noexcept
    requires detail::all_convertible<std::string_view::value_type, C1, CN...>
  {
    auto offset = 0ull;
    for (auto c : str | views::reverse)
    {
      if (eq_none(c, std::forward<C1>(c1), std::forward<CN>(cn)...))
        break;

      ++offset;
    }
    str.remove_suffix(offset);
    return str;
  }

  //
  // Trims a string_view from both sides
  // Takes a number of characters (or value convertible to them)
  // which will be removed if encountered in no specific order
  //
  template <typename C1, typename ...CN>
  constexpr auto trim(std::string_view str, C1&& c1, CN&& ...cn) noexcept
    requires detail::all_convertible<std::string_view::value_type, C1, CN...>
  {
    str = ltrim(str, std::forward<C1>(c1), std::forward<CN>(cn)...);
    return rtrim(str, std::forward<C1>(c1), std::forward<CN>(cn)...);
  }
}