#pragma once
#include <source_location>
#include <stacktrace>

namespace utils
{
  namespace detail
  {
    using src_loc = std::source_location;
    using ostream = std::ostream;
  }

  //
  // Assertion helper
  //
  inline bool assertion(bool cond, std::string_view condStr, detail::ostream* ostream, detail::src_loc loc = detail::src_loc::current())
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

#ifndef NDEBUG
  #define UTILS_ASSERT(cond) BREAK_ON(!utils::assertion(static_cast<bool>(cond), #cond, &std::cerr))
#else
  #define UTILS_ASSERT(cond)
#endif

