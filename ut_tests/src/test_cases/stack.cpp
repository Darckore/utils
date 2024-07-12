#include "utils/utils.hpp"

namespace ut_tests
{
  namespace
  {
    struct stack_item
    {
      CLASS_SPECIALS_ALL(stack_item);
      stack_item(int v) noexcept : value{ v }
      {}
      int value{};
    };

    struct stack_item_nodef
    {
      CLASS_SPECIALS_NODEFAULT(stack_item_nodef);
      stack_item_nodef(int v) noexcept : value{ v }
      {}
      int value{};
    };

    using stack_t = utils::stack<stack_item>;
    using nodef_stack_t = utils::stack<stack_item_nodef>;
  }
}

namespace ut_tests
{
  // Meaningless, just for instantiations, basically
  TEST(stack, t_basics)
  {
    utils::stack<int> st;
    st.push(1);
    st.push(2);
    st.push(3);

    ASSERT_EQ(st.size(), 3);
    auto top = int{};

    top = st.top(); st.pop();
    ASSERT_EQ(top, 3);
    top = st.top(); st.pop();
    ASSERT_EQ(top, 2);
    top = st.top(); st.pop();
    ASSERT_EQ(top, 1);

    ASSERT_TRUE(st.empty());
  }

  TEST(stack, t_extract)
  {
    stack_t st;
    st.push(42);

    auto v = st.extract();
    ASSERT_EQ(v.value, 42);
    ASSERT_TRUE(st.empty());

    v = st.extract();
    ASSERT_EQ(v.value, 0);

    v = st.extract(-42);
    ASSERT_EQ(v.value, -42);

    nodef_stack_t nds;
    auto nv = nds.extract(42);
    ASSERT_EQ(nv.value, 42);
  }
}