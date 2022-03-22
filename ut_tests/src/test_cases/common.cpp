#include "utils/utils.hpp"
#include <variant>
using namespace utils;

namespace ut_tests
{
  TEST(common, t_visit)
  {
    std::variant <int, bool, float> var;
    constexpr auto visitor = utils::visitor{
      [](int i)
      {
        return static_cast<double>(i + 228);
      },
      [](bool b)
      {
        return static_cast<double>(!b);
      },
      [](float f)
      {
        return std::sqrt(static_cast<double>(f));
      }
    };

    var = 42;
    const auto r1 = std::visit(visitor, var);
    EXPECT_DOUBLE_EQ(r1, 270.0);

    var = false;
    const auto r2 = std::visit(visitor, var);
    EXPECT_DOUBLE_EQ(r2, 1.0);

    var = 0.25f;
    const auto r3 = std::visit(visitor, var);
    EXPECT_DOUBLE_EQ(r3, 0.5);
  }

  TEST(common, t_in_range)
  {
    constexpr auto rngStart = 0;
    constexpr auto rngEnd = 10;
    constexpr auto valIn = 5.0f;
    constexpr auto valOut = 20.0;

    EXPECT_TRUE(in_range(valIn, rngStart, rngEnd));
    EXPECT_FALSE(in_range(valOut, rngStart, rngEnd));
  }

  TEST(common, t_eq_all)
  {
    constexpr auto baseline = 42;
    constexpr auto ok1 = 42;
    constexpr auto ok2 = 42.0f;
    constexpr auto ok3 = 42ll;
    constexpr auto nope = 69.0;

    EXPECT_TRUE(eq_all(baseline, ok1));
    EXPECT_TRUE(eq_all(baseline, ok1, ok2));
    EXPECT_TRUE(eq_all(baseline, ok1, ok2, ok3));
    EXPECT_FALSE(eq_all(baseline, nope, ok2, ok3));
    EXPECT_FALSE(eq_all(baseline, ok1, nope, ok3));
    EXPECT_FALSE(eq_all(baseline, ok1, ok2, nope));
  }

  TEST(common, t_eq_any)
  {
    constexpr auto baseline = 42;
    constexpr auto eq = 42.0f;
    constexpr auto nope1 = 228.0;
    constexpr auto nope2 = 69ll;
    constexpr auto nope3 = 0.5;

    EXPECT_TRUE(eq_any(baseline, eq));
    EXPECT_TRUE(eq_any(baseline, eq, nope1));
    EXPECT_TRUE(eq_any(baseline, eq, nope1, nope2));
    EXPECT_TRUE(eq_any(baseline, eq, nope1, nope2, nope3));
    EXPECT_FALSE(eq_any(baseline, nope1));
    EXPECT_FALSE(eq_any(baseline, nope1, nope2, nope3));
  }

  TEST(common, t_eq_none)
  {
    constexpr auto baseline = 42;
    constexpr auto eq = 42.0f;
    constexpr auto nope1 = 228.0;
    constexpr auto nope2 = 69ll;
    constexpr auto nope3 = 0.5;

    EXPECT_TRUE(eq_none(baseline, nope1));
    EXPECT_TRUE(eq_none(baseline, nope1, nope2));
    EXPECT_TRUE(eq_none(baseline, nope1, nope2, nope3));
    EXPECT_FALSE(eq_none(baseline, eq, nope1));
    EXPECT_FALSE(eq_none(baseline, eq, nope1, nope2, nope3));
  }

  TEST(common, t_mutate)
  {
    struct test_struc
    {
      int field{};

      int& ref()
      {
        return utils::mutate(std::as_const(*this).ref());
      }
      const int& ref() const
      {
        return field;
      }

      int* ptr()
      {
        return utils::mutate(std::as_const(*this).ptr());
      }
      const int* ptr() const
      {
        return &field;
      }
    };

    auto regular = [](test_struc& dummy)
    {
      auto&& ref = dummy.ref();
      EXPECT_TRUE((std::is_same_v<decltype(ref), int&>));
      EXPECT_EQ(ref, 42);
      ref = 69;
      EXPECT_EQ(dummy.field, 69);

      auto ptr = dummy.ptr();
      ASSERT_TRUE(ptr);
      EXPECT_TRUE((std::is_same_v<decltype(ptr), int*>));

      *ptr = 228;
      EXPECT_EQ(dummy.field, 228);
    };
    auto constRef = [](const test_struc& dummy)
    {
      auto&& ref = dummy.ref();
      EXPECT_TRUE((std::is_same_v<decltype(ref), const int&>));
      EXPECT_EQ(ref, 228);

      auto ptr = dummy.ptr();
      EXPECT_TRUE((std::is_same_v<decltype(ptr), const int*>));
      EXPECT_EQ(*ptr, 228);
    };

    test_struc thing{ 42 };
    regular(thing);
    constRef(thing);
  }

}