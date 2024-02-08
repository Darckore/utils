#include "utils/utils.hpp"

namespace ut_tests
{
  namespace
  {
    struct list_node : public utils::ilist_node<list_node>
    {
      int value;

      list_node(list_type& list, int val) noexcept :
        base_type{ list },
        value{ val }
      {}
    };

    struct list_wrapper
    {
      utils::ilist<list_node> list;

      list_wrapper() noexcept
      {
        list.emplace_back(0);
        list.emplace_back(1);
        list.emplace_back(2);
        list.emplace_back(3);
        list.emplace_back(4);
        list.emplace_back(5);
        list.emplace_back(6);
        list.emplace_back(7);
        list.emplace_back(8);
        list.emplace_back(9);
      }
    };

    template <std::size_t N>
    void verify_list(list_wrapper& lw, const std::array<int, N>& baseline) noexcept
    {
      auto&& list = lw.list;
      if (list.empty())
      {
        if (!baseline.empty())
        {
          FAIL() << "List is empty, but baseline has " << baseline.size() << " elements";
          return;
        }
      }

      auto&& first = list.front();
      auto&& last = list.back();
      if (!first.is_head())
      {
        FAIL() << "Head has a prev node";
        return;
      }
      if (!last.is_tail())
      {
        FAIL() << "Tail has a next node";
        return;
      }

      if (utils::eq_any(&first, first.prev(), first.next()))
      {
        FAIL() << "Head refers to itself";
        return;
      }
      if (utils::eq_any(&last, last.prev(), last.next()))
      {
        FAIL() << "Tail refers to itself";
      }

      const auto baseSz = baseline.size();
      for (auto idx = 0ull; auto&& node : list)
      {
        ASSERT_LT(idx, baseSz);

        const auto val = node.value;
        const auto exp = baseline[idx++];
        EXPECT_EQ(val, exp);
      }
    }
  }

  TEST(ilist, t_emplace_back)
  {
    list_wrapper lw;
    verify_list(lw, std::array{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 });
  }

  TEST(ilist, t_emplace_front)
  {
    list_wrapper lw;
    lw.list.emplace_front(42);
    lw.list.emplace_front(69);
    verify_list(lw, std::array{ 69, 42, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 });
  }

  TEST(ilist, t_emplace_before)
  {
    list_wrapper lw;
    auto anchor = lw.list.front().next(); // element 1
    lw.list.emplace_before(*anchor, 42);
    lw.list.emplace_before(*anchor, 69);
    verify_list(lw, std::array{ 0, 42, 69, 1, 2, 3, 4, 5, 6, 7, 8, 9 });
  }

  TEST(ilist, t_emplace_before_front)
  {
    list_wrapper lw;
    auto&& anchor = lw.list.front();
    lw.list.emplace_before(anchor, 42);
    lw.list.emplace_before(anchor, 69);
    verify_list(lw, std::array{ 42, 69, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 });
  }

  TEST(ilist, t_emplace_before_back)
  {
    list_wrapper lw;
    auto&& anchor = lw.list.back();
    lw.list.emplace_before(anchor, 42);
    lw.list.emplace_before(anchor, 69);
    verify_list(lw, std::array{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 42, 69, 9 });
  }

  TEST(ilist, t_emplace_after)
  {
    list_wrapper lw;
    auto anchor = lw.list.front().next(); // element 1
    lw.list.emplace_after(*anchor, 42);
    lw.list.emplace_after(*anchor, 69);
    verify_list(lw, std::array{ 0, 1, 69, 42, 2, 3, 4, 5, 6, 7, 8, 9 });
  }

  TEST(ilist, t_emplace_after_front)
  {
    list_wrapper lw;
    auto&& anchor = lw.list.front();
    lw.list.emplace_after(anchor, 42);
    lw.list.emplace_after(anchor, 69);
    verify_list(lw, std::array{ 0, 69, 42, 1, 2, 3, 4, 5, 6, 7, 8, 9 });
  }

  TEST(ilist, t_emplace_after_back)
  {
    list_wrapper lw;
    auto&& anchor = lw.list.back();
    lw.list.emplace_after(anchor, 42);
    lw.list.emplace_after(anchor, 69);
    verify_list(lw, std::array{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 69, 42 });
  }
}