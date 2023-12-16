#pragma once

namespace utils
{
  //
  // A basic string pool
  // Caches strings and ensures they are safe to be referenced
  //
  class string_pool final
  {
  public:
    using str_t   = std::string_view;
    using key_t   = hashed_string;
    using buf_t   = std::string;
    using store_t = std::unordered_map<key_t, buf_t>;

  public:
    CLASS_SPECIALS_NONE_CUSTOM(string_pool);

    ~string_pool() noexcept = default;
    string_pool() noexcept = default;

  public:
    //
    // Interns a string
    //
    str_t intern(buf_t str) noexcept
    {
      m_buffer.reserve(str.length());
      m_buffer.append(str);
      return intern();
    }

    //
    // Formats data using the given format string, interns the result,
    // and returns a string view into it.
    // 
    // On format failure, an empty string view is returned
    //
    template <typename ...Args> requires (not_empty<Args...>)
    str_t format(str_t fmtString, Args&& ...args) noexcept
    {
      auto fmtArgs = std::make_format_args(std::forward<Args>(args)...);

      // We don't care about exceptions here. An empty resulting string means error
      try
      {
        std::vformat_to(std::back_inserter(m_buffer), fmtString, std::move(fmtArgs));
      }
      catch (...)
      {
        return {};
      }

      return intern();
    }

  private:
    //
    // Interns a string from the buffer
    //
    str_t intern() noexcept
    {
      if (m_buffer.empty())
        return {};

      auto key = key_t{ m_buffer };
      auto newIt = m_data.try_emplace(key, m_buffer);
      m_buffer.clear();
      return newIt.first->second;
    }

  private:
    buf_t m_buffer;
    store_t m_data;
  };
}