#include "utils/utils.hpp"
#include "tests/helpers.hpp"
using namespace utils;

TEST(matr, t_deduction)
{
  constexpr matrix m2i{ 
    vector{ 1,   2.0 },
    vector{ 3ll, 4.0f } };
  EXPECT_TRUE((utils::testing::test_type<int, 2, 2>(m2i)));

  constexpr matrix m34d{
    vector{ 1.0,  2,  3},
    vector{  4,   5,  6},
    vector{  7,   8,  9},
    vector{ 10,  11, 12}
  };
  EXPECT_TRUE((utils::testing::test_type<double, 3, 4>(m34d)));
}

TEST(matr, t_aliases)
{
  constexpr matrf2 m2f;
  EXPECT_TRUE((utils::testing::test_type<float, 2, 2>(m2f)));

  constexpr matrf3 m3f;
  EXPECT_TRUE((utils::testing::test_type<float, 3, 3>(m3f)));

  constexpr matrd2 m2d;
  EXPECT_TRUE((utils::testing::test_type<double, 2, 2>(m2d)));

  constexpr matrd3 m3d;
  EXPECT_TRUE((utils::testing::test_type<double, 3, 3>(m3d)));

  constexpr matri2 m2i;
  EXPECT_TRUE((utils::testing::test_type<int, 2, 2>(m2i)));

  constexpr matri3 m3i;
  EXPECT_TRUE((utils::testing::test_type<int, 3, 3>(m3i)));
}