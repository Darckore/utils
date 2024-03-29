#include "utils/utils.hpp"

namespace ut_tests
{
  using namespace std::literals;

  TEST(strings, t_split)
  {
    constexpr auto str = "eeny#$%meeny#$%miny#$%moe"sv;
    auto splitRng = utils::split(str, "#$%"sv);
    constexpr std::array baseline{ "eeny"sv, "meeny"sv, "miny"sv, "moe"sv };
    using sz_t = decltype(baseline)::size_type;

    sz_t idx = 0;
    for (auto item : splitRng)
    {
      EXPECT_EQ(item, baseline[idx++]);
    }

    EXPECT_EQ(idx, baseline.size());
  }

  TEST(strings, t_trim)
  {
    using utils::trim;
    using utils::ltrim;
    using utils::rtrim;

    constexpr auto baseline = "lorem ipsum"sv;

    constexpr auto curlys = "{{lorem ipsum}}"sv;

    constexpr auto ltrimmed = ltrim(curlys, "{{"sv);
    EXPECT_TRUE(ltrimmed.starts_with(baseline));
    EXPECT_TRUE(ltrimmed.ends_with("}}"sv));

    constexpr auto rtrimmed = rtrim(ltrimmed, "}}"sv);
    EXPECT_EQ(baseline, rtrimmed);

    constexpr auto trimmed_from_list = trim(curlys, '{', '}', '#', '@');
    EXPECT_EQ(baseline, trimmed_from_list);

    constexpr auto ltrimmedFromList = ltrim(curlys, '{', '}', '#', '@');
    EXPECT_EQ(ltrimmed, ltrimmedFromList);

    constexpr auto rtrimmedFromList = rtrim(curlys, '{', '}', '#', '@');
    EXPECT_TRUE(rtrimmedFromList.ends_with(baseline));
    EXPECT_TRUE(rtrimmedFromList.starts_with("{{"sv));

    constexpr auto eqs = "==lorem ipsum==";

    constexpr auto eqtrimmed = trim(eqs, "=="sv);
    EXPECT_EQ(baseline, eqtrimmed);

    constexpr auto eqtrimmedFromList = trim(eqs, '=', 'a', '$');
    EXPECT_EQ(baseline, eqtrimmedFromList);
  }

  TEST(strings, t_trim_spaces)
  {
    using utils::trim;
    using utils::ltrim;
    using utils::rtrim;

    constexpr auto str = " \r\n\rhi there\tm8\v\f"sv;

    EXPECT_TRUE(ltrim(str).starts_with("hi there"));
    EXPECT_TRUE(rtrim(str).ends_with("\tm8"));
    EXPECT_EQ(trim(str), "hi there\tm8"sv);
  }

  TEST(strings, t_hashed_str)
  {
    using utils::hashed_string;
    constexpr auto str = "I am a string to be hashed"sv;
    const auto hstr = hashed_string{ str };

    const auto shash = std::hash<hashed_string>{}(hstr);
    ASSERT_EQ(shash, *hstr);
  }

  TEST(strings, t_read_file)
  {
    using utils::read_file;
    static constexpr auto okValue = "this file has no purpose in life"sv;
    auto existing = "data/test_file.txt"sv;
    auto wrong    = "somewhere/test_file"sv;

    auto bad = read_file(wrong);
    EXPECT_FALSE(bad.has_value());

    auto good = read_file(existing);
    ASSERT_TRUE(good.has_value());
    auto contents = utils::trim(std::string_view{ *good });
    EXPECT_EQ(okValue, contents);
  }

  TEST(strings, t_pool_same)
  {
    utils::string_pool pool;
    auto s1 = "hello world"sv;
    auto s2 = "hello "s + "world"s;
    auto intr1 = pool.intern(s1).data();
    auto intr2 = pool.intern(s2).data();

    EXPECT_EQ(pool.count(), 1ull);
    ASSERT_EQ(intr1, intr2);
  }

  TEST(strings, t_pool_diff)
  {
    utils::string_pool pool;
    auto s1 = "hello world"sv;
    auto s2 = "goodbye world"sv;
    auto intr1 = pool.intern(s1).data();
    auto intr2 = pool.intern(s2).data();

    EXPECT_EQ(pool.count(), 2ull);
    ASSERT_NE(intr1, intr2);
  }

  TEST(strings, t_pool_fmt)
  {
    utils::string_pool pool;
    auto s   = "hello world 111"sv;
    auto fmt = "{} {} {}"sv;
    auto intr1 = pool.intern(s).data();
    auto intr2 = pool.format(fmt, "hello"sv, "world"sv, 111).data();

    EXPECT_EQ(pool.count(), 1ull);
    ASSERT_EQ(intr1, intr2);
  }

  static constexpr auto lol = "lol."sv;
  static constexpr auto kek = "kek."sv;

  TEST(strings, t_pref_pool)
  {
    utils::prefixed_pool pp;
    auto l0 = pp.next_indexed(lol);
    auto l1 = pp.next_indexed(lol);
    EXPECT_EQ(l0, "lol.0"s);
    EXPECT_EQ(l1, "lol.1"s);

    auto k0 = pp.next_indexed(kek);
    auto k1 = pp.next_indexed(kek);
    EXPECT_EQ(k0, "kek.0"sv);
    EXPECT_EQ(k1, "kek.1"sv);
  }

  TEST(strings, t_pref_reset_all)
  {
    utils::prefixed_pool pp;
    auto l0 = pp.next_indexed(lol);
    auto k0 = pp.next_indexed(kek);
    EXPECT_EQ(l0, "lol.0"sv);
    EXPECT_EQ(k0, "kek.0"sv);

    pp.reset();
    auto l1 = pp.next_indexed(lol);
    auto k1 = pp.next_indexed(kek);
    EXPECT_EQ(l1, "lol.0"sv);
    EXPECT_EQ(k1, "kek.0"sv);

    auto l0d = l0.data();
    auto l1d = l1.data();
    auto k0d = k0.data();
    auto k1d = k1.data();
    ASSERT_EQ(l0d, l1d);
    ASSERT_EQ(k0d, k1d);
  }

  TEST(strings, t_pref_reset_one)
  {
    utils::prefixed_pool pp;
    auto l0 = pp.next_indexed(lol);
    auto k0 = pp.next_indexed(kek);
    EXPECT_EQ(l0, "lol.0"sv);
    EXPECT_EQ(k0, "kek.0"sv);

    pp.reset(lol);
    auto l1 = pp.next_indexed(lol);
    auto k1 = pp.next_indexed(kek);
    EXPECT_EQ(l1, "lol.0"sv);
    EXPECT_EQ(k1, "kek.1"sv);

    auto l0d = l0.data();
    auto l1d = l1.data();
    auto k0d = k0.data();
    auto k1d = k1.data();
    ASSERT_EQ(l0d, l1d);
    ASSERT_NE(k0d, k1d);
  }

  TEST(strings, t_pref_restore)
  {
    utils::prefixed_pool pp;
    auto l0 = pp.next_indexed(lol);
    auto l1 = pp.next_indexed(lol);
    (void)pp.next_indexed(kek);
    (void)pp.next_indexed(kek);
    (void)pp.next_indexed(kek);
    (void)pp.next_indexed(kek);

    auto cache = pp.cache_and_reset();
    EXPECT_EQ(cache.size(), 2ull);

    auto ll0 = pp.next_indexed(lol);
    EXPECT_EQ(ll0, l0);

    auto kk = pp.next_indexed(kek);
    EXPECT_EQ(kk, "kek.0"sv);

    pp.restore(std::move(cache));
    auto l2 = pp.next_indexed(lol);
    EXPECT_EQ(l2, "lol.2"sv);
    
    auto k4 = pp.next_indexed(kek);
    EXPECT_EQ(k4, "kek.4"sv);
  }
}