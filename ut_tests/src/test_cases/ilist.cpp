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

      template <typename ...Values> requires (utils::all_same<int, Values...>)
      list_wrapper(Values ...vals) noexcept
      {
        (..., (list.emplace_back(vals)));
      }
    };

    template <std::size_t N>
    void verify_list(utils::ilist<list_node>& list, const std::array<int, N>& baseline) noexcept
    {
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
      auto idx = 0ull;
      for (auto&& node : list)
      {
        ASSERT_LT(idx, baseSz);

        const auto val = node.value;
        const auto exp = baseline[idx++];
        EXPECT_EQ(val, exp);
      }

      ASSERT_EQ(idx, baseSz);
    }

    template <std::size_t N>
    void verify_list(list_wrapper& lw, const std::array<int, N>& baseline) noexcept
    {
      verify_list(lw.list, baseline);
    }
  }

  TEST(ilist, t_emplace_back)
  {
    list_wrapper lw{ 0, 1, 2, 3, 4, 5, 6 };
    verify_list(lw, std::array{ 0, 1, 2, 3, 4, 5, 6 });
  }

  TEST(ilist, t_emplace_front)
  {
    list_wrapper lw{ 0, 1, 2, 3, 4 };
    lw.list.emplace_front(42);
    lw.list.emplace_front(69);
    verify_list(lw, std::array{ 69, 42, 0, 1, 2, 3, 4 });
  }

  TEST(ilist, t_emplace_before)
  {
    list_wrapper lw{ 0, 1, 2, 3, 4 };
    auto anchor = lw.list.front().next(); // element 1
    lw.list.emplace_before(*anchor, 42);
    lw.list.emplace_before(*anchor, 69);
    verify_list(lw, std::array{ 0, 42, 69, 1, 2, 3, 4 });
  }

  TEST(ilist, t_emplace_before_front)
  {
    list_wrapper lw{ 0, 1, 2, 3, 4 };
    auto&& anchor = lw.list.front();
    lw.list.emplace_before(anchor, 42);
    lw.list.emplace_before(anchor, 69);
    verify_list(lw, std::array{ 42, 69, 0, 1, 2, 3, 4 });
  }

  TEST(ilist, t_emplace_before_back)
  {
    list_wrapper lw{ 0, 1, 2, 3, 4 };
    auto&& anchor = lw.list.back();
    lw.list.emplace_before(anchor, 42);
    lw.list.emplace_before(anchor, 69);
    verify_list(lw, std::array{ 0, 1, 2, 3, 42, 69, 4 });
  }

  TEST(ilist, t_emplace_after)
  {
    list_wrapper lw{ 0, 1, 2, 3, 4 };
    auto anchor = lw.list.front().next(); // element 1
    lw.list.emplace_after(*anchor, 42);
    lw.list.emplace_after(*anchor, 69);
    verify_list(lw, std::array{ 0, 1, 69, 42, 2, 3, 4 });
  }

  TEST(ilist, t_emplace_after_front)
  {
    list_wrapper lw{ 0, 1, 2, 3, 4 };
    auto&& anchor = lw.list.front();
    lw.list.emplace_after(anchor, 42);
    lw.list.emplace_after(anchor, 69);
    verify_list(lw, std::array{ 0, 69, 42, 1, 2, 3, 4 });
  }

  TEST(ilist, t_emplace_after_back)
  {
    list_wrapper lw{ 0, 1, 2, 3, 4 };
    auto&& anchor = lw.list.back();
    lw.list.emplace_after(anchor, 42);
    lw.list.emplace_after(anchor, 69);
    verify_list(lw, std::array{ 0, 1, 2, 3, 4, 69, 42 });
  }

  TEST(ilist, t_remove_before)
  {
    list_wrapper lw{ 0, 1, 2, 3, 4 };
    auto kill = lw.list.front().next()->next(); // element 2
    lw.list.remove_before(*kill);
    verify_list(lw, std::array{ 0, 2, 3, 4 });
  }

  TEST(ilist, t_remove_before_front)
  {
    list_wrapper lw{ 0, 1, 2, 3, 4 };
    auto&& kill = lw.list.front();
    lw.list.remove_before(kill);
    verify_list(lw, std::array{ 0, 1, 2, 3, 4 });
  }

  TEST(ilist, t_remove_before_back)
  {
    list_wrapper lw{ 0, 1, 2, 3, 4 };
    auto&& kill = lw.list.back();
    lw.list.remove_before(kill);
    verify_list(lw, std::array{ 0, 1, 2, 4 });
  }

  TEST(ilist, t_remove_after)
  {
    list_wrapper lw{ 0, 1, 2, 3, 4 };
    auto kill = lw.list.front().next()->next(); // element 2
    lw.list.remove_after(*kill);
    verify_list(lw, std::array{ 0, 1, 2, 4 });
  }

  TEST(ilist, t_remove_after_front)
  {
    list_wrapper lw{ 0, 1, 2, 3, 4 };
    auto&& kill = lw.list.front();
    lw.list.remove_after(kill);
    verify_list(lw, std::array{ 0, 2, 3, 4 });
  }

  TEST(ilist, t_remove_after_back)
  {
    list_wrapper lw{ 0, 1, 2, 3, 4 };
    auto&& kill = lw.list.back();
    lw.list.remove_after(kill);
    verify_list(lw, std::array{ 0, 1, 2, 3, 4 });
  }

  TEST(ilist, t_remove)
  {
    list_wrapper lw{ 0, 1, 2, 3, 4 };
    auto kill = lw.list.front().next()->next(); // element 2
    lw.list.remove(*kill);
    verify_list(lw, std::array{ 0, 1, 3, 4 });
  }

  TEST(ilist, t_remove_front)
  {
    list_wrapper lw{ 0, 1, 2, 3, 4 };
    auto&& kill = lw.list.front();
    lw.list.remove(kill);
    verify_list(lw, std::array{ 1, 2, 3, 4 });
  }

  TEST(ilist, t_remove_back)
  {
    list_wrapper lw{ 0, 1, 2, 3, 4 };
    auto&& kill = lw.list.back();
    lw.list.remove(kill);
    verify_list(lw, std::array{ 0, 1, 2, 3 });
  }

  TEST(ilist, t_pop_front)
  {
    list_wrapper lw{ 0, 1, 2, 3, 4 };
    lw.list.pop_front();
    lw.list.pop_front();
    verify_list(lw, std::array{ 2, 3, 4 });
  }

  TEST(ilist, t_pop_back)
  {
    list_wrapper lw{ 0, 1, 2, 3, 4 };
    lw.list.pop_back();
    lw.list.pop_back();
    verify_list(lw, std::array{ 0, 1, 2 });
  }

  TEST(ilist, t_split)
  {
    list_wrapper lw{ 0, 1, 2, 3, 4 };
    auto split = lw.list.front().next()->next(); // element 2
    auto other = lw.list.split_at(*split);
    verify_list(lw, std::array{ 0, 1 });
    verify_list(other, std::array{ 2, 3, 4 });
  }

  TEST(ilist, t_split_front)
  {
    list_wrapper lw{ 0, 1, 2, 3, 4 };
    auto&& split = lw.list.front();
    auto other = lw.list.split_at(split);
    ASSERT_TRUE(lw.list.empty());
    verify_list(other, std::array{ 0, 1, 2, 3, 4 });
  }

  TEST(ilist, t_split_back)
  {
    list_wrapper lw{ 0, 1, 2, 3, 4 };
    auto&& split = lw.list.back();
    auto other = lw.list.split_at(split);
    verify_list(lw, std::array{ 0, 1, 2, 3 });
    verify_list(other, std::array{ 4 });
  }

  TEST(ilist, t_reorder)
  {
    list_wrapper lw{ 0, 1, 2, 3, 4 };
    auto e1 = lw.list.front().next(); // element 1
    auto e3 = e1->next()->next(); // element 3
    lw.list.reorder(*e1, *e3);
    verify_list(lw, std::array{ 0, 3, 2, 1, 4 });
  }

  TEST(ilist, t_reorder_front)
  {
    list_wrapper lw{ 0, 1, 2, 3, 4 };
    auto&& e0 = lw.list.front(); // element 0
    auto e2 = e0.next()->next(); // element 2
    lw.list.reorder(e0, *e2);
    verify_list(lw, std::array{ 2, 1, 0, 3, 4 });
  }

  TEST(ilist, t_reorder_back)
  {
    list_wrapper lw{ 0, 1, 2, 3, 4 };
    auto&& e0 = lw.list.front(); // element 0
    auto e2 = e0.next()->next(); // element 2
    lw.list.reorder(lw.list.back(), *e2);
    verify_list(lw, std::array{ 0, 1, 4, 3, 2 });
  }

  TEST(ilist, t_reorder_front_back)
  {
    list_wrapper lw{ 0, 1, 2, 3, 4 };
    auto&& e0 = lw.list.front(); // element 0
    auto&& e4 = lw.list.back(); // element 4
    lw.list.reorder(e0, e4);
    verify_list(lw, std::array{ 4, 1, 2, 3, 0 });
  }

  TEST(ilist, t_reorder_adjacent)
  {
    list_wrapper lw{ 0, 1, 2, 3 };
    auto e1 = lw.list.front().next(); // element 1
    auto e2 = e1->next(); // element 2
    lw.list.reorder(*e1, *e2);
    verify_list(lw, std::array{ 0, 2, 1, 3 });
  }
}