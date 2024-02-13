#include "utils/utils.hpp"

namespace ut_tests
{
  namespace
  {
    struct list_node : public utils::ilist_node<list_node>
    {
      int value;

      constexpr bool operator==(const list_node&) const noexcept = default;
      constexpr auto operator<=>(const list_node&) const noexcept = default;

      list_node(list_type& list, int val) noexcept :
        base_type{ list },
        value{ val }
      {}
    };

    struct list_wrapper
    {
      utils::ilist<list_node> list{};

      template <typename ...Values> requires (utils::all_same<int, Values...>)
        list_wrapper(Values ...vals) noexcept
      {
        (..., (list.emplace_back(vals)));
      }
    };

    template <bool Reverse, typename T, typename A, std::size_t N>
    void verify_list(utils::ilist_view<Reverse, T, A> list, const std::array<int, N>& baseline) noexcept
    {
      if (list.size() != baseline.size())
      {
        FAIL() << "Baseline with " << baseline.size()
          << " items is compared to a list view of size " << list.size();
        return;
      }

      for (auto&& [node, exp] : utils::make_iterators(list, baseline))
      {
        EXPECT_EQ(node.value, exp);
      }
    }

    template <std::size_t N>
    void verify_list(const utils::ilist<list_node>& list, const std::array<int, N>& baseline) noexcept
    {
      if (list.size() != baseline.size())
      {
        FAIL() << "Baseline with " << baseline.size()
          << " items is compared to a list of size " << list.size();
        return;
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

      if (first.same_as_any(first.prev(), first.next()))
      {
        FAIL() << "Head refers to itself";
        return;
      }
      if (last.same_as_any(last.prev(), last.next()))
      {
        FAIL() << "Tail refers to itself";
      }

      auto actualLast = decltype(&last){};
      auto actualFirst = decltype(&first){};
      for (auto&& [node, exp] : utils::make_iterators(list, baseline))
      {
        if (!node.is_attached())
        {
          FAIL() << "Detached node with value " << node.value;
          return;
        }

        if (!node.belongs_to(list))
        {
          FAIL() << "Foreign node with value " << node.value;
          return;
        }

        if (!actualFirst) actualFirst = &node;
        actualLast = &node;
        EXPECT_EQ(node.value, exp);
      }

      ASSERT_TRUE(first.same_as(actualFirst));
      ASSERT_TRUE(last.same_as(actualLast));
    }

    template <std::size_t N>
    void verify_list(list_wrapper& lw, const std::array<int, N>& baseline) noexcept
    {
      verify_list(lw.list, baseline);
    }
  }
}

namespace ut_tests
{
  TEST(ilist, t_move)
  {
    list_wrapper lw{ 0, 1, 2, 3, 4, 5, 6 };
    
    decltype(lw.list) consumer{};
    consumer.emplace_back(7);
    consumer.emplace_back(8);

    consumer = std::move(lw.list);
    verify_list(consumer, std::array{ 0, 1, 2, 3, 4, 5, 6 });

    ASSERT_TRUE(lw.list.empty());
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

  TEST(ilist, t_attach_front)
  {
    list_wrapper src{ 1, 2, 3, 4 };
    list_wrapper dst{};

    auto item = &src.list.front();
    src.list.detach(*item);
    dst.list.attach_front(*item);
    verify_list(dst, std::array{ 1 });

    item = &src.list.back();
    src.list.detach(*item);
    dst.list.attach_front(*item);
    verify_list(dst, std::array{ 4, 1 });
  }

  TEST(ilist, t_attach_back)
  {
    list_wrapper src{ 1, 2, 3, 4 };
    list_wrapper dst{};

    auto item = &src.list.front();
    src.list.detach(*item);
    dst.list.attach_back(*item);
    verify_list(dst, std::array{ 1 });

    item = &src.list.back();
    src.list.detach(*item);
    dst.list.attach_back(*item);
    verify_list(dst, std::array{ 1, 4 });
  }

  TEST(ilist, t_attach_before)
  {
    list_wrapper src{ 1, 2, 3, 4 };
    list_wrapper dst{ 5 };

    auto item = &src.list.front();
    src.list.detach(*item);
    dst.list.attach_before(dst.list.front(), *item);
    verify_list(dst, std::array{ 1, 5 });

    item = &src.list.back();
    src.list.detach(*item);
    dst.list.attach_before(dst.list.back(), *item);
    verify_list(dst, std::array{ 1, 4, 5 });
  }

  TEST(ilist, t_attach_after)
  {
    list_wrapper src{ 1, 2, 3, 4 };
    list_wrapper dst{ 5 };

    auto item = &src.list.front();
    src.list.detach(*item);
    dst.list.attach_after(dst.list.front(), *item);
    verify_list(dst, std::array{ 5, 1 });

    item = &src.list.back();
    src.list.detach(*item);
    dst.list.attach_after(dst.list.back(), *item);
    verify_list(dst, std::array{ 5, 1, 4 });
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

  TEST(ilist, t_detach)
  {
    list_wrapper lw{ 0, 1, 2, 3, 4 };
    auto kill = lw.list.front().next()->next(); // element 2
    lw.list.detach(*kill);
    verify_list(lw, std::array{ 0, 1, 3, 4 });
    EXPECT_TRUE(kill->is_head());
    EXPECT_TRUE(kill->is_tail());
    EXPECT_FALSE(kill->is_attached());
    delete kill;
  }

  TEST(ilist, t_detach_front)
  {
    list_wrapper lw{ 0, 1, 2, 3, 4 };
    auto kill = &lw.list.front();
    lw.list.detach(*kill);
    verify_list(lw, std::array{ 1, 2, 3, 4 });
    EXPECT_TRUE(kill->is_head());
    EXPECT_TRUE(kill->is_tail());
    EXPECT_FALSE(kill->is_attached());
    delete kill;
  }

  TEST(ilist, t_detach_back)
  {
    list_wrapper lw{ 0, 1, 2, 3, 4 };
    auto kill = &lw.list.back();
    lw.list.detach(*kill);
    verify_list(lw, std::array{ 0, 1, 2, 3 });
    EXPECT_TRUE(kill->is_head());
    EXPECT_TRUE(kill->is_tail());
    EXPECT_FALSE(kill->is_attached());
    delete kill;
  }

  TEST(ilist, t_pop_front)
  {
    list_wrapper lw{ 0, 1, 2, 3, 4 };
    lw.list.pop_front().pop_front();
    verify_list(lw, std::array{ 2, 3, 4 });
  }

  TEST(ilist, t_pop_back)
  {
    list_wrapper lw{ 0, 1, 2, 3, 4 };
    lw.list.pop_back().pop_back();
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

  TEST(ilist, t_reorder_adjacent_front_back)
  {
    list_wrapper lw{ 0, 1 };
    lw.list.reorder(lw.list.front(), lw.list.back());
    verify_list(lw, std::array{ 1, 0 });

    lw.list.reorder(lw.list.back(), lw.list.front());
    verify_list(lw, std::array{ 0, 1 });
  }

  TEST(ilist, t_reverse)
  {
    list_wrapper lw{ 1, 2, 3, 4 };
    lw.list.reverse();
    verify_list(lw, std::array{ 4, 3, 2, 1 });
    lw.list.reverse();
    verify_list(lw, std::array{ 1, 2, 3, 4 });
  }

  TEST(ilist, t_prepend)
  {
    list_wrapper src{ 1, 2, 3, 4 };
    list_wrapper dst{};
    dst.list.prepend(std::move(src.list));
    verify_list(dst, std::array{ 1, 2, 3, 4 });

    list_wrapper src2{ 5, 6, 7 };
    dst.list.prepend(std::move(src2.list));
    verify_list(dst, std::array{ 5, 6, 7, 1, 2, 3, 4 });
  }

  TEST(ilist, t_prepend_to)
  {
    list_wrapper src{ 5, 6, 7 };
    list_wrapper dst{ 1, 2, 3, 4 };
    auto&& pre = dst.list.front();
    dst.list.prepend_to(pre, std::move(src.list));
    verify_list(dst, std::array{ 5, 6, 7, 1, 2, 3, 4 });

    list_wrapper src2{ 8, 9 };
    dst.list.prepend_to(pre, std::move(src2.list));
    verify_list(dst, std::array{ 5, 6, 7, 8, 9, 1, 2, 3, 4 });
  }

  TEST(ilist, t_append)
  {
    list_wrapper src{ 1, 2, 3, 4 };
    list_wrapper dst{};
    dst.list.append(std::move(src.list));
    verify_list(dst, std::array{ 1, 2, 3, 4 });

    list_wrapper src2{ 5, 6, 7 };
    dst.list.append(std::move(src2.list));
    verify_list(dst, std::array{ 1, 2, 3, 4, 5, 6, 7 });
  }

  TEST(ilist, t_append_to)
  {
    list_wrapper src{ 5, 6, 7 };
    list_wrapper dst{ 1, 2, 3, 4 };
    auto&& app = dst.list.back();
    dst.list.append_to(app, std::move(src.list));
    verify_list(dst, std::array{ 1, 2, 3, 4, 5, 6, 7 });

    list_wrapper src2{ 8, 9 };
    dst.list.append_to(app, std::move(src2.list));
    verify_list(dst, std::array{ 1, 2, 3, 4, 8, 9, 5, 6, 7 });
  }

  TEST(ilist, t_find)
  {
    list_wrapper lw{ 1, 2, 3, 4, 5, 6 };
    auto toFind = lw.list.back().prev()->prev()->to_iterator(); // element 4
    auto found = lw.list.find([](auto&& item) noexcept
      {
        return item.value == 4;
      });

    ASSERT_EQ(toFind, found);
  }

  TEST(ilist, t_contains)
  {
    list_wrapper lw{ 1, 2, 3, 4, 5, 6 };
    auto found = lw.list.contains([](auto&& item) noexcept
      {
        return item.value == 6;
      });

    ASSERT_TRUE(found);

    found = lw.list.contains([](auto&& item) noexcept
      {
        return item.value == 10;
      });

    ASSERT_FALSE(found);
  }

  TEST(ilist, t_transform)
  {
    list_wrapper lw{ 1, 2, 3, 4, 5, 6 };
    lw.list.apply([](auto&& node) noexcept
      {
        auto&& v = node.value;
        if (v % 2 == 0)
          v *= 3;
      });
    verify_list(lw, std::array{ 1, 6, 3, 12, 5, 18 });

    lw.list.apply([](auto&& node) noexcept
      {
        if (node.value % 2 != 0)
          node.list().remove(node);
      });
    verify_list(lw, std::array{ 6, 12, 18 });
  }

  TEST(ilist, t_erase)
  {
    list_wrapper lw{ 1, 2, 3, 4, 5, 6 };
    lw.list.erase([](auto&& item) noexcept
      {
        return item.value % 3 == 0;
      });
    verify_list(lw, std::array{ 1, 2, 4, 5 });
  }

  TEST(ilist, t_filter)
  {
    list_wrapper lw{ 1, 2, 3, 4, 5, 6 };
    lw.list.filter([](auto&& item) noexcept
      {
        return item.value % 3 == 0;
      });
    verify_list(lw, std::array{ 3, 6 });
  }
}


namespace ut_tests
{
  TEST(ilist_view, t_fwd)
  {
    using utils::ilist_view;

    list_wrapper lw{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    auto&& list = lw.list;
    auto lv0 = list.to_view();

    ilist_view lv1{ list };
    EXPECT_EQ(lv0, lv1);
    verify_list(lv1, std::array{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 });

    ilist_view lv2{ list.begin(), list.end() };
    EXPECT_EQ(lv0, lv2);
    verify_list(lv2, std::array{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 });

    auto beg = std::next(list.begin(), 3); // element 3
    auto end = std::next(list.begin(), 6); // element 6
    lv2 = { beg, end };
    EXPECT_NE(lv0, lv2);
    verify_list(lv2, std::array{ 3, 4, 5 });

    ilist_view lv3{ beg, end, 4 };
    EXPECT_NE(lv0, lv3);
    verify_list(lv3, std::array{ 3, 4, 5, 6 });

    ilist_view lv4{ beg, 3 };
    EXPECT_NE(lv0, lv4);
    verify_list(lv4, std::array{ 3, 4, 5 });
 }

  TEST(ilist_view, t_rev)
  {
    using utils::ilist_view;

    list_wrapper lw{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    auto&& list = lw.list;
    auto lv0 = list.to_rev_view();

    ilist_view lv1{ list.rbegin(), list.rend() };
    EXPECT_EQ(lv0, lv1);
    verify_list(lv1, std::array{ 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 });

    auto beg = std::next(list.rbegin(), 3); // element 6
    auto end = std::next(list.rbegin(), 6); // element 3
    lv1 = { beg, end };
    EXPECT_NE(lv0, lv1);
    verify_list(lv1, std::array{ 6, 5, 4 });

    ilist_view lv2{ beg, end, 4 };
    EXPECT_NE(lv0, lv2);
    verify_list(lv2, std::array{ 6, 5, 4, 3 });

    ilist_view lv3{ beg, 3 };
    EXPECT_NE(lv0, lv3);
    verify_list(lv3, std::array{ 6, 5, 4 });
  }
}