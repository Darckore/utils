#include "utils/utils.hpp"
using namespace utils;

TEST(strings, t_to_string_view)
{
  constexpr std::array arr{ 'l', 'o', 'l', ' ', 'k', 'e', 'k' };
  constexpr auto baseline = "lol kek"sv;
  auto res = detail::to_string_view(arr);

  EXPECT_EQ(baseline, res);
}

TEST(strings, t_split)
{
  constexpr auto str = "eeny#$%meeny#$%miny#$%moe"sv;
  auto splitRng = split(str, "#$%"sv);
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
  constexpr auto baseline = "lorem ipsum"sv;
  
  constexpr auto curlys    = "{{lorem ipsum}}"sv;

  constexpr auto ltrimmed  = ltrim(curlys, "{{"sv);
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