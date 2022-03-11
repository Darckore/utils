#pragma once

namespace utils
{
  namespace detail
  {
    template <typename T>
    struct rnd_distr;

    template <integer T>
    struct rnd_distr<T>
    {
      using type = std::uniform_int_distribution<T>;
    };

    template <real T>
    struct rnd_distr<T>
    {
      using type = std::uniform_real_distribution<T>;
    };

    template <>
    struct rnd_distr<bool>
    {
      using type = std::bernoulli_distribution;
    };

    template <typename T>
    using random_distribution_t = rnd_distr<T>::type;
  }

  //
  // A random generator
  // Usage:
  // rng r{ from, to };
  // auto rnd_val = r();
  //
  template <typename T,
    typename Distr  = detail::random_distribution_t<T>,
    std::uniform_random_bit_generator Engine = std::mt19937>
  class rng
  {
  public:
    using value_type     = T;
    using seed_seq_t     = std::seed_seq;
    using distribution_t = Distr;
    using engine_t       = Engine;
    using seed_type      = engine_t::result_type;
  
  public:
    CLASS_SPECIALS_NONE(rng);

    template <typename A1, typename ...Args>
      requires detail::all_convertible<A1, Args...>
    explicit rng(A1&& val, Args&&... vals) :
      m_rng{ std::random_device{}() },
      m_dist{ std::forward<A1>(val), std::forward<Args>(vals)... }
    {}

    template <typename A>
    explicit rng(A&& val) :
      m_rng{ std::random_device{}() },
      m_dist{ std::forward<A>(val) }
    {}
  
    auto operator()()
    {
      return m_dist(m_rng);
    }

    void seed(seed_type s)
    {
      m_rng.seed(s);
    }
    void seed(seed_seq_t& seq)
    {
      m_rng.seed(seq);
    }
  
  private:
    engine_t m_rng;
    distribution_t m_dist;
  };

  template <typename T, typename ...Args>
  rng(T, Args...)->rng<T>;

  template <typename T>
  rng(T)->rng<T>;
}