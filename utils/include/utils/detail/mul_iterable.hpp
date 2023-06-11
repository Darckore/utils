#pragma once

namespace utils
{
  namespace detail
  {
    template <typename I>
    concept fwd_it = std::forward_iterator<I>;
  }

  //
  // An iterator which incorporates multiple iterators
  // into different ranges and allows iterating through all
  // of them simultaneously
  //
  template <detail::fwd_it ...Iters>
    requires (detail::not_empty<Iters...>)
  class multiple_it
  {
  public:
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using value_type = std::tuple<typename Iters::value_type...>;
    using iters = std::tuple<Iters...>;
    using ptrs  = std::tuple<typename Iters::pointer...>;
    using refs  = std::tuple<typename Iters::reference...>;

    static constexpr auto iter_count = std::tuple_size_v<iters>;

  private:
    template <typename Int, Int... Seq>
    using idx_seq = std::integer_sequence<Int, Seq...>;

    template <size_type N>
    using idx_gen = std::make_index_sequence<N>;

  private:
    template <typename Int, Int... Seq>
    constexpr void increment(idx_seq<Int, Seq...>) noexcept
    {
      ((++(std::get<Seq>(m_iters))), ...);
    }

    template <typename Int, Int... Seq>
    constexpr refs deref_all(idx_seq<Int, Seq...>) const noexcept
    {
      return { (*std::get<Seq>(m_iters))... };
    }

    template <typename Int, Int... Seq>
    constexpr bool eq_compare(const multiple_it& other, idx_seq<Int, Seq...>) const noexcept
    {
      return ((std::get<Seq>(m_iters) == std::get<Seq>(other.m_iters)) || ...);
    }

  public:
    CLASS_SPECIALS_ALL(multiple_it);

    constexpr explicit multiple_it(Iters... iters) noexcept :
      m_iters{ iters... }
    {}

    constexpr bool operator==(const multiple_it& other) const noexcept
    {
      return eq_compare(other, idx_gen<iter_count>{});
    }

    constexpr multiple_it& operator++() noexcept
    {
      increment(idx_gen<iter_count>{});
      return *this;
    }

    constexpr multiple_it operator++(int) noexcept
    {
      auto&& self = *this;
      auto copy = self;
      ++self;
      return copy;
    }

    constexpr auto operator*() const noexcept
    {
      return deref_all(idx_gen<iter_count>{});
    }

  public:
    template <size_type Idx>
      requires (Idx < iter_count)
    constexpr auto get() const noexcept
    {
      return std::get<Idx>(m_iters);
    }

  private:
    iters m_iters{};
  };


  namespace detail
  {
    template <iterable ...Ranges>
    class iterator_pair
    {
    private:
      template <iterable Range>
      using range_beg = decltype(std::begin(std::declval<Range&>()));

      template <iterable Range>
      using range_end = decltype(std::end(std::declval<Range&>()));

    public:
      using iterator = multiple_it<range_beg<Ranges>...>;
      using sentinel = multiple_it<range_end<Ranges>...>;

    public:
      CLASS_SPECIALS_NODEFAULT(iterator_pair);

      constexpr iterator_pair(Ranges& ...ranges) noexcept :
        m_beg{ std::begin(ranges)... },
        m_end{ std::end(ranges)... }
      {}

      constexpr bool operator==(const iterator_pair&) const noexcept = default;

    public:
      auto begin() const noexcept
      {
        return m_beg;
      }
      auto end() const noexcept
      {
        return m_end;
      }

    private:
      iterator m_beg{};
      sentinel m_end{};
    };
  }

  //
  // Makes an iterable range out of multiple collections
  //
  template <detail::iterable ...Ranges>
  constexpr auto make_iterators(Ranges& ...ranges) noexcept
  {
    return detail::iterator_pair{ ranges... };
  }
}

template <typename ...T>
struct std::tuple_size<utils::multiple_it<T...>> : 
  std::tuple_size<typename utils::multiple_it<T...>::iters>
{};

template <std::size_t I, typename ...T>
struct std::tuple_element<I, utils::multiple_it<T...>> :
  std::tuple_element<I, typename utils::multiple_it<T...>::iters>
{};