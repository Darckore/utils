#pragma once

namespace utils
{
  namespace detail
  {
    //
    // For Quake's fast inverse square root
    //
    template <real T>
    struct sqrt_magic;

    template <>
    struct sqrt_magic<float>
    {
      static constexpr auto value = 0x5f3759df;
      using type = std::uint32_t;
    };
    template <>
    struct sqrt_magic<double>
    {
      static constexpr auto value = 0x5fe6eb50c7b537a9;
      using type = std::uint64_t;
    };

    template <real T>
    inline constexpr auto sqrt_magic_v = sqrt_magic<T>::value;

    template <real T>
    using sqrt_magic_t = typename sqrt_magic<T>::type;

    template <max_int_t Bytes>
    struct max_factorial;

    template <max_int_t Bytes>
    inline constexpr auto max_factorial_v = max_factorial<Bytes>::value;

    template <>
    struct max_factorial<1>
    {
      static constexpr max_int_t value{ 5 };
    };

    template <>
    struct max_factorial<2>
    {
      static constexpr max_int_t value{ 8 };
    };

    template <>
    struct max_factorial<4>
    {
      static constexpr max_int_t value{ 12 };
    };

    template <>
    struct max_factorial<8>
    {
      static constexpr max_int_t value{ 20 };
    };
  } 

  //
  // A constexpr abs version
  //
  template <typename T> requires std::is_arithmetic_v<T>
  constexpr auto abs(T val) noexcept
  {
    if constexpr (std::unsigned_integral<T>)
      return val;
    else
      return val < 0 ? -val : val;
  }

  //
  // Return the sign of the given value
  //
  template <typename T> requires std::is_arithmetic_v<T>
  constexpr auto sign(T val) noexcept
  {
    if constexpr (std::unsigned_integral<T>)
      return val != 0 ? T{ 1 } : T{ 0 };
    else
      return val < 0
            ? T{ -1 }
            : (val > 0 ? T{ 1 } : T{ 0 });
  }

  //
  // This is mostly for floats
  // To avoid dealing with the epsilon bs on comparing them
  //
  template <comparable T> requires std::is_arithmetic_v<T>
  constexpr bool eq(T left, T right) noexcept
  {
    constexpr auto max_diff = std::numeric_limits<T>::epsilon();
    return abs(left - right) <= max_diff;
  }

  //
  // Inverts the value ( 1 / val )
  //
  template <real T>
  constexpr auto inv(T val) noexcept
  {
    if (val == T{})
      return std::numeric_limits<T>::infinity();

    return T{ 1 } / val;
  }

  //
  // Quake's fast inverse square root
  //
  template <real T>
  constexpr auto inv_sqrt(T number) noexcept
  {
    using detail::sqrt_magic_t;
    using detail::sqrt_magic_v;
    using std::bit_cast;
    using result_type = T;
    using intrm_type = sqrt_magic_t<result_type>;
    constexpr auto one = result_type{ 1 };

    // Special case
    if (eq(number, one))
    {
      return one;
    }

    constexpr auto magic = sqrt_magic_v<result_type>;
    constexpr auto threehalfs1 = result_type{ 1.50131454 };
    constexpr auto threehalfs2 = result_type{ 1.50000086 };
    constexpr auto correction  = result_type{ 0.999124984 };
    const auto half = result_type{ 0.500438180 } * number;

    const auto intermediate = bit_cast<intrm_type>(number);
    auto  res = bit_cast<result_type>(magic - (intermediate >> 1));
    res = res * (threehalfs1 - (half * res * res));
    res = res * (threehalfs2 - correction * (half * res * res));
    return res;
  }

  //
  // Factorial
  // Returns 0 if overflown
  //
  template <std::unsigned_integral I>
  constexpr auto factorial(I value) noexcept
  {
    using result_type = max_int_t;
    if (!value)
      return result_type{ 1 };

    if (value > detail::max_factorial_v<sizeof(result_type)>)
      return result_type{};

    auto res = result_type{ value };
    for (result_type cur = value - 1; cur != 0; --cur)
      res *= cur;

    return res;
  }
}

#include "trig.hpp"
#include "ratio.hpp"