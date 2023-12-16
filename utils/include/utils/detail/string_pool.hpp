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
    using str_t     = std::string_view;
    using key_t     = hashed_string;
    using buf_t     = std::string;
    using store_t   = std::unordered_map<key_t, buf_t>;
    using size_type = store_t::size_type;

  public:
    CLASS_SPECIALS_NONE_CUSTOM(string_pool);

    ~string_pool() noexcept = default;
    string_pool() noexcept = default;

  public:
    //
    // Interns a string
    //
    str_t intern(str_t str) noexcept
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

    //
    // Returns the number of interned items
    // This can be used as counter
    //
    size_type count() const noexcept
    {
      return m_data.size();
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


  //
  // A pool of string pools which allows auto-generating and interning strings
  // starting with the same prefix
  //
  class prefixed_pool final
  {
  public:
    using pool_t    = string_pool;
    using key_t     = pool_t::key_t;
    using str_t     = pool_t::str_t;
    using size_type = pool_t::size_type;

    using store_t = std::unordered_map<key_t, pool_t>;

  public:
    CLASS_SPECIALS_NONE_CUSTOM(prefixed_pool);

    ~prefixed_pool() noexcept = default;
    prefixed_pool() noexcept = default;

  public:
    //
    // Generates the next indexed value for the given prefix
    // e.g. the first ever call with 'meow' will give 'meow0', next - 'meow1', etc.
    //
    str_t next_indexed(str_t prefix) noexcept
    {
      auto&& pool = get_pool(prefix);
      const auto idx = pool.count();
      return pool.format("{}{}", prefix, idx);
    }

    //
    // Generates the a value for the given prefix according to the provided format
    // string and arguments. The format string must account for the prefix itself
    //
    template <typename ...Args>
    str_t format(str_t prefix, str_t fmt, Args&& ...args) noexcept
    {
      auto&& pool = get_pool(prefix);
      return pool.format(fmt, prefix, std::forward<Args>(args)...);
    }

  private:
    //
    // Returns a string pool for the given prefix
    //
    pool_t& get_pool(str_t prefix) noexcept
    {
      auto it = m_pools.try_emplace(prefix);
      return it.first->second;
    }

  private:
    store_t m_pools;
  };
}