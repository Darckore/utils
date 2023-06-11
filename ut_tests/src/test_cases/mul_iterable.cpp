#include "utils/utils.hpp"

namespace ut_tests
{
  TEST(comp_iter, t_same)
  {
    using utils::multiple_it;
    using vec = std::vector<int>;
    vec v1{ 1, 2, 3, 4, 5 };
    const vec v2{ 6, 7, 8, 9, 0 };

    multiple_it mi{ v1.begin() + 2, v2.begin() + 3 };
    auto a1 = mi.get<0>();
    *a1 = 10;
    auto&& [aa1, aa2] = *mi;
    utils::unused(a1, aa1, aa2);

    multiple_it me{ v1.begin(), v2.end() };
    auto eq = false;
    ++mi;
    eq = mi == me;
    ++mi;
    eq = mi == me;
    ++mi;
    eq = mi == me;
  }
}