#include "utils/utils.hpp"
using namespace utils;

namespace
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

TEST(vect, t_deduction)
{
  constexpr vector v2i{ 1, 2 };
  EXPECT_TRUE((test_type<int, 2>(v2i)));

  constexpr vector v3f{ 1.0f, 2.0f, 3.0f };
  EXPECT_TRUE((test_type<float, 3>(v3f)));

  constexpr vector v4d{ 1.0, 2.0, 3.0, 4.0 };
  EXPECT_TRUE((test_type<double, 4>(v4d)));

  constexpr vector v2if{ 1, 2.0f };
  EXPECT_TRUE((test_type<int, 2>(v2if)));

  constexpr vector v3fdi{ 1.0f, 2.0, 3 };
  EXPECT_TRUE((test_type<float, 3>(v3fdi)));

  constexpr vector v4difll{ 1.0, 2, 3.0f, 4ll };
  EXPECT_TRUE((test_type<double, 4>(v4difll)));
}

TEST(vect, t_aliases)
{
  constexpr vecf2 v2f;
  EXPECT_TRUE((test_type<float, 2>(v2f)));

  constexpr vecf3 v3f;
  EXPECT_TRUE((test_type<float, 3>(v3f)));

  constexpr vecd2 v2d;
  EXPECT_TRUE((test_type<double, 2>(v2d)));

  constexpr vecd3 v3d;
  EXPECT_TRUE((test_type<double, 3>(v3d)));

  constexpr point2d v2i;
  EXPECT_TRUE((test_type<int, 2>(v2i)));

  constexpr point3d v3i;
  EXPECT_TRUE((test_type<int, 3>(v3i)));
}

TEST(vect, t_conv)
{
  constexpr vecf2 v{ 1, 2 };

  constexpr auto v2f = v;
  EXPECT_TRUE((test_type<float, 2>(v2f)));
  EXPECT_FLOAT_EQ(v2f[0], 1.0f);
  EXPECT_FLOAT_EQ(v2f[1], 2.0f);

  constexpr auto v3i = static_cast<point3d>(v);
  EXPECT_TRUE((test_type<int, 3>(v3i)));
  EXPECT_EQ(v3i[0], 1);
  EXPECT_EQ(v3i[1], 2);
  EXPECT_EQ(v3i[2], 0);

  constexpr auto v4d = v3i.to<double, 4>();
  EXPECT_TRUE((test_type<double, 4>(v4d)));
  EXPECT_DOUBLE_EQ(v4d[0], 1.0);
  EXPECT_DOUBLE_EQ(v4d[1], 2.0);
  EXPECT_DOUBLE_EQ(v4d[2], 0.0);
  EXPECT_DOUBLE_EQ(v4d[3], 0.0);

  constexpr point2d v2i = v4d;
  EXPECT_EQ(v2i[0], 1);
  EXPECT_EQ(v2i[1], 2);
}

TEST(vect, t_compare)
{
  constexpr auto v3f = vector{ 69.0f, 42.0f, 228.0f };
  
  constexpr point3d v3i{ 69, 42, 228 };
  EXPECT_TRUE(v3f == v3i);
  EXPECT_FALSE(v3f != v3i);

  constexpr auto v3d = vector{ 0.0, 42.0, 69.0 };
  EXPECT_TRUE(v3d != v3i);
  EXPECT_FALSE(v3d == v3i);

  constexpr point2d v2i = v3f;
  EXPECT_TRUE(v2i != v3f);
  EXPECT_FALSE(v2i == v3f);
}

TEST(vect, t_axes)
{
  using vt = vector<float, 4>;
  
  constexpr auto n0 = vt::axis_norm<0>();
  EXPECT_TRUE((n0 == vt{ 1 }));
  EXPECT_FLOAT_EQ(n0.len(), 1.0f);

  constexpr auto n1 = vt::axis_norm<1>();
  EXPECT_TRUE((n1 == vt{ 0, 1 }));
  EXPECT_FLOAT_EQ(n1.len(), 1.0f);

  constexpr auto n2 = vt::axis_norm<2>();
  EXPECT_TRUE((n2 == vt{ 0, 0, 1 }));
  EXPECT_FLOAT_EQ(n2.len(), 1.0f);

  constexpr auto n3 = vt::axis_norm<3>();
  EXPECT_TRUE((n3 == vt{ 0, 0, 0, 1 }));
  EXPECT_FLOAT_EQ(n3.len(), 1.0f);
}

TEST(vect, t_norm)
{
  constexpr auto x =  10.0f;
  constexpr auto y =  -3.05f;
  constexpr auto z =  4.3773f;
  constexpr auto exLSQ = x * x + y * y + z * z;

  constexpr vecf3 v3f{ x, y, z };

  constexpr auto lenSq = v3f.len_sq();
  EXPECT_FLOAT_EQ(lenSq, exLSQ);

  constexpr auto v3norm  = v3f.get_normalised();
  constexpr auto lsqNorm = v3norm.len_sq();
  constexpr auto lNorm   = v3norm.len();
  EXPECT_FLOAT_EQ(lsqNorm, lNorm);
  EXPECT_TRUE(v3norm.is_normalised());
}

TEST(vect, t_scale)
{
  constexpr auto mul = 10.0;
  constexpr auto div = 20.0;

  vector v{ 1.0, 2.0, 0.5 };
  auto&& [x, y, z] = v;
  
  const auto vs = v * mul;
  auto&& [sx, sy, sz] = vs;
  EXPECT_DOUBLE_EQ(sx, (x * mul));
  EXPECT_DOUBLE_EQ(sy, (y * mul));
  EXPECT_DOUBLE_EQ(sz, (z * mul));
}