#pragma once

namespace utils::testing
{
  template <typename T>
  struct vec_test
  {
    using underlying_type = std::remove_cvref_t<T>;
    using type = underlying_type::value_type;
    static constexpr auto dimensions = underlying_type::dimensions;
  };

  template <typename T, std::size_t N>
  bool test_type(auto&& vec)
  {
    using vt = vec_test<decltype(vec)>;
    return std::is_same_v<T, typename vt::type> && N == vt::dimensions;
  }
}