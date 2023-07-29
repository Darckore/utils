#pragma once

namespace utils
{
  //
  // A simple clock
  //
  template <real T, typename Clock = chrono::steady_clock>
  class clock
  {
  public:
    using value_type   = T;
    using clock_t      = Clock;
    using time_point_t = typename clock_t::time_point;
    using duration_t   = chrono::duration<value_type>;

    CLASS_SPECIALS_NONE_CUSTOM(clock);
    clock() = default;
    ~clock() = default;

    auto operator()() noexcept
    {
      const duration_t diff = update();
      return diff.count();
    }

    auto operator()(value_type ceiling) noexcept
    {
      auto&& self = *this;
      const auto count = self();
      return count < ceiling ? count : ceiling;
    }

    template <typename Intr>
    auto diff_as() noexcept
    {
      return chrono::duration_cast<Intr>(update());
    }

    template <typename Intr>
    auto peek() const noexcept
    {
      return chrono::duration_cast<Intr>(cur_diff());
    }

    auto peek() const noexcept
    {
      const duration_t diff = cur_diff();
      return diff.count();
    }

  private:
    auto cur_diff() const noexcept
    {
      return clock_t::now() - m_mark;
    }

    auto update() noexcept
    {
      const auto prev = m_mark;
      m_mark = clock_t::now();
      return m_mark - prev;
    }

  private:
    time_point_t m_mark = clock_t::now();
  };

}