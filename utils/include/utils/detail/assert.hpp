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
  void assertion(bool cond, std::string_view condStr, detail::ostream* ostream, detail::src_loc loc = detail::src_loc::current())
  {
    if (cond) return;

    if (ostream)
    {
      constexpr auto fmt = "Assertion '{}' in {} failed at '{}':{}\n"sv;
      auto&& out = *ostream;
      std::string outStr;
      std::vformat_to(std::back_inserter(outStr), fmt, 
                      std::make_format_args(condStr, loc.function_name(), loc.file_name(), loc.line()));
      out << outStr;
      out << "Stack trace:\n" << std::stacktrace::current() << '\n';
    }

    BREAK_ON(true);
  }
}

#ifndef NDEBUG
  #define UTILS_ASSERT(cond) utils::assertion(static_cast<bool>(cond), #cond, &std::cerr)
#else
  #define UTILS_ASSERT(cond)
#endif

