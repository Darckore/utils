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
    return std::is_same_v<T, typename vt::type> &&
           N == vt::dimensions;
  }

  template <typename T>
  struct matr_test
  {
    using underlying_type = std::remove_cvref_t<T>;
    using type = underlying_type::value_type;
    static constexpr auto width  = underlying_type::width;
    static constexpr auto height = underlying_type::height;
  };

  template <typename T, std::size_t W, std::size_t H>
  bool test_type(auto&& matr)
  {
    using mt = matr_test<decltype(matr)>;
    return std::is_same_v<T, typename mt::type> &&
           W == mt::width &&
           H == mt::height;
  }
}