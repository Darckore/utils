#include "utils/utils.hpp"

namespace ut_tests
{
  TEST(comp_iter, t_same)
  {
    using utils::multiple_it;
    using vec = std::vector<int>;
    vec v1{ 1, 2, 3, 4, 5 };
    const vec v2{ 6, 7, 8, 9, 0 };

    multiple_it mi{ v1.begin(), v2.begin() };
    auto a1 = mi.get<0>();
    auto a2 = mi.get<1>();
    *a1 = 10;
    auto&& [aa1, aa2] = *mi;
    ++mi;
    auto b1 = mi.get<0>();
    auto b2 = mi.get<1>();
    utils::unused(a1, a2, b1, b2, aa1, aa2);


    using mitype = decltype(mi);
    constexpr auto tupsz = std::tuple_size_v<mitype>;
    utils::unused(tupsz);

    using tupel0 = std::tuple_element_t<0, mitype>;
    using tupel1 = std::tuple_element_t<1, mitype>;
  }
}