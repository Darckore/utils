#include "utils/utils.hpp"

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
}