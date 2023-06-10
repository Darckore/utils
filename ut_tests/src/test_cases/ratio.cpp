#include "gtest/gtest.h"
#include "utils/utils.hpp"

namespace ut_tests
{
  TEST(ratios, t_basics)
  {
    using utils::ratio;
    using utils::abs;
    using utils::sign;
    using utils::inv;

    constexpr ratio r0{ 1, 2 };
    EXPECT_EQ(r0.num(), 1);
    EXPECT_EQ(r0.denom(), 2);
    EXPECT_TRUE(r0 > 0);

    constexpr ratio r1{ 1, -2 };
    EXPECT_EQ(r1.num(), 1);
    EXPECT_EQ(r1.denom(), 2);
    EXPECT_TRUE(r1 < 0);

    EXPECT_EQ(abs(r1), r0);
    EXPECT_EQ(sign(r0), 1);
    EXPECT_EQ(sign(r1), -1);

    EXPECT_EQ(inv(r1), -2);
  }

  TEST(ratios, t_modifications)
  {
    using utils::ratio;

    auto r = ratio{ 12, 36 };
    EXPECT_EQ(r.num(), 12);
    EXPECT_EQ(r.denom(), 36);
    EXPECT_TRUE(r == (ratio{ 1, 3 }));

    r.simplify();
    EXPECT_EQ(r.num(), 1);
    EXPECT_EQ(r.denom(), 3);

    r.flip();
    EXPECT_EQ(r.num(), 3);
    EXPECT_EQ(r.denom(), 1);
    EXPECT_TRUE(r == 3);

    r += 2;
    r.flip();
    EXPECT_TRUE(r == (ratio{ 1, 5 }));

    r *= ratio{ 5, 6 };
    EXPECT_TRUE(r == (ratio{ 1, 6 }));

    r -= 1;
    EXPECT_TRUE(r == (ratio{ -5, 6 }));

    r /= ratio{ -1, 6 };
    EXPECT_TRUE(r == 5);

    r = ratio{ 1, 4 };
    EXPECT_TRUE(r == 0.25);
    EXPECT_TRUE(r < 0.5);
    EXPECT_TRUE(r > 0.1);
    EXPECT_FALSE(r > 0.5);
    EXPECT_FALSE(r < 0.1);
  }

  TEST(ratios, t_zero_infinity)
  {
    using utils::ratio;

    constexpr auto r0 = ratio{ 1, 0 };
    EXPECT_TRUE(r0.is_infinity());
    constexpr auto infP = r0.to<float>();
    EXPECT_TRUE(std::isinf(infP));
    EXPECT_TRUE(infP > 0);

    constexpr auto r1 = -r0;
    EXPECT_TRUE(r1.is_infinity());
    constexpr auto infN = r1.to<float>();
    EXPECT_TRUE(infN);
    EXPECT_TRUE(infN < 0);

    auto r2 = r1.get_flipped();
    auto r3 = r2 + 5;

    enum class zero_state
    {
      zero,
      nonZero
    };

    auto checkZero = [](auto r) noexcept
    {
      if (r)
        return zero_state::nonZero;

      return zero_state::zero;
    };

    EXPECT_EQ(checkZero(r2), zero_state::zero);
    EXPECT_EQ(checkZero(r3), zero_state::nonZero);
  }

  TEST(ratios, t_arithmetics)
  {
    using utils::ratio;

    constexpr auto r0 = ratio{ 1, 2 };
    constexpr auto r1 = ratio{ 3ull, 7ull, -1 };

    using ct = std::common_type_t<decltype(r0)::value_type, decltype(r1)::value_type>;

    constexpr auto add = r1 + r0;
    EXPECT_TRUE((std::is_same_v<ct, decltype(add)::value_type>));
    EXPECT_TRUE(add == (ratio{ 1, 14 }));

    constexpr auto sub = r1 - r0;
    EXPECT_TRUE((std::is_same_v<ct, decltype(sub)::value_type>));
    EXPECT_TRUE(sub == (ratio{ -13, 14 }));

    constexpr auto mul = r1 * r0;
    EXPECT_TRUE((std::is_same_v<ct, decltype(mul)::value_type>));
    EXPECT_TRUE(mul == (ratio{ -3, 14 }));

    constexpr auto div = r0 / r1;
    EXPECT_TRUE((std::is_same_v<ct, decltype(div)::value_type>));
    EXPECT_TRUE(div == (ratio{ -7, 6 }));
  }

  TEST(ratios, t_arithmetics_neg)
  {
    using utils::ratio;

    constexpr auto r0 = ratio{ -1, 2 };
    constexpr auto r1 = ratio{ -5, 8 };

    constexpr auto add = r0 + r1;
    EXPECT_TRUE(add == (ratio{ -9, 8 }));

    constexpr auto sub = r0 - r1;
    EXPECT_TRUE(sub == (ratio{ 1, 8 }));

    constexpr auto mul = r0 * r1;
    EXPECT_TRUE(mul == (ratio{ 5, 16 }));

    constexpr auto div = r0 / r1;
    EXPECT_TRUE(div == (ratio{ 4, 5 }));
  }

}