#pragma once

namespace utils
{
  template <typename T> class ilist;

  //
  // Intrusive list node
  // Used as a CRTP base class for anything stored in the list
  //
  template <typename Derived>
  class ilist_node
  {
  public:
    using value_type      = Derived;
    using base_type       = ilist_node<value_type>;
    using list_type       = ilist<value_type>;
    using pointer         = value_type*;
    using const_pointer   = const value_type*;
    using reference       = value_type&;
    using const_reference = const value_type&;

    friend list_type;

  public:
    CLASS_SPECIALS_NONE(ilist_node);

    ~ilist_node() noexcept
    {
      if (m_prev)
        m_prev->m_next = m_next;
      if (m_next)
        m_next->m_prev = m_prev;
    }

  protected:
    ilist_node(list_type& owner) noexcept :
      m_list{ &owner }
    {
      static_assert(std::derived_from<value_type, base_type>);
    }

  public:
    const list_type& list() const noexcept
    {
      return *m_list;
    }
    list_type& list() noexcept
    {
      return FROM_CONST(list);
    }

    const_pointer prev() const noexcept
    {
      return m_prev;
    }
    pointer prev() noexcept
    {
      return FROM_CONST(prev);
    }

    const_pointer next() const noexcept
    {
      return m_next;
    }
    pointer next() noexcept
    {
      return FROM_CONST(next);
    }

    bool is_head() const noexcept
    {
      return !prev();
    }
    bool is_tail() const noexcept
    {
      return !next();
    }

  protected:
    void kill_prev() noexcept
    {
      dealloc(m_prev);
    }
    void kill_next() noexcept
    {
      dealloc(m_next);
    }

    void reorder_with(reference other) noexcept
    {
      auto p = m_prev;
      auto n = m_next;
      auto op = other.prev();
      auto on = other.next();

      other.m_prev = p;
      if (p) p->m_next = &other;
      other.m_next = n;
      if (n) n->m_prev = &other;
      m_prev = op;
      if (op) op->m_next = to_derived();
      m_next = on;
      if (on) on->m_prev = to_derived();
    }

    template <typename ...Args>
    reference add_before(Args&& ...args) noexcept
    {
      return alloc(m_prev, to_derived(), list(), std::forward<Args>(args)...);
    }
    template <typename ...Args>
    reference add_after(Args&& ...args) noexcept
    {
      return alloc(to_derived(), m_next, list(), std::forward<Args>(args)...);
    }

  private:
    pointer to_derived() noexcept
    {
      return static_cast<pointer>(this);
    }

    template <typename ...Args> requires (std::constructible_from<value_type, list_type&, Args...>)
    static reference alloc(pointer l, pointer r, list_type& owner, Args&& ...args) noexcept
    {
      auto newVal = new value_type{ owner, std::forward<Args>(args)... };
      newVal->m_prev = l;
      if (l) l->m_next = newVal;
      newVal->m_next = r;
      if (r) r->m_prev = newVal;
      return *newVal;
    }

    static void dealloc(pointer ptr) noexcept
    {
      delete ptr;
    }

  private:
    list_type* m_list{};
    pointer m_prev{};
    pointer m_next{};
  };


  //
  // Base class for list iterators
  //
  template <typename T>
  class ilist_iter
  {
  public:
    using node_type       = ilist_node<std::remove_const_t<T>>;
    using value_type      = T;
    using pointer         = value_type*;
    using const_pointer   = const value_type*;
    using reference       = value_type&;
    using const_reference = const value_type&;

  public:
    CLASS_SPECIALS_ALL(ilist_iter);
    ~ilist_iter() noexcept = default;

    explicit ilist_iter(pointer node) noexcept :
      m_node{ node }
    {}

    constexpr bool operator==(const ilist_iter&) const noexcept = default;

    reference operator*() const noexcept
    {
      return *m_node;
    }

    pointer operator->() const noexcept
    {
      return m_node;
    }

    pointer get() const noexcept
    {
      return m_node;
    }

  protected:
    void next() noexcept
    {
      m_node = m_node->next();
    }
    void prev() noexcept
    {
      m_node = m_node->next();
    }

  private:
    pointer m_node{};
  };


  //
  // Forward iterator
  //
  template <typename T>
  class ilist_fwd_iter final : public ilist_iter<T>
  {
  public:
    using base_type = ilist_iter<T>;

  public:
    CLASS_SPECIALS_ALL(ilist_fwd_iter);
    ~ilist_fwd_iter() noexcept = default;

    explicit ilist_fwd_iter(base_type::pointer node) noexcept :
      base_type{ node }
    {}

    ilist_fwd_iter& operator++() noexcept
    {
      base_type::next();
      return *this;
    }
    ilist_fwd_iter operator++(int) noexcept
    {
      auto copy = *this;
      ++(*this);
      return copy;
    }

    ilist_fwd_iter& operator--() noexcept
    {
      base_type::prev();
      return *this;
    }
    ilist_fwd_iter operator--(int) noexcept
    {
      auto copy = *this;
      --(*this);
      return copy;
    }
  };


  //
  // Reverse iterator
  //
  template <typename T>
  class ilist_rev_iter final : public ilist_iter<T>
  {
  public:
    using base_type = ilist_iter<T>;

  public:
    CLASS_SPECIALS_ALL(ilist_rev_iter);
    ~ilist_rev_iter() noexcept = default;

    explicit ilist_rev_iter(base_type::pointer node) noexcept :
      base_type{ node }
    {}

    ilist_rev_iter& operator++() noexcept
    {
      base_type::prev();
      return *this;
    }
    ilist_rev_iter operator++(int) noexcept
    {
      auto copy = *this;
      ++(*this);
      return copy;
    }

    ilist_rev_iter& operator--() noexcept
    {
      base_type::next();
      return *this;
    }
    ilist_rev_iter operator--(int) noexcept
    {
      auto copy = *this;
      --(*this);
      return copy;
    }
  };


  //
  // An doubly-linked intrusive list
  // Allows elements to know their locations within the container
  // The elements combine pointer to adjacent nodes with data
  //
  template <typename T>
  class ilist final
  {
  public:
    using iterator               = ilist_fwd_iter<T>;
    using const_iterator         = ilist_fwd_iter<const T>;
    using reverse_iterator       = ilist_rev_iter<T>;
    using const_reverse_iterator = ilist_rev_iter<const T>;
    using node_type              = iterator::node_type;
    using value_type             = iterator::value_type;
    using pointer                = iterator::pointer;
    using const_pointer          = iterator::const_pointer;
    using reference              = iterator::reference;
    using const_reference        = iterator::const_reference;

  public:
    CLASS_SPECIALS_NONE_CUSTOM(ilist);

    ~ilist() noexcept
    {
      clear();
    }

    ilist() noexcept = default;

  private:
    ilist(pointer h, pointer t) noexcept :
      m_head{ h },
      m_tail{ t }
    {
      if (!m_tail) m_tail = m_head;

      auto first = m_head;
      while (first)
      {
        first->m_list = this;
        first = first->next();
      }
    }

  public:
    template <typename ...Args>
    reference emplace_front(Args&& ...args) noexcept
    {
      if (empty())
        return init(std::forward<Args>(args)...);

      auto&& newHead = m_head->add_before(std::forward<Args>(args)...);
      m_head = &newHead;
      return front();
    }

    template <typename ...Args>
    reference emplace_back(Args&& ...args) noexcept
    {
      if (empty())
        return init(std::forward<Args>(args)...);

      auto&& newTail = m_tail->add_after(std::forward<Args>(args)...);
      m_tail = &newTail;
      return back();
    }

    template <typename ...Args>
    reference emplace_before(reference node, Args&& ...args) noexcept
    {
      UTILS_ASSERT(this == &node.list());
      if (&node == m_head)
        return emplace_front(std::forward<Args>(args)...);

      return node.add_before(std::forward<Args>(args)...);
    }

    template <typename ...Args>
    reference emplace_after(reference node, Args&& ...args) noexcept
    {
      UTILS_ASSERT(this == &node.list());
      if (&node == m_tail)
        return emplace_back(std::forward<Args>(args)...);

      return node.add_after(std::forward<Args>(args)...);
    }

    void remove_before(reference node) noexcept
    {
      UTILS_ASSERT(&node.list() == this);

      if (node.prev() == m_head)
        m_head = &node;

      node.kill_prev();
    }
    void remove_after(reference node) noexcept
    {
      UTILS_ASSERT(&node.list() == this);

      if (node.next() == m_tail)
        m_tail = &node;

      node.kill_next();
    }
    void remove(reference node) noexcept
    {
      UTILS_ASSERT(&node.list() == this);
      auto p = node.prev();
      auto n = node.next();
      if (p) p->m_next = n;
      if (n) n->m_prev = p;

      if (&node == m_head)
        m_head = n;
      if (&node == m_tail)
        m_tail = p;

      node_type::dealloc(&node);
    }

    void reorder(reference l, reference r) noexcept
    {
      UTILS_ASSERT(&l.list() == this);
      UTILS_ASSERT(&r.list() == this);
      l.reorder_with(r);
      auto newHead = m_head;
      auto newTail = m_tail;

      if (&l == m_head)
        newHead = &r;
      if (&l == m_tail)
        newTail = &r;
      if (&r == m_head)
        newHead = &l;
      if (&r == m_tail)
        newTail = &l;

      m_head = newHead;
      m_tail = newTail;
    }

    void pop_front() noexcept
    {
      if (empty())
        return;

      auto head = m_head;
      m_head = head->next();

      if (!empty())
      {
        m_head->kill_prev();
      }
      else
      {
        node_type::dealloc(head);
        m_tail = {};
      }
    }
    void pop_back() noexcept
    {
      if (empty())
        return;

      auto tail = m_tail;
      m_tail = tail->prev();

      if (m_tail)
      {
        m_tail->kill_next();
      }
      else
      {
        node_type::dealloc(tail);
        m_head = {};
      }
    }

    ilist split_at(reference from) noexcept
    {
      UTILS_ASSERT(&from.list() == this);
      auto newHead = &from;
      auto newTail = m_tail;

      auto fp = from.m_prev;
      from.m_prev = {};

      if (fp)
        fp->m_next = {};

      if (&from == m_head)
      {
        m_head = {};
        m_tail = {};
      }
      if (&from == m_tail)
      {
        m_tail = fp;
      }
      if (!m_tail)
        m_tail = m_head;

      return { newHead, newTail };
    }

    const_reference front() const noexcept
    {
      return *m_head;
    }
    reference front() noexcept
    {
      return FROM_CONST(front);
    }

    const_reference back() const noexcept
    {
      return *m_tail;
    }
    reference back() noexcept
    {
      return FROM_CONST(back);
    }

    bool empty() const noexcept
    {
      return !m_head;
    }

    void clear() noexcept
    {
      if (empty())
        return;

      while (!m_head->is_tail())
        m_head->kill_next();

      m_tail = {};
      node_type::dealloc(m_head);
      m_head = {};
    }

  public:
    auto begin() const noexcept
    {
      return const_iterator{ m_head };
    }
    auto end() const noexcept
    {
      return const_iterator{};
    }

    auto begin() noexcept
    {
      return iterator{ m_head };
    }
    auto end() noexcept
    {
      return iterator{};
    }

    auto rbegin() const noexcept
    {
      return const_reverse_iterator{ m_head };
    }
    auto rend() const noexcept
    {
      return const_reverse_iterator{};
    }

    auto rbegin() noexcept
    {
      return reverse_iterator{ m_head };
    }
    auto rend() noexcept
    {
      return reverse_iterator{};
    }

  private:
    template <typename ...Args>
    reference init(Args&& ...args) noexcept
    {
      m_head = &node_type::alloc(m_head, m_tail, *this, std::forward<Args>(args)...);
      m_tail = m_head;
      return *m_head;
    }

  private:
    pointer m_head{};
    pointer m_tail{};
  };

}