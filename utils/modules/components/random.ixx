module;

#include <random>
#include "utils/macros.hpp"

export module utils:random;
import :definitions;

export namespace utils
{
  //
  // A random generator
  // Usage:
  // rng r{ from, to };
  // auto rnd_val = r();
  //
  template <typename T,
    typename Distr = random_distribution_t<T>,
    std::uniform_random_bit_generator Engine = std::mt19937>
  class rng
  {
  public:
    using value_type = T;
    using seed_seq_t = std::seed_seq;
    using distribution_t = Distr;
    using engine_t = Engine;
    using seed_type = engine_t::result_type;

  public:
    CLASS_SPECIALS_NONE(rng);

    template <typename A1, typename A2, typename ...Args>
      requires all_convertible<A1, A2, Args...>
    explicit rng(A1&& val1, A2&& val2, Args&&... vals) :
      m_rng{ std::random_device{}() },
      m_dist{ std::forward<A1>(val1), std::forward<A2>(val2), std::forward<Args>(vals)... }
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
  rng(T, Args...) -> rng<T>;
}