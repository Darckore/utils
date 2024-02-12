#pragma once

namespace utils
{
  template <typename T, typename Allocator> class ilist;
  template <typename T, typename Allocator> class ilist_iter;
  template <typename T, typename Allocator> class ilist_fwd_iter;
  template <typename T, typename Allocator> class ilist_rev_iter;

  //
  // Intrusive list node
  // Used as a CRTP base class for anything stored in the list
  //
  template <typename Derived, typename Allocator = std::allocator<Derived>>
  class ilist_node
  {
  public:
    using value_type      = Derived;
    using allocator_type  = Allocator;
    using base_type       = ilist_node<value_type, allocator_type>;
    using list_type       = ilist<value_type, allocator_type>;
    using pointer         = value_type*;
    using const_pointer   = const value_type*;
    using reference       = value_type&;
    using const_reference = const value_type&;

    using iterator               = ilist_fwd_iter<Derived, allocator_type>;
    using const_iterator         = ilist_fwd_iter<const Derived, allocator_type>;
    using reverse_iterator       = ilist_rev_iter<Derived, allocator_type>;
    using const_reverse_iterator = ilist_rev_iter<const Derived, allocator_type>;

    friend list_type;

  public:
    CLASS_SPECIALS_NONE(ilist_node);

    ~ilist_node() noexcept
    {
      mutual_link(prev(), next());
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

    bool same_as(const_pointer other) const noexcept
    {
      return to_derived() == other;
    }

    template <typename First, typename ...Args> requires (all_convertible<const_pointer, Args...>)
    bool same_as_any(First first, Args ...args) const noexcept
    {
      return (same_as(first) && ... && same_as(args));
    }

    template <typename First, typename ...Args> requires (all_convertible<const_pointer, Args...>)
    bool same_as_none(First first, Args ...args) const noexcept
    {
      return !same_as_any(first, args...);
    }

    bool belongs_to(const list_type* list) const noexcept
    {
      return m_list == list;
    }
    bool belongs_to(const list_type& list) const noexcept
    {
      return belongs_to(&list);
    }

    iterator to_iterator() noexcept;
    const_iterator to_iterator() const noexcept;
    reverse_iterator to_reverse_iterator() noexcept;
    const_reverse_iterator to_reverse_iterator() const noexcept;

  private:
    void kill_prev(allocator_type alloc) noexcept
    {
      dealloc(alloc, prev());
    }
    void kill_next(allocator_type alloc) noexcept
    {
      dealloc(alloc, next());
    }

    void reorder_with(reference other) noexcept
    {
      if (same_as(&other))
        return;

      auto self      = to_derived();
      auto myPrev    = prev();
      auto myNext    = next();
      auto otherPrev = other.prev();
      auto otherNext = other.next();

      set_next(myPrev, &other);
      set_prev(myNext, &other);
      if (!other.same_as(myPrev)) other.set_prev(myPrev);
      else other.set_prev(self);
      if (!other.same_as(myNext)) other.set_next(myNext);
      else other.set_next(self);

      set_next(otherPrev, self);
      set_prev(otherNext, self);
      if (!same_as(otherPrev)) set_prev(otherPrev);
      else set_prev(&other);
      if (!same_as(otherNext)) set_next(otherNext);
      else set_next(&other);
    }

    template <typename ...Args>
    reference add_before(allocator_type alloc, Args&& ...args) noexcept
    {
      return make(alloc, prev(), to_derived(), list(), std::forward<Args>(args)...);
    }
    template <typename ...Args>
    reference add_after(allocator_type alloc, Args&& ...args) noexcept
    {
      return make(alloc, to_derived(), next(), list(), std::forward<Args>(args)...);
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

    void drop_next() noexcept
    {
      set_next({});
    }
    void drop_prev() noexcept
    {
      set_prev({});
    }
    void drop_list() noexcept
    {
      m_list = {};
    }
    void detach() noexcept
    {
      drop_prev();
      drop_next();
      drop_list();
    }

    void set_next(pointer n) noexcept
    {
      m_next = n;
    }
    void set_prev(pointer p) noexcept
    {
      m_prev = p;
    }
    void attach(list_type& list) noexcept
    {
      m_list = &list;
    }

    static reference link(pointer l, reference node, pointer r) noexcept
    {
      mutual_link(l, &node);
      mutual_link(&node, r);
      return node;
    }

    template <typename ...Args> requires (std::constructible_from<value_type, list_type&, Args...>)
    static reference make(allocator_type alloc, pointer l, pointer r, list_type& owner, Args&& ...args) noexcept
    {
      auto storage = alloc.allocate(1);
      auto newVal = new (storage) value_type{ owner, std::forward<Args>(args)... };
      return link(l, *newVal, r);
    }

    static void dealloc(allocator_type alloc, pointer ptr) noexcept
    {
      if (!ptr) return;

      ptr->~value_type();
      alloc.deallocate(ptr, 1);
    }

    static void set_next(pointer node, pointer p) noexcept
    {
      if (node) node->set_next(p);
    }
    static void set_prev(pointer node, pointer p) noexcept
    {
      if (node) node->set_prev(p);
    }
    static void mutual_link(pointer l, pointer r) noexcept
    {
      set_next(l, r);
      set_prev(r, l);
    }

  private:
    list_type* m_list{};
    pointer m_prev{};
    pointer m_next{};
  };


  //
  // Base class for list iterators
  //
  template <typename T, typename Allocator = std::allocator<T>>
  class ilist_iter
  {
  public:
    using node_type       = ilist_node<std::remove_const_t<T>, Allocator>;
    using allocator_type  = node_type::allocator_type;
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
  template <typename T, typename Allocator = std::allocator<T>>
  class ilist_fwd_iter final : public ilist_iter<T, Allocator>
  {
  public:
    using base_type = ilist_iter<T, Allocator>;

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
  template <typename T, typename Allocator = std::allocator<T>>
  class ilist_rev_iter final : public ilist_iter<T>
  {
  public:
    using base_type = ilist_iter<T, Allocator>;

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

  template <typename T, typename Allocator>
  ilist_node<T, Allocator>::iterator ilist_node<T, Allocator>::to_iterator() noexcept
  {
    return iterator{ to_derived() };
  }

  template <typename T, typename Allocator>
  ilist_node<T, Allocator>::const_iterator ilist_node<T, Allocator>::to_iterator() const noexcept
  {
    return const_iterator{ to_derived() };
  }

  template <typename T, typename Allocator>
  ilist_node<T, Allocator>::reverse_iterator ilist_node<T, Allocator>::to_reverse_iterator() noexcept
  {
    return reverse_iterator{ to_derived() };
  }

  template <typename T, typename Allocator>
  ilist_node<T, Allocator>::const_reverse_iterator ilist_node<T, Allocator>::to_reverse_iterator() const noexcept
  {
    return const_reverse_iterator{ to_derived() };
  }

  template <typename F, typename Node>
  concept ilist_unary_predicate =
    std::is_nothrow_invocable_r_v<bool, F, const Node&>;

  template <typename F, typename Node>
  concept ilist_binary_predicate =
    std::is_nothrow_invocable_r_v<bool, F, const Node&, const Node&>;

  template <typename F, typename Node>
  concept ilist_unary_transform =
    std::is_nothrow_invocable_r_v<void, F, Node&>;


  //
  // A doubly-linked intrusive list
  // Allows elements to know their locations within the container
  // The elements combine pointer to adjacent nodes with data
  //
  template <typename T, typename Allocator = std::allocator<T>>
  class ilist final
  {
  public:
    using iterator               = ilist_fwd_iter<T, Allocator>;
    using const_iterator         = ilist_fwd_iter<const T, Allocator>;
    using reverse_iterator       = ilist_rev_iter<T, Allocator>;
    using const_reverse_iterator = ilist_rev_iter<const T, Allocator>;
    using allocator_type         = iterator::allocator_type;
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

    explicit ilist(const allocator_type& alloc) noexcept :
      m_alloc{ alloc }
    {}

  private:
    ilist(pointer h, pointer t) noexcept :
      m_head{ h },
      m_tail{ t }
    {
      if (!m_head)
      {
        reset_tail();
        return;
      }

      if (!m_tail) m_tail = m_head;
      assume_ownership(*m_head, *m_tail);
    }

  public: // addition and removal of elements
    template <typename ...Args>
    reference emplace_front(Args&& ...args) noexcept
    {
      if (empty())
        return init(std::forward<Args>(args)...);

      auto&& newHead = m_head->add_before(allocator(), std::forward<Args>(args)...);
      set_head(&newHead);
      grow();
      return front();
    }

    template <typename ...Args>
    reference emplace_back(Args&& ...args) noexcept
    {
      if (empty())
        return init(std::forward<Args>(args)...);

      auto&& newTail = m_tail->add_after(allocator(), std::forward<Args>(args)...);
      set_tail(&newTail);
      grow();
      return back();
    }

    template <typename ...Args>
    reference emplace_before(reference node, Args&& ...args) noexcept
    {
      UTILS_ASSERT(node.belongs_to(this));
      if (node.same_as(m_head))
        return emplace_front(std::forward<Args>(args)...);

      grow();
      return node.add_before(allocator(), std::forward<Args>(args)...);
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
      UTILS_ASSERT(node.belongs_to(this));
      if (node.same_as(m_tail))
        return emplace_back(std::forward<Args>(args)...);

      grow();
      return node.add_after(allocator(), std::forward<Args>(args)...);
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
      set_head(&node);
      return *this;
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
      set_tail(&node);
      return *this;
    }

    ilist& attach_before(reference node, reference attached) noexcept
    {
      if (!node.belongs_to(this) || attached.is_attached())
      {
        UTILS_ASSERT(false);
        return *this;
      }

      assume_ownership(attached);
      node_type::link(node.prev(), attached, &node);
      if (node.same_as(m_head)) set_head(&attached);
      return *this;
    }
    ilist& attach_before(iterator it, reference attached) noexcept
    {
      if (!it)
      {
        UTILS_ASSERT(false);
        return *this;
      }

      return attach_before(*it, attached);
    }
    ilist& attach_before(reverse_iterator it, reference attached) noexcept
    {
      if (!it)
      {
        UTILS_ASSERT(false);
        return *this;
      }

      return attach_after(*it, attached);
    }

    ilist& attach_after(reference node, reference attached) noexcept
    {
      if (!node.belongs_to(this) || attached.is_attached())
      {
        UTILS_ASSERT(false);
        return *this;
      }

      assume_ownership(attached);
      node_type::link(&node, attached, node.next());
      if (node.same_as(m_tail)) set_tail(&attached);
      return *this;
    }
    ilist& attach_after(iterator it, reference attached) noexcept
    {
      if (!it)
      {
        UTILS_ASSERT(false);
        return *this;
      }

      return attach_after(*it, attached);
    }
    ilist& attach_after(reverse_iterator it, reference attached) noexcept
    {
      if (!it)
      {
        UTILS_ASSERT(false);
        return *this;
      }

      return attach_before(*it, attached);
    }

    ilist& remove_before(reference node) noexcept
    {
      if (!node.belongs_to(this))
      {
        UTILS_ASSERT(false);
        return *this;
      }

      if (node.prev() == m_head)
        set_head(&node);

      if (node.prev()) shrink();
      node.kill_prev(allocator());
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
      if (!node.belongs_to(this))
      {
        UTILS_ASSERT(false);
        return *this;
      }

      if (node.next() == m_tail)
        set_tail(&node);

      if(node.next()) shrink();
      node.kill_next(allocator());
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
      if (!node.belongs_to(this))
      {
        UTILS_ASSERT(false);
        return *this;
      }

      auto p = node.prev();
      auto n = node.next();
      node_type::mutual_link(p, n);

      if (node.same_as(m_head)) set_head(n);
      if (node.same_as(m_tail)) set_tail(p);

      node.detach();
      shrink();
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
      node_type::dealloc(allocator(), &node);
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
      set_head(head->next());

      if (m_head)
      {
        m_head->kill_prev(allocator());
      }
      else
      {
        node_type::dealloc(allocator(), head);
        reset_tail();
      }
      shrink();
      return *this;
    }
    ilist& pop_back() noexcept
    {
      if (empty())
        return *this;

      auto tail = m_tail;
      set_tail(tail->prev());

      if (m_tail)
      {
        m_tail->kill_next(allocator());
      }
      else
      {
        node_type::dealloc(allocator(), tail);
        reset_bounds();
      }
      shrink();
      return *this;
    }

    ilist& prepend(ilist&& other) noexcept
    {
      if (other.empty())
        return *this;

      assume_ownership(other.front(), other.back());
      if (m_head) node_type::link(other.m_tail, *m_head, m_head->next());
      m_head = other.m_head;
      if (!m_tail) m_tail = other.m_tail;
      other.loose_content();
      return *this;
    }
    ilist& prepend_to(reference node, ilist&& other) noexcept
    {
      if (!node.belongs_to(this))
      {
        UTILS_ASSERT(false);
        return *this;
      }
      if (other.empty())
        return *this;

      assume_ownership(other.front(), other.back());
      auto nodePrev = node.prev();
      node_type::link(other.m_tail, node, node.next());
      node_type::link(nodePrev, other.front(), other.m_head->next());
      if (node.same_as(m_head)) set_head(other.m_head);
      other.loose_content();
      return *this;
    }
    ilist& prepend_to(iterator it, ilist&& other) noexcept
    {
      if (!it)
        return prepend(std::move(other));

      return prepend_to(*it, std::move(other));
    }
    ilist& prepend_to(reverse_iterator it, ilist&& other) noexcept
    {
      other.reverse();
      if (!it)
        return append(std::move(other));

      return append_to(*it, std::move(other));
    }

    ilist& append(ilist&& other) noexcept
    {
      if (other.empty())
        return *this;

      assume_ownership(other.front(), other.back());
      node_type::link(m_tail, other.front(), other.m_head->next());
      m_tail = other.m_tail;
      if (!m_head) m_head = other.m_head;
      other.loose_content();
      return *this;
    }
    ilist& append_to(reference node, ilist&& other) noexcept
    {
      if (!node.belongs_to(this))
      {
        UTILS_ASSERT(false);
        return *this;
      }
      if (other.empty())
        return *this;

      assume_ownership(other.front(), other.back());
      auto nodeNext = node.next();
      node_type::link(node.prev(), node, other.m_head);
      node_type::link(other.m_tail->prev(), other.back(), nodeNext);
      if (node.same_as(m_tail)) set_tail(other.m_tail);
      other.loose_content();
      return *this;
    }
    ilist& append_to(iterator it, ilist&& other) noexcept
    {
      if (!it)
        return append(std::move(other));

      return append_to(*it, std::move(other));
    }
    ilist& append_to(reverse_iterator it, ilist&& other) noexcept
    {
      other.reverse();
      if (!it)
        return prepend(std::move(other));

      return prepend_to(*it, std::move(other));
    }

  public: // accessors and order manipulation
    const_reference front() const noexcept
    {
      UTILS_ASSERT(!empty());
      return *m_head;
    }
    reference front() noexcept
    {
      return FROM_CONST(front);
    }

    const_reference back() const noexcept
    {
      UTILS_ASSERT(!empty());
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
        m_head->kill_next(allocator());

      node_type::dealloc(allocator(), m_head);
      loose_content();
      return *this;
    }

    ilist& reorder(reference l, reference r) noexcept
    {
      if (!l.belongs_to(this) || !r.belongs_to(this))
      {
        UTILS_ASSERT(false);
        return *this;
      }

      l.reorder_with(r);
      auto newHead = m_head;
      auto newTail = m_tail;

      if (l.same_as(m_head)) newHead = &r;
      if (l.same_as(m_tail)) newTail = &r;
      if (r.same_as(m_head)) newHead = &l;
      if (r.same_as(m_tail)) newTail = &l;

      set_head(newHead);
      set_tail(newTail);
      return *this;
    }

    ilist split_at(reference from) noexcept
    {
      UTILS_ASSERT(from.belongs_to(this));
      auto newHead = &from;
      auto newTail = m_tail;

      auto fp = from.prev();
      from.drop_prev();
      node_type::set_next(fp, {});

      if (from.same_as(m_head)) reset_bounds();
      set_tail(fp);
      reset_size();
      auto head = m_head;
      while (head)
      {
        grow();
        head = head->next();
      }
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
        cur->set_prev(curNext);
        cur->set_next(curPrev);
        cur = curNext;
      }

      set_head(prevTail);
      set_tail(prevHead);
      return *this;
    }

  private:
    template <ilist_unary_predicate<value_type> Pred>
    const_pointer find_impl(Pred&& pred) const noexcept
    {
      auto head = m_head;
      while (head)
      {
        if (pred(*head)) break;
        head = head->next();
      }
      return head;
    }

  public: // iterative manipulations
    template <ilist_unary_predicate<value_type> Pred>
    auto find(Pred&& pred) const noexcept
    {
      auto found = find_impl(std::forward<Pred>(pred));
      return const_iterator{ found };
    }

    template <ilist_unary_predicate<value_type> Pred>
    auto find(Pred&& pred) noexcept
    {
      auto found = FROM_CONST(find_impl, std::forward<Pred>(pred));
      return iterator{ found };
    }

    template <ilist_unary_transform<value_type> Transform>
    ilist& apply(Transform&& transform) noexcept
    {
      auto head = m_head;
      while (head)
      {
        auto next = head->next();
        transform(*head);
        head = next;
      }
      return *this;
    }

    template <ilist_unary_predicate<value_type> Pred>
    ilist& erase(Pred&& pred) noexcept
    {
      auto head = m_head;
      while (head)
      {
        auto next = head->next();
        if (pred(*head))
          remove(*head);
        head = next;
      }
      return *this;
    }

  public: // info and iteration
    auto allocator() const noexcept
    {
      return m_alloc;
    }

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
      set_head(&node_type::make(allocator(), m_head, m_tail, *this, std::forward<Args>(args)...));
      grow();
      return *m_head;
    }

    void assume_ownership(reference node) noexcept
    {
      grow();
      node.attach(*this);
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

    void grow() noexcept
    {
      ++m_size;
    }
    void shrink() noexcept
    {
      --m_size;
    }
    void set_tail(pointer t) noexcept
    {
      m_tail = t;
      if (!m_head) m_head = m_tail;
    }
    void set_head(pointer h) noexcept
    {
      m_head = h;
      if (!m_tail) m_tail = m_head;
    }
    void reset_tail() noexcept
    {
      m_tail = {};
    }
    void reset_head() noexcept
    {
      m_head = {};
    }
    void reset_bounds() noexcept
    {
      reset_head();
      reset_tail();
    }
    void reset_size() noexcept
    {
      m_size = {};
    }
    void loose_content() noexcept
    {
      reset_bounds();
      reset_size();
    }

  private:
    pointer m_head{};
    pointer m_tail{};
    size_type m_size{};
    allocator_type m_alloc{};
  };

}