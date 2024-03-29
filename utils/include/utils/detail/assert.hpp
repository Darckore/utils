#pragma once

namespace utils
{

  //
  // Assertion helper
  //
  inline bool assertion(bool cond, std::string_view condStr, ostream* ostream, src_loc loc = src_loc::current())
  {
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

