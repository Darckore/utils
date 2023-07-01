#pragma once

namespace utils
{
  //
  // Specifies the system handler target
  //
  enum class handler_for : std::uint8_t
  {
    terminate,
    allocation
  };

  namespace detail
  {
    using handler_type = void(*)();

    template <handler_for>
    auto set_handler(handler_type) noexcept;

    template <>
    inline auto set_handler<handler_for::terminate>(handler_type handler) noexcept
    {
      auto prev = std::set_terminate(handler);
      return prev == &std::abort ? nullptr : prev;
    }

    template <>
    inline auto set_handler<handler_for::allocation>(handler_type handler) noexcept
    {
      return std::set_new_handler(handler);
    }

    template <typename F>
    concept system_handler = std::is_invocable_r_v<void, F>;

    struct sys_handler_base {};

    template <typename T>
    concept handler_wrapper = std::derived_from<T, sys_handler_base>;

    template <handler_wrapper Wrapper>
    inline std::stack<Wrapper>& get_handler_stack() noexcept;
  }

  //
  // Sets a system handler and maintains the chain of the previously set ones
  // When the handler is called, executes the chain
  //
  template <handler_for Target>
  class sys_handler final : public detail::sys_handler_base
  {
  public:
    static constexpr auto target = Target;

    using self_type     = sys_handler<target>;
    using handler_type  = std::function<void()>;

  private:
    static decltype(auto) get_stack() noexcept
    {
      return detail::get_handler_stack<self_type>();
    }

    [[noreturn]] static void handle() noexcept
    {
      auto&& stack = get_stack();
      auto cur = std::move(stack.top());
      stack.pop();
      cur.on_handler();
    }

  public:
    CLASS_SPECIALS_NODEFAULT_NOCOPY(sys_handler);

    template <detail::system_handler Handler>
    static void make_handler(int exitCode, Handler&& handler) noexcept
    {
      get_stack().push(self_type{ exitCode, std::forward<Handler>(handler) });
    }

  private:
    template <detail::system_handler Handler>
    sys_handler(int exitCode, Handler&& handler) noexcept :
      m_handler{ std::forward<Handler>(handler) },
      m_exitCode{ exitCode }
    {
      m_prev = detail::set_handler<target>(handle);
    }

    [[noreturn]] void on_handler() noexcept
    {
      if (m_handler(); m_prev)
        m_prev();

      if constexpr (target == handler_for::terminate)
        std::exit(m_exitCode);
      else
        std::terminate();
    }

  private:
    handler_type m_handler{};
    handler_type m_prev{};
    int m_exitCode{};
  };

  namespace detail
  {
    template <handler_wrapper Wrapper>
    inline std::stack<Wrapper>& get_handler_stack() noexcept
    {
      static std::stack<Wrapper> stack;
      return stack;
    }
  }

  template <detail::system_handler Handler>
  inline void set_terminate(int exitCode, Handler&& handler) noexcept
  {
    sys_handler<handler_for::terminate>::
      make_handler(exitCode, std::forward<Handler>(handler));
  }

  template <detail::system_handler Handler>
  inline void set_new(Handler&& handler) noexcept
  {
    sys_handler<handler_for::allocation>::
      make_handler(0, std::forward<Handler>(handler));
  }
}