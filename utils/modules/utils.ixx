module;

#include <utility>

export module utils;

export import :definitions;
export import :common;
export import :colour;
export import :clock;
export import :random;
export import :strings;
export import :math;
export import :ratio;

export
{
  template <>
  struct std::hash<utils::hashed_string>
  {
    constexpr auto operator()(const utils::hashed_string& s) const noexcept
    {
      return *s;
    }
  };
}