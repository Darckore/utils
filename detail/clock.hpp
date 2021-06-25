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
    virtual ~clock() = default;
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
    auto peek() noexcept
    {
      return chrono::duration_cast<Intr>(clock_t::now() - m_mark);
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
  // Base class for the timer's callbacks
  //
  class I_timer
  {
  public:
    using interval_unit_t = chrono::milliseconds;

    virtual void set_interval(interval_unit_t interval) noexcept = 0;
    virtual void start() = 0;
    virtual void stop() = 0;

    virtual bool is_autostart() const noexcept = 0;
    virtual bool is_deferred() const noexcept = 0;
    virtual bool is_manual() const noexcept = 0;
  };

  //
  // A timer. Calls a function ashynchronously at the given interval
  //
  template <typename F>
    requires std::is_invocable_r_v<void, F, I_timer&>
  class timer : public I_timer, private clock<double>
  {
  public:
    using underlying_type = clock::value_type;
    using callback_t = F;

    using enum timer_policy;

  public:
    CLASS_SPECIALS_NONE(timer);
    virtual ~timer() noexcept
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

    virtual void set_interval(interval_unit_t interval) noexcept override
    {
      m_interval = interval;
    }
    virtual void start() override
    {
      stop();
      reset_clock();
      auto new_exec = std::async(std::launch::async, [this](std::future<void> abort_flag)
        {
          tick(std::move(abort_flag));
        }, std::move(m_stop.get_future()));
      m_execution = std::move(new_exec);
    }
    virtual void stop() override
    {
      if (m_execution.valid())
      {
        m_stop.set_value();
        m_stop = {};
        m_execution.get();
      }
    }

    virtual bool is_autostart() const noexcept override
    {
      return m_policy == automatic;
    }
    virtual bool is_deferred() const noexcept override
    {
      return m_policy == deferred;
    }
    virtual bool is_manual() const noexcept override
    {
      return m_policy == manual;
    }

  private:
    void mark()
    {
      m_callback(*this);
      if (is_manual())
      {
        stop();
      }
      reset_clock();
    }
    void reset_clock()
    {
      auto&& self = *this;
      unused(self());
    }
    void tick(std::future<void> abort_flag)
    {
      using namespace std::literals::chrono_literals;
      for (;;)
      {
        if (abort_flag.wait_for(0ms) == std::future_status::ready)
          break;

        const auto diff = peek<interval_unit_t>();
        if (diff >= m_interval)
        {
          mark();
        }
      }
    }

  private:
    interval_unit_t m_interval;
    std::promise<void> m_stop;
    std::future<void>  m_execution;
    callback_t m_callback;
    timer_policy m_policy;
  };
}