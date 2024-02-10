#pragma once

namespace utils
{
  template <typename T> class ilist;
  template <typename T> class ilist_iter;
  template <typename T> class ilist_fwd_iter;
  template <typename T> class ilist_rev_iter;

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

    using iterator               = ilist_fwd_iter<Derived>;
    using const_iterator         = ilist_fwd_iter<const Derived>;
    using reverse_iterator       = ilist_rev_iter<Derived>;
    using const_reverse_iterator = ilist_rev_iter<const Derived>;

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
    bool is_attached() const noexcept
    {
      return static_cast<bool>(m_list);
    }

    const list_type& list() const noexcept
    {
      UTILS_ASSERT(is_attached());
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

    iterator to_iterator() noexcept;
    const_iterator to_iterator() const noexcept;
    reverse_iterator to_reverse_iterator() noexcept;
    const_reverse_iterator to_reverse_iterator() const noexcept;

  private:
    void kill_prev() noexcept
    {
      dealloc(m_prev);
    }
    void kill_next() noexcept
    {
      dealloc(m_next);
    }

    bool same_as(const_pointer other) const noexcept
    {
      return to_derived() == other;
    }

    void reorder_with(reference other) noexcept
    {
      if (same_as(&other))
        return;

      auto self      = to_derived();
      auto myPrev    = m_prev;
      auto myNext    = m_next;
      auto otherPrev = other.prev();
      auto otherNext = other.next();

      if (myPrev) myPrev->m_next = &other;
      if (myNext) myNext->m_prev = &other;
      
      if (!other.same_as(myPrev)) other.m_prev = myPrev;
      else other.m_prev = self;

      if (!other.same_as(myNext)) other.m_next = myNext;
      else other.m_next = self;

      if (otherPrev) otherPrev->m_next = self;
      if (otherNext) otherNext->m_prev = self;
      
      if (!same_as(otherPrev)) m_prev = otherPrev;
      else m_prev = &other;

      if (!same_as(otherNext)) m_next = otherNext;
      else m_next = &other;
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
    const_pointer to_derived() const noexcept
    {
      return static_cast<const_pointer>(this);
    }
    pointer to_derived() noexcept
    {
      return FROM_CONST(to_derived);
    }

    static reference link(pointer l, reference node, pointer r) noexcept
    {
      node.m_prev = l;
      if (l) l->m_next = &node;
      node.m_next = r;
      if (r) r->m_prev = &node;
      return node;
    }

    template <typename ...Args> requires (std::constructible_from<value_type, list_type&, Args...>)
    static reference alloc(pointer l, pointer r, list_type& owner, Args&& ...args) noexcept
    {
      auto newVal = new value_type{ owner, std::forward<Args>(args)... };
      return link(l, *newVal, r);
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
    using size_type       = std::size_t;
    using difference_type = std::ptrdiff_t;

  public:
    CLASS_SPECIALS_ALL(ilist_iter);
    ~ilist_iter() noexcept = default;

    explicit ilist_iter(pointer node) noexcept :
      m_node{ node }
    {}

    explicit operator bool() const noexcept
    {
      return static_cast<bool>(m_node);
    }

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
      m_node = m_node->prev();
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
  // Node to iterator conversion
  //

  template <typename T>
  ilist_node<T>::iterator ilist_node<T>::to_iterator() noexcept
  {
    return iterator{ to_derived() };
  }

  template <typename T>
  ilist_node<T>::const_iterator ilist_node<T>::to_iterator() const noexcept
  {
    return const_iterator{ to_derived() };
  }

  template <typename T>
  ilist_node<T>::reverse_iterator ilist_node<T>::to_reverse_iterator() noexcept
  {
    return reverse_iterator{ to_derived() };
  }

  template <typename T>
  ilist_node<T>::const_reverse_iterator ilist_node<T>::to_reverse_iterator() const noexcept
  {
    return const_reverse_iterator{ to_derived() };
  }


  //
  // A doubly-linked intrusive list
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
    using size_type              = iterator::size_type;
    using difference_type        = iterator::difference_type;

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
      if (!m_head)
      {
        m_tail = {};
        return;
      }

      if (!m_tail) m_tail = m_head;
      assume_ownership(*m_head, *m_tail);
    }

  public:
    template <typename ...Args>
    reference emplace_front(Args&& ...args) noexcept
    {
      if (empty())
        return init(std::forward<Args>(args)...);

      auto&& newHead = m_head->add_before(std::forward<Args>(args)...);
      m_head = &newHead;
      ++m_size;
      return front();
    }

    template <typename ...Args>
    reference emplace_back(Args&& ...args) noexcept
    {
      if (empty())
        return init(std::forward<Args>(args)...);

      auto&& newTail = m_tail->add_after(std::forward<Args>(args)...);
      m_tail = &newTail;
      ++m_size;
      return back();
    }

    template <typename ...Args>
    reference emplace_before(reference node, Args&& ...args) noexcept
    {
      UTILS_ASSERT(this == &node.list());
      if (&node == m_head)
        return emplace_front(std::forward<Args>(args)...);

      ++m_size;
      return node.add_before(std::forward<Args>(args)...);
    }

    template <typename ...Args>
    reference emplace_before(iterator it, Args&& ...args) noexcept
    {
      if (!it)
        return emplace_back(std::forward<Args>(args)...);

      return emplace_before(*it, std::forward<Args>(args)...);
    }

    template <typename ...Args>
    reference emplace_before(reverse_iterator it, Args&& ...args) noexcept
    {
      if (!it)
        return emplace_front(std::forward<Args>(args)...);

      return emplace_after(*it, std::forward<Args>(args)...);
    }

    template <typename ...Args>
    reference emplace_after(reference node, Args&& ...args) noexcept
    {
      UTILS_ASSERT(this == &node.list());
      if (&node == m_tail)
        return emplace_back(std::forward<Args>(args)...);

      ++m_size;
      return node.add_after(std::forward<Args>(args)...);
    }

    template <typename ...Args>
    reference emplace_after(iterator it, Args&& ...args) noexcept
    {
      if (!it)
        return emplace_back(std::forward<Args>(args)...);

      return emplace_after(*it, std::forward<Args>(args)...);
    }

    template <typename ...Args>
    reference emplace_after(reverse_iterator it, Args&& ...args) noexcept
    {
      if (!it)
        return emplace_front(std::forward<Args>(args)...);

      return emplace_before(*it, std::forward<Args>(args)...);
    }

    ilist& attach_front(reference node) noexcept
    {
      if (node.is_attached())
      {
        UTILS_ASSERT(false);
        return *this;
      }

      assume_ownership(node);
      node_type::link({}, node, m_head);
      m_head = &node;
      if (!m_tail) m_tail = m_head;
      return *this;
    }
    ilist& attach_front(iterator it) noexcept
    {
      if (!it) return *this;
      return attach_front(*it);
    }
    ilist& attach_front(reverse_iterator it) noexcept
    {
      if (!it) return *this;
      return attach_front(*it);
    }

    ilist& attach_back(reference node) noexcept
    {
      if (node.is_attached())
      {
        UTILS_ASSERT(false);
        return *this;
      }

      assume_ownership(node);
      node_type::link(m_tail, node, {});
      m_tail = &node;
      if (!m_head) m_head = m_tail;
      return *this;
    }
    ilist& attach_back(iterator it) noexcept
    {
      if (!it) return *this;
      return attach_back(*it);
    }
    ilist& attach_back(reverse_iterator it) noexcept
    {
      if (!it) return *this;
      return attach_back(*it);
    }

    ilist& attach_before(reference node, reference attached) noexcept
    {
      UTILS_ASSERT(&node.list() == this);
      if (attached.is_attached())
      {
        UTILS_ASSERT(false);
        return *this;
      }

      assume_ownership(attached);
      node_type::link(node.prev(), attached, &node);
      if (m_head == &node) m_head = &attached;
      return *this;
    }

    ilist& attach_after(reference node, reference attached) noexcept
    {
      UTILS_ASSERT(&node.list() == this);
      if (attached.is_attached())
      {
        UTILS_ASSERT(false);
        return *this;
      }

      assume_ownership(attached);
      node_type::link(&node, attached, node.next());
      if (m_tail == &node) m_tail = &attached;
      return *this;
    }

    ilist& remove_before(reference node) noexcept
    {
      UTILS_ASSERT(&node.list() == this);

      if (node.prev() == m_head)
        m_head = &node;

      if (node.prev()) --m_size;
      node.kill_prev();
      return *this;
    }
    ilist& remove_before(iterator it) noexcept
    {
      if (!it) return *this;
      return remove_before(*it);
    }
    ilist& remove_before(reverse_iterator it) noexcept
    {
      if (!it) return *this;
      return remove_after(*it);
    }
    
    ilist& remove_after(reference node) noexcept
    {
      UTILS_ASSERT(&node.list() == this);

      if (node.next() == m_tail)
        m_tail = &node;

      if(node.next()) --m_size;
      node.kill_next();
      return *this;
    }
    ilist& remove_after(iterator it) noexcept
    {
      if (!it) return *this;
      return remove_after(*it);
    }
    ilist& remove_after(reverse_iterator it) noexcept
    {
      if (!it) return *this;
      return remove_before(*it);
    }

    ilist& detach(reference node) noexcept
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

      node.m_prev = {};
      node.m_next = {};
      node.m_list = {};
      --m_size;
      return *this;
    }
    ilist& detach(iterator it) noexcept
    {
      if (!it) return *this;
      return detach(*it);
    }
    ilist& detach(reverse_iterator it) noexcept
    {
      if (!it) return *this;
      return detach(*it);
    }

    ilist& remove(reference node) noexcept
    {
      detach(node);
      node_type::dealloc(&node);
      return *this;
    }
    ilist& remove(iterator it) noexcept
    {
      if (!it) return *this;
      return remove(*it);
    }
    ilist& remove(reverse_iterator it) noexcept
    {
      if (!it) return *this;
      return remove(*it);
    }

    ilist& pop_front() noexcept
    {
      if (empty())
        return *this;

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
      --m_size;
      return *this;
    }
    ilist& pop_back() noexcept
    {
      if (empty())
        return *this;

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
      --m_size;
      return *this;
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

    ilist& clear() noexcept
    {
      if (empty())
        return *this;

      while (!m_head->is_tail())
        m_head->kill_next();

      m_tail = {};
      node_type::dealloc(m_head);
      m_head = {};
      m_size = {};
      return *this;
    }

    ilist& reorder(reference l, reference r) noexcept
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
      return *this;
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

      auto newSize = size_type{};
      auto head = m_head;
      while (head)
      {
        ++newSize;
        head = head->next();
      }
      m_size = newSize;

      return { newHead, newTail };
    }

    ilist& reverse() noexcept
    {
      auto prevHead = m_head;
      auto prevTail = m_tail;

      auto cur = m_head;
      while (cur)
      {
        auto curNext = cur->next();
        auto curPrev = cur->prev();
        cur->m_prev = curNext;
        cur->m_next = curPrev;
        cur = curNext;
      }

      m_head = prevTail;
      m_tail = prevHead;
      return *this;
    }

  public:
    auto size() const noexcept
    {
      return m_size;
    }

    bool empty() const noexcept
    {
      return !m_size;
    }

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
      return const_reverse_iterator{ m_tail };
    }
    auto rend() const noexcept
    {
      return const_reverse_iterator{};
    }

    auto rbegin() noexcept
    {
      return reverse_iterator{ m_tail };
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
      ++m_size;
      return *m_head;
    }

    void assume_ownership(reference node) noexcept
    {
      ++m_size;
      node.m_list = this;
    }

    void assume_ownership(reference h, reference t) noexcept
    {
      auto first = &h;
      while (first && first != t.next())
      {
        assume_ownership(*first);
        first = first->next();
      }
    }

  private:
    pointer m_head{};
    pointer m_tail{};
    size_type m_size{};
  };

}