#include "utils/utils.hpp"

namespace ut_tests
{
  TEST(clrs, t_rgb)
  {
    using utils::colour;
    EXPECT_EQ((colour{}), 0x00000000u);
    EXPECT_EQ((colour{ 255,   255, 255 }), 0xFFFFFFFFu);
    EXPECT_EQ((colour{ 255,     0,   0 }), 0xFF0000FFu);
    EXPECT_EQ((colour{ 0,   255,   0 }),   0xFF00FF00u);
    EXPECT_EQ((colour{ 0,     0, 255 }),   0xFFFF0000u);
    EXPECT_EQ((colour{ 0,   255, 255 }),   0xFFFFFF00u);
    EXPECT_EQ((colour{ 255,     0, 255 }), 0xFFFF00FFu);

    EXPECT_EQ((colour{ 255, 10, 15, 128 }), 0x800F0AFFu);
  }

  TEST(clrs, t_norm)
  {
    using utils::colour;

    auto clr = colour{ 255, 0, 255 / 5 }.to_float_norm();
    EXPECT_FLOAT_EQ(1.0f, clr[0]);
    EXPECT_FLOAT_EQ(0.0f, clr[1]);
    EXPECT_FLOAT_EQ(0.2f, clr[2]);
    EXPECT_FLOAT_EQ(1.0f, clr[3]);
  }

  TEST(clrs, t_denorm)
  {
    using utils::colour;

    constexpr auto ref = colour{ 255, 0, 51 };
    
    auto clr1 = colour::from_rgba(1.0f, 0.0f, 0.2f, 1.0f);
    EXPECT_EQ(clr1, ref);

    auto clr2 = colour::from_norm({ 1.0f, 0.0f, 0.2f, 1.0f });
    EXPECT_EQ(clr1, clr2);
  }

  TEST(clrs, t_components)
  {
    using utils::colour;
    colour clr{};
    EXPECT_EQ(clr.red(), 0u);
    EXPECT_EQ(clr.green(), 0u);
    EXPECT_EQ(clr.blue(), 0u);
    EXPECT_EQ(clr.alpha(), 0u);

    clr.set_red(10);
    clr.set_green(15);
    clr.set_blue(128);
    clr.set_alpha(255);
    EXPECT_EQ(clr.red(), 10u);
    EXPECT_EQ(clr.green(), 15u);
    EXPECT_EQ(clr.blue(), 128u);
    EXPECT_EQ(clr.alpha(), 255u);
    EXPECT_EQ(clr, 0xFF800F0Au);
  }

  TEST(clrs, t_conv)
  {
    using utils::colour;
    colour clr{ 0x0A0B0C0D };
    EXPECT_EQ(clr.red(), 13u);
    EXPECT_EQ(clr.green(), 12u);
    EXPECT_EQ(clr.blue(), 11u);
    EXPECT_EQ(clr.alpha(), 10u);

    clr = 0xABCDu;
    EXPECT_EQ(clr.red(), 205u);
    EXPECT_EQ(clr.green(), 171u);
    EXPECT_EQ(clr.blue(), 0u);
    EXPECT_EQ(clr.alpha(), 0u);

    auto func = [](colour::value_type val) noexcept
    {
      EXPECT_EQ(val, 0x0000ABCDu);
    };
    func(clr);
  }

  TEST(clrs, t_bitwise)
  {
    using utils::colour;
    namespace colours = utils::colours;

    constexpr auto alpha = colour{ 0xFF000000u };
    constexpr auto black = colours::black;
    constexpr auto white = colours::white;
    constexpr auto red   = colours::red;
    constexpr auto green = colours::lime;
    constexpr auto blue  = colours::blue;

    EXPECT_EQ((black & white), black);
    EXPECT_EQ((black | white), white);
    EXPECT_EQ((black ^ white), (white & ~alpha));
    EXPECT_EQ((white ^ white), (black & ~alpha));
    EXPECT_EQ(~black, (white & ~alpha));

    EXPECT_EQ((white & ~blue | alpha), colours::yellow);

    EXPECT_EQ((red | blue), colours::chromakey);

    EXPECT_EQ(((red | green | blue) & ~alpha), ~black);
  }

}