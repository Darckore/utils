#pragma once

namespace utils
{
  //
  // A stack implementation based on std vector
  //
  template <clonable T, typename Allocator = std::allocator<T>>
  class stack final
  {
  public:
    using container_type         = std::vector<T, Allocator>;
    using allocator_type         = container_type::allocator_type;
    using value_type             = container_type::value_type;
    using rvalue_reference       = value_type&&;
    using pointer                = container_type::pointer;
    using const_pointer          = container_type::const_pointer;
    using reference              = container_type::reference;
    using const_reference        = container_type::const_reference;
    using size_type              = container_type::size_type;
    using difference_type        = container_type::difference_type;
    using iterator               = container_type::iterator;
    using const_iterator         = container_type::const_iterator;
    using reverse_iterator       = container_type::reverse_iterator;
    using const_reverse_iterator = container_type::const_reverse_iterator;

  public:
    CLASS_SPECIALS_ALL(stack);
    CLASS_DEFAULT_DTOR(stack);
    CLASS_DEFAULT_EQ(stack);

    stack(size_type prealloc, const allocator_type& alloc = {}) noexcept :
      m_data{ alloc }
    {
      m_data.reserve(prealloc);
    }

  public:
    auto size() const noexcept
    {
      return m_data.size();
    }

    auto empty() const noexcept
    {
      return m_data.empty();
    }

    void push(const_reference item) noexcept
    {
      m_data.push_back(item);
    }
    void push(rvalue_reference item) noexcept
    {
      m_data.push_back(std::move(item));
    }

    void pop() noexcept
    {
      if(!empty())
        m_data.pop_back();
    }

    const_reference top() const noexcept
    {
      UTILS_ASSERT(!empty());
      return m_data.back();
    }
    reference top() noexcept
    {
      return FROM_CONST(top);
    }

    value_type extract() noexcept requires (default_constructible<value_type>)
    {
      SCOPE_GUARD(pop());
      return (!empty()) ? std::move(top()) : value_type{};
    }

    value_type extract(value_type def) noexcept
    {
      SCOPE_GUARD(pop());
      return (!empty()) ? std::move(top()) : std::move(def);
    }

    void walk_up(size_type count, callable<void, const_reference> auto&& proc) noexcept
    {
      if (count > m_data.size())
        return;

      auto beg = std::next(m_data.begin(), m_data.size() - count);
      for (auto it = beg; it < m_data.end(); ++it)
        proc(*it);

      while (count--)
        pop();
    }

  private:
    container_type m_data;
  };

}