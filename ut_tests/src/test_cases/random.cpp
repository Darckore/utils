#include "utils/utils.hpp"

namespace ut_tests
{
  namespace
  {
    template <typename T>
    using series_cont = std::vector<T>;

    template <typename Rng>
    auto make_series(Rng& r, auto seriesSize)
    {
      using res_t = series_cont<typename Rng::value_type>;
      res_t seq;
      seq.reserve(seriesSize);

      for (auto idx = 0ull; idx < seriesSize; ++idx)
      {
        seq.emplace_back(r());
      }

      return seq;
    }

    template <typename T>
    bool check_series(T min, T max, const series_cont<T>& seq) noexcept
    {
      for (auto val : seq)
      {
        if (!utils::in_range(val, min, max))
          return false;
      }
      return true;
    }

    template <typename T>
    bool compare_series(const series_cont<T>& s1, const series_cont<T>& s2) noexcept
    {
      return std::equal(s1.begin(), s1.end(), s2.begin(), utils::eq<T>);
    }

    template <typename T>
    void test_default()
    {
      constexpr auto min = T{ 0 };
      constexpr auto max = T{ 99 };

      utils::rng r{ min, max };
      auto seq = make_series(r, 100ull);
      EXPECT_TRUE(check_series(min, max, seq));
    }

    template <typename T>
    void test_seed()
    {
      using utils::rng;

      constexpr auto min = T{ 0 };
      constexpr auto max = T{ 99 };
      constexpr auto seed = 1337;

      rng r1{ min, max };
      r1.seed(seed);
      rng r2{ min, max };
      r2.seed(seed);
      auto s1 = make_series(r1, 100ull);
      auto s2 = make_series(r2, 100ull);
      EXPECT_TRUE(compare_series(s1, s2));
    }

  }

  TEST(random, t_default_int)
  {
    test_default<int>();
  }

  TEST(random, t_default_float)
  {
    test_default<float>();
  }

  TEST(random, t_seed_int)
  {
    test_seed<int>();
  }

  TEST(random, t_seed_float)
  {
    test_seed<float>();
  }
}