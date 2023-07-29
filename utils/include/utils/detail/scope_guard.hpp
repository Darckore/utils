#pragma once

namespace utils
{
  //
  // Takes a reference to a variable and (possibly) a new value
  // Resets the old value on scope exit
  //
  template <typename T>
  class value_guard final
  {
  public:
    using value_type = T;
    using reference = value_type&;

  public:
    CLASS_SPECIALS_NONE(value_guard);

    value_guard(reference var) noexcept :
      m_ref{ var },
      m_old{ var }
    {}

    value_guard(reference var, value_type newVal) noexcept :
      m_ref{ var },
      m_old{ std::move(var) }
    {
      var = std::move(newVal);
    }

    ~value_guard() noexcept
    {
      m_ref = std::move(m_old);
    }

  private:
    reference m_ref;
    value_type m_old;
  };


  namespace detail
  {
    template <typename Callable>
    concept scope_exit = std::is_nothrow_invocable_r_v<void, Callable>;
  }

  //
  // Performs an action at scope exit
  //
  template <detail::scope_exit Callable>
  class scope_terminator final
  {
  public:
    using func_t = Callable;

  public:
    CLASS_SPECIALS_NONE(scope_terminator);

    scope_terminator(func_t fn) noexcept :
      m_func{ std::move(fn) }
    {}

    ~scope_terminator() noexcept
    {
      m_func();
    }

  private:
    func_t m_func;
  };

}