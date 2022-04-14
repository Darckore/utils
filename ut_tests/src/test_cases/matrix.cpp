#include "utils/utils.hpp"
#include "tests/helpers.hpp"
using namespace utils;

namespace ut_tests
{
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

  TEST(matr, t_conv)
  {
    constexpr matrf2 m{
      vecf2{ 1, 2 },
      vecf2{ 3, 4 } };

    constexpr auto m2f = m;
    EXPECT_TRUE((utils::testing::test_type<float, 2, 2>(m2f)));
    EXPECT_FLOAT_EQ(m2f[0][0], 1.0f);
    EXPECT_FLOAT_EQ(m2f[0][1], 2.0f);
    EXPECT_FLOAT_EQ(m2f[1][0], 3.0f);
    EXPECT_FLOAT_EQ(m2f[1][1], 4.0f);

    constexpr auto m2i = m.to<int, 2, 2>();
    EXPECT_TRUE((utils::testing::test_type<int, 2, 2>(m2i)));
    EXPECT_EQ(m2i[0][0], 1);
    EXPECT_EQ(m2i[0][1], 2);
    EXPECT_EQ(m2i[1][0], 3);
    EXPECT_EQ(m2i[1][1], 4);

    constexpr matrd3 m3d = m2i;
    EXPECT_TRUE((utils::testing::test_type<double, 3, 3>(m3d)));
    EXPECT_DOUBLE_EQ(m3d[0][0], 1.0);
    EXPECT_DOUBLE_EQ(m3d[0][1], 2.0);
    EXPECT_DOUBLE_EQ(m3d[0][2], 0.0);
    EXPECT_DOUBLE_EQ(m3d[1][0], 3.0);
    EXPECT_DOUBLE_EQ(m3d[1][1], 4.0);
    EXPECT_DOUBLE_EQ(m3d[1][2], 0.0);
    EXPECT_DOUBLE_EQ(m3d[2][0], 0.0);
    EXPECT_DOUBLE_EQ(m3d[2][1], 0.0);
    EXPECT_DOUBLE_EQ(m3d[2][2], 0.0);
  }

  TEST(matr, t_compare)
  {
    constexpr auto m3f = matrix{
      vector{ 69.0f, 42,  228 },
      vector{    -1,  5,   19 },
      vector{   666, 11,  -14 } };

    constexpr matri3 m3i{
      vector{    69, 42,  228 },
      vector{    -1,  5,   19 },
      vector{   666, 11,  -14 } };
    EXPECT_TRUE(eq(m3f, m3i));
    EXPECT_TRUE(m3f == m3i);
    EXPECT_FALSE(m3f != m3i);

    constexpr auto m3d = matrix{
      vector{ 0.0, 42,  228 },
      vector{  -1,  5,   19 },
      vector{  42, 11,  -14 } };
    EXPECT_TRUE(m3d != m3i);
    EXPECT_FALSE(m3d == m3i);

    constexpr matri2 m2i = m3f;
    EXPECT_TRUE(m2i != m3f);
    EXPECT_FALSE(m2i == m3f);
  }

  TEST(matr, t_identity)
  {
    constexpr auto id3d = matrd3::identity();
    EXPECT_DOUBLE_EQ((id3d.get<0, 0>()), 1.0);
    EXPECT_DOUBLE_EQ((id3d.get<0, 1>()), 0.0);
    EXPECT_DOUBLE_EQ((id3d.get<0, 2>()), 0.0);
    EXPECT_DOUBLE_EQ((id3d.get<1, 0>()), 0.0);
    EXPECT_DOUBLE_EQ((id3d.get<1, 1>()), 1.0);
    EXPECT_DOUBLE_EQ((id3d.get<1, 2>()), 0.0);
    EXPECT_DOUBLE_EQ((id3d.get<2, 0>()), 0.0);
    EXPECT_DOUBLE_EQ((id3d.get<2, 1>()), 0.0);
    EXPECT_DOUBLE_EQ((id3d.get<2, 2>()), 1.0);
  }

  TEST(matr, t_scale)
  {
    constexpr auto mul = 10.0;
    constexpr auto div = 20.0;

    matrd2 m{
      vecd2{ 1, 2 },
      vecd2{ 3, 4 } };

    const auto mc = m;
    auto&& [r1, r2] = m;

    const auto mm = m * mul;
    auto&& [mr1, mr2] = mm;
    EXPECT_TRUE(mr1 == (r1 * mul));
    EXPECT_TRUE(mr2 == (r2 * mul));

    const auto md = m / div;
    auto&& [dr1, dr2] = md;
    EXPECT_TRUE(dr1 == (r1 / div));
    EXPECT_TRUE(dr2 == (r2 / div));

    m *= mul;
    EXPECT_TRUE(r1 == mr1);
    EXPECT_TRUE(r2 == mr2);

    m = mc;
    m /= div;
    EXPECT_TRUE(r1 == dr1);
    EXPECT_TRUE(r2 == dr2);

    m = mc;
    m.scale(mul);
    EXPECT_FALSE(m == mc);
    m.scale_inv(mul);
    EXPECT_TRUE(m == mc);
  }

  TEST(matr, t_arithmetic)
  {
    matrix m1{ vector{ 1.0,   2.0 },
               vector{ 5.0,  -6.0 } };
    matrix m2{ vector{ 3.0f, -4.0f },
               vector{ 5.0,  -6.0 } };

    constexpr matrix mneg{ vector{ -3.0, 4.0 },
                           vector{ -5.0, 6.0 } };
    EXPECT_TRUE(mneg == (-m2));

    constexpr matrix msum{ vector{  4.0,  -2.0 },
                           vector{ 10.0, -12.0 } };
    EXPECT_TRUE(msum == (m1 + m2));

    constexpr matrix mdif{ vector{ -2.0, 6.0 },
                           vector{  0.0, 0.0 } };
    EXPECT_TRUE(mdif == (m1 - m2));

    matrd2 m3 = m2;

    m3 += m1;
    EXPECT_TRUE(m3 == msum);
    m3 -= m1;
    EXPECT_TRUE(m3 == m2);
  }

  TEST(matr, t_column)
  {
    constexpr matrix m{
      vector{ 1, 2 },
      vector{ 3, 4 },
      vector{ 5, 6 } };

    constexpr auto c1 = m.column<0>();
    constexpr auto c2 = m.column<1>();

    EXPECT_TRUE(c1 == (vector{ 1, 3, 5 }));
    EXPECT_TRUE(c2 == (vector{ 2, 4, 6 }));
  }

  TEST(matr, t_mul)
  {
    constexpr matrix m1{
      vector{ 0,   4, -2 },
      vector{ -4, -3,  0 } };

    constexpr matrix m2{
      vector{ 0.0,  1 },
      vector{   1, -1 },
      vector{   2,  3 } };

    constexpr auto mmul = m1 * m2;
    EXPECT_TRUE((utils::testing::test_type<double, 2, 2>(mmul)));

    constexpr auto rmul = m2 * m1;
    EXPECT_TRUE((utils::testing::test_type<double, 3, 3>(rmul)));
    EXPECT_FALSE(rmul == mmul);

    // Calculated elsewhere
    constexpr matrix res{ vector{  0, -10 },
                          vector{ -3,  -1 } };
    EXPECT_TRUE(res == mmul);
  }

  TEST(matr, t_rotation2d)
  {
    constexpr vecd2 v{ 1.0, 2.0 };
    constexpr auto angle = deg_to_rad(60.0);
    constexpr auto vr0 = v.get_rotated(angle);

    constexpr auto rm = matrd2::rotation(angle);
    constexpr auto vr = rm * v;

    EXPECT_TRUE(eq(vr, vr0));
  }

  TEST(matr, t_transpose)
  {
    constexpr auto m = matrix{ vector{  1,  2,  3 },
                               vector{  4,  5,  6 },
                               vector{  7,  8,  9 },
                               vector{ 10, 11, 12 } };

    constexpr auto mexpr = matrix{ vector { 1, 4, 7, 10 },
                                   vector { 2, 5, 8, 11 },
                                   vector { 3, 6, 9, 12 } };

    const auto mt = m.transpose();
    EXPECT_TRUE(mt == mexpr);
  }
}