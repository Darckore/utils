#include "utils/utils.hpp"

namespace ut_tests
{
  TEST(mul_iter, t_same)
  {
    using utils::multiple_it;
    std::array a1{ 1, 2 };
    std::array a2{ 3, 4 };
    std::array a3{ 5, 6 };

    int one{};
    int two{};
    int three{};
    multiple_it beg{ a1.begin(), a2.begin(), a3.begin() };
    multiple_it end{ a1.end(), a2.end(), a3.end() };
    ASSERT_NE(beg, end);
    std::tie(one, two, three) = *beg;
    EXPECT_EQ(one, 1);
    EXPECT_EQ(two, 3);
    EXPECT_EQ(three, 5);
    ++beg;
    ASSERT_NE(beg, end);
    std::tie(one, two, three) = *beg;
    EXPECT_EQ(one, 2);
    EXPECT_EQ(two, 4);
    EXPECT_EQ(three, 6);
    ++beg;
    ASSERT_EQ(beg, end);
  }

  TEST(mul_iter, t_dif_len)
  {
    using utils::multiple_it;
    std::array a1{ 1, 2 };
    std::array a2{ 1, 2, 3, 4, 5, 6 };

    multiple_it beg{ a1.begin(), a2.begin() };
    multiple_it end{ a1.end(), a2.end() };
    ASSERT_NE(beg, end);
    std::advance(beg, 2);
    ASSERT_EQ(beg, end);
  }

  TEST(mul_iter, t_iterations)
  {
    using utils::multiple_it;
    std::array a1{ 1, 2, 3 };
    std::array a2{ 1, 2, 3, 4, 5, 6 };
    std::array a3{ 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    multiple_it beg{ a1.begin(), a2.begin(), a3.begin() };
    multiple_it end{ a1.end(), a2.end(), a3.end() };
    auto itCount = 0;
    for (auto it = beg; it != end; ++it)
      ++itCount;
    ASSERT_EQ(itCount, 3);
  }

  TEST(mul_iter, t_iter_pair)
  {
    using utils::detail::iterator_pair;
    std::array a1{ 1, 2 };
    const std::array a2{ 1, 2, 3 };

    iterator_pair ip{ a1, a2 };
    auto itCount = 0;
    for (auto&& _ : ip)
    {
      utils::unused(_);
      ++itCount;
    }

    ASSERT_EQ(itCount, 2);
  }

  TEST(mul_iter, t_make_pair)
  {
    using utils::detail::iterator_pair;
    using utils::make_iterators;
    std::array a1{ 1, 2 };
    std::array a2{ 1, 2, 3 };

    auto iters = make_iterators(a1, a2);
    using iters_t = decltype(iters);

    iterator_pair ip{ a1, a2 };
    using pair_t = decltype(ip);

    ASSERT_TRUE((std::is_same_v<iters_t, pair_t>));
    ASSERT_EQ(iters, ip);
  }
}