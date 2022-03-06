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
    ~clock() = default;
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

  //
  // Determines how the timer should be executed:
  // - automatic: starts immediately, restart is automatic
  //              until stop is called or the object is destroyed
  // - deferred:  need to call start to run, restart is automatic
  //              until stop is called or the object is destroyed
  // - manual:    need to call start to run and restart
  //
  enum class timer_policy : uint8_t
  {
    automatic,
    deferred,
    manual
  };

  //
  // A timer. Calls a function ashynchronously at the given interval
  //
  template <typename F>
  class timer
  {
  public:
    using interval_unit_t = chrono::milliseconds;
    using clock_t = clock<double>;
    using underlying_type = clock_t::value_type;
    using callback_t = F;
    using exec_t = std::future<void>;

    using enum timer_policy;

  public:
    CLASS_SPECIALS_NONE(timer);
    ~timer() noexcept
    {
      stop();
    }

    timer(timer_policy policy, interval_unit_t interval, callback_t callback) :
      m_interval{ interval },
      m_callback{ std::move(callback) },
      m_policy{ policy }
    {
      if (is_autostart())
        start();
    }

    timer(interval_unit_t interval, callback_t callback) :
      timer{ deferred, interval, std::move(callback) }
    {}

    void set_interval(interval_unit_t interval) noexcept
    {
      m_interval = interval;
    }
    void start()
    {
      stop();
      reset_clock();
      m_active = true;
      auto new_exec = std::async(std::launch::async,
        [this]
        {
          tick();
        });
      
      m_execution = std::move(new_exec);
    }
    void stop() 
    {
      m_active = false;
    }

    bool is_autostart() const noexcept
    {
      return m_policy == automatic;
    }
    bool is_deferred() const noexcept
    {
      return m_policy == deferred;
    }
    bool is_manual() const noexcept
    {
      return m_policy == manual;
    }

  private:
    void callback() noexcept(std::is_nothrow_invocable_v<callback_t>)
    {
      if constexpr (std::is_invocable_r_v<void, callback_t, decltype(*this)&>)
      {
        m_callback(*this);
      }
      else if constexpr (std::is_invocable_r_v<void, callback_t>)
      {
        m_callback();
      }
      else
      {
        static_assert(false, "Invalid callback signature");
      }
    }
    void mark()
    {
      callback();
      if (is_manual())
      {
        stop();
      }
      reset_clock();
    }
    void reset_clock()
    {
      m_clock();
    }
    void tick()
    {
      for (;;)
      {
        if (!m_active)
          return;

        if (m_clock.peek<interval_unit_t>() >= m_interval)
        {
          mark();
        }
      }
    }

  private:
    callback_t m_callback;
    interval_unit_t m_interval;
    exec_t m_execution;
    clock_t m_clock;
    std::atomic_bool m_active = false;
    timer_policy m_policy;
  };
}