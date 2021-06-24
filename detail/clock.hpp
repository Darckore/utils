#pragma once

namespace utils
{

  //
  // A simple clock
  //
  template <detail::real T,
            typename Clock = chrono::steady_clock>
  class clock
  {
  public:
    using value_type = T;
    using clock_t = Clock;
    using time_point_t = typename clock_t::time_point;
    using duration_t = chrono::duration<value_type>;

    clock() = default;
    CLASS_SPECIALS_NONE_CUSTOM(clock);

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

  private:
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