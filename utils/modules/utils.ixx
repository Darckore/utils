module;

#include <utility>
#include <source_location>
#include <stacktrace>
#include <string_view>
#include <iostream>
#include <format>

export module utils;

export import :definitions;
export import :common;
export import :colour;
export import :clock;
export import :random;
export import :strings;
export import :math;
export import :ratio;

namespace utils
{
  using src_loc = std::source_location;
}

export namespace utils
{
  //
  // Assertion helper
  //
  bool assertion(bool cond, std::string_view condStr, std::ostream* ostream = &std::cerr, src_loc loc = src_loc::current())
  {
    using namespace std::literals;
    if (cond) return true;

    if (ostream)
    {
      constexpr auto fmt = "Assertion '{}' in {} failed at '{}':{}\n"sv;
      auto&& out = *ostream;
      auto outStr = std::format(fmt, condStr, loc.function_name(), loc.file_name(), loc.line());
      out << outStr;
      out << "Stack trace:\n" << std::stacktrace::current() << '\n';
    }

    return false;
  }
}

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