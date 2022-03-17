#include "utils/utils.hpp"
using namespace utils;

//
// This test is garbage, but I don't know of a better way to estimate Quake's root accuracy
//
TEST(math, t_inv_sqrt)
{
  constexpr std::array data{
    0.000234,
    0.001,
    0.05,
    0.2,
    0.5,
    1.0,
    1.345,
    2.28,
    3.1415926,
    6.96969,
    420.042,
    1024.0,
    1337.7331,
    374890.982661993
  };

  constexpr auto precision = 1e6;
  for (auto val : data)
  {
    const auto sqrtStd  = std::sqrt(val);
    const auto sqrtCalc = inv(inv_sqrt(val));
    const auto rate = sqrtCalc / sqrtStd;
    const auto rounded = std::round(rate * precision) / precision;
    EXPECT_DOUBLE_EQ(rounded, 1.0);
  }
}

TEST(math, t_abs)
{
  constexpr auto ip =  42;
  constexpr auto in = -42;
  EXPECT_TRUE(eq_all(ip, abs(ip), abs(in)));

  constexpr auto uns = 42u;
  EXPECT_EQ(uns, abs(uns));

  constexpr auto fp =  42.0f;
  constexpr auto fn = -42.0f;
  EXPECT_FLOAT_EQ(fp, abs(fp));
  EXPECT_FLOAT_EQ(fp, abs(fn));
}

TEST(math, t_sign)
{
  constexpr auto zero  = 0;
  constexpr auto zeroU = 0u;
  EXPECT_EQ(sign(zero),  zero);
  EXPECT_EQ(sign(zeroU), zeroU);

  constexpr auto ip =  42;
  constexpr auto in = -42;
  EXPECT_EQ(sign(ip),  1);
  EXPECT_EQ(sign(in), -1);

  constexpr auto zeroD = 0.0;
  EXPECT_DOUBLE_EQ(zeroD, sign(zeroD));

  constexpr auto fp =  42.0f;
  constexpr auto fn = -42.0f;
  EXPECT_FLOAT_EQ(sign(fp),  1);
  EXPECT_FLOAT_EQ(sign(fn), -1);
}

TEST(math, t_eq)
{
  constexpr auto epsD = std::numeric_limits<double>::epsilon();
  constexpr auto baseD = 0.0;
  constexpr auto lessD = baseD - epsD;
  constexpr auto grtrD = baseD + epsD;
  EXPECT_TRUE (eq(baseD, lessD));
  EXPECT_TRUE (eq(baseD, grtrD));
  EXPECT_FALSE(eq(lessD, grtrD));

  constexpr auto epsF = std::numeric_limits<float>::epsilon();
  constexpr auto baseF = 0.0f;
  constexpr auto lessF = baseF - epsF;
  constexpr auto grtrF = baseF + epsF;
  EXPECT_TRUE (eq(baseF, lessF));
  EXPECT_TRUE (eq(baseF, grtrF));
  EXPECT_FALSE(eq(lessF, grtrF));
}

TEST(math, t_inv)
{
  constexpr auto zero =  -0.0;
  constexpr auto pos  =  2.0;
  constexpr auto neg  = -2.0;

  EXPECT_DOUBLE_EQ(inv(pos),  0.5);
  EXPECT_DOUBLE_EQ(inv(neg), -0.5);
  
  EXPECT_TRUE(std::isinf(inv(zero)));
}

TEST(math, t_factorial)
{
  using int_t = detail::max_int_t;
  constexpr std::array results {
    int_t{ 1 },
    int_t{ 1 },
    int_t{ 2 },
    int_t{ 6 },
    int_t{ 24 },
    int_t{ 120 },
    int_t{ 720 },
    int_t{ 5040 },
    int_t{ 40320 },
    int_t{ 362880 },
    int_t{ 3628800 },
    int_t{ 39916800 },
    int_t{ 479001600 },
    int_t{ 6227020800 },
    int_t{ 87178291200 },
    int_t{ 1307674368000 },
    int_t{ 20922789888000 },
    int_t{ 355687428096000 },
    int_t{ 6402373705728000 },
    int_t{ 121645100408832000 },
    int_t{ 2432902008176640000 }
  };

  using sz_t = decltype(results)::size_type;
  for (auto idx = sz_t{}; idx < results.size(); ++idx)
  {
    EXPECT_EQ(results[idx], factorial(idx));
  }
}