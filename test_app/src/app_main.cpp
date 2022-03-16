#include "utils/utils.hpp"
#include <iostream>

int main()
{
  constexpr utils::vector vv{ 10, 20 };
  constexpr auto mul = vv * 10;
  constexpr auto div = vv / 10;
  return 0;
}