#pragma once

namespace utils
{
  namespace detail
  {
    //
    // Converts a range into a string_view
    //
    template <ranges::range R>
    constexpr auto to_string_view(R&& range) noexcept
      requires std::convertible_to<decltype(*range.begin()), std::string_view::value_type>
    {
      using size_type = std::string_view::size_type;
      return std::string_view{ &*range.begin(), static_cast<size_type>(ranges::distance(range)) };
    }
  }

  //
  // Splits a string_view into an iterable range
  //
  inline constexpr auto split(std::string_view str, std::string_view pattern) noexcept
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
  inline constexpr auto ltrim(std::string_view str, std::string_view pattern) noexcept
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
  inline constexpr auto rtrim(std::string_view str, std::string_view pattern) noexcept
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
  inline constexpr auto trim(std::string_view str, std::string_view pattern) noexcept
  {
    return rtrim(ltrim(str, pattern), pattern);
  }

  //
  // Trims a string_view from the left
  // Takes a number of characters (or values convertible to them)
  // which will be removed if encountered in no specific order
  //
  template <typename C1, typename ...CN>
    requires (all_convertible<std::string_view::value_type, C1, CN...>)
  constexpr auto ltrim(std::string_view str, C1&& c1, CN&& ...cn) noexcept
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
  // Takes a number of characters (or values convertible to them)
  // which will be removed if encountered in no specific order
  //
  template <typename C1, typename ...CN>
    requires (all_convertible<std::string_view::value_type, C1, CN...>)
  constexpr auto rtrim(std::string_view str, C1&& c1, CN&& ...cn) noexcept
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
  // Takes a number of characters (or values convertible to them)
  // which will be removed if encountered in no specific order
  //
  template <typename C1, typename ...CN>
    requires (all_convertible<std::string_view::value_type, C1, CN...>)
  constexpr auto trim(std::string_view str, C1&& c1, CN&& ...cn) noexcept
  {
    str = ltrim(str, std::forward<C1>(c1), std::forward<CN>(cn)...);
    return rtrim(str, std::forward<C1>(c1), std::forward<CN>(cn)...);
  }

  //
  // Trims a string_view from the left
  // Removes whitespace chars:
  // ' ', '\f', '\n', '\r', '\t', '\v'
  //
  inline constexpr auto ltrim(std::string_view str) noexcept
  {
    return ltrim(str, ' ', '\f', '\n', '\r', '\t', '\v');
  }

  //
  // Trims a string_view from the right
  // Removes whitespace chars:
  // ' ', '\f', '\n', '\r', '\t', '\v'
  //
  inline constexpr auto rtrim(std::string_view str) noexcept
  {
    return rtrim(str, ' ', '\f', '\n', '\r', '\t', '\v');
  }

  //
  // Trims a string_view from both sides
  // Removes whitespace chars:
  // ' ', '\f', '\n', '\r', '\t', '\v'
  //
  inline constexpr auto trim(std::string_view str) noexcept
  {
    return trim(str, ' ', '\f', '\n', '\r', '\t', '\v');
  }

  //
  // Reads contents of a file into a string
  //
  inline auto read_file(const fsys::path& fname) noexcept -> std::expected<std::string, std::error_code>
  {
    std::error_code errc;
    auto at = fsys::canonical(fname, errc);
    if (errc)
    {
      return std::unexpected{ errc };
    }

    std::string buf;
    std::ifstream in{ at.string() };
    if (!in)
    {
      return std::unexpected{ std::make_error_code(std::io_errc::stream) };
    }

    in.seekg(0, std::ios::end);
    buf.reserve(in.tellg());
    in.seekg(0, std::ios::beg);

    using it = std::istreambuf_iterator<std::string::value_type>;
    buf.assign(it{ in }, it{});
    return buf;
  }

  //
  // A hashed string representation. Contains a string's hash code,
  // can be used as an 'id' of sorts
  //
  class hashed_string
  {
  public:
    using hash_type   = max_int_t;
    using string_type = std::string_view;
    using size_type   = string_type::size_type;

  public:
    CLASS_SPECIALS_ALL_CUSTOM(hashed_string);

    constexpr hashed_string() noexcept = default;

    constexpr hashed_string(string_type str) noexcept :
      m_hash{ hash(str) }
    {}

    constexpr hashed_string(const char* str) noexcept :
      hashed_string{ string_type{ str } }
    {}

    constexpr hashed_string(const char* str, size_type len) noexcept :
      hashed_string{ string_type{ str, len } }
    {}

    constexpr hashed_string(const std::string& str) noexcept :
      hashed_string{ string_type{ str } }
    {}

    constexpr auto operator*() const noexcept
    {
      return m_hash;
    }

    constexpr bool operator==(const hashed_string& other) const noexcept = default;

  private:
    hash_type m_hash{};
  };
}


template <>
struct std::hash<utils::hashed_string>
{
  constexpr auto operator()(const utils::hashed_string& s) const noexcept
  {
    return *s;
  }
};
