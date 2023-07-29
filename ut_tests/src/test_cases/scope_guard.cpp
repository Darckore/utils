#include "utils/utils.hpp"

namespace ut_tests
{
  TEST(scopes, t_val)
  {
    int x = 42;
    {
      utils::value_guard _{ x };
      x = 69;
    }
    ASSERT_EQ(x, 42);

    {
      utils::value_guard _{ x, 69 };
      ASSERT_EQ(x, 69);
    }
    ASSERT_EQ(x, 42);
  }

  TEST(scopes, t_val_macro)
  {
    int x = 42;
    {
      VALUE_GUARD(x);
      x = 69;
    }
    ASSERT_EQ(x, 42);

    {
      VALUE_GUARD(x, 69);
      ASSERT_EQ(x, 69);
    }
    ASSERT_EQ(x, 42);
  }

  TEST(scopes, t_term)
  {
    int x = 0;
    auto at_exit = [&x]() noexcept { x = 42; };
    {
      utils::scope_terminator _{ at_exit };
    }
    ASSERT_EQ(x, 42);
  }

  TEST(scopes, t_term_macro)
  {
    struct tester
    {
      int x = 0;

      void change(int val) noexcept
      {
        x = val;
      }
    };

    tester t{};
    {
      SCOPE_GUARD(int a = 41; t.change(a + 1));
    }
    ASSERT_EQ(t.x, 42);
  }
}