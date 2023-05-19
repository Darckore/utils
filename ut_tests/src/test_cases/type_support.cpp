#include "utils/utils.hpp"

namespace ut_tests
{
  namespace
  {
    struct base1
    {
      enum class id : std::uint8_t { Kind1, Kind2 };

      base1(id i) noexcept : m_id{ i } {}
      id myid() const noexcept { return m_id; }
      id m_id;
    };

    struct d11 : base1 { d11() noexcept : base1{ id::Kind1 } {} };
    struct d12 : base1 { d12() noexcept : base1{ id::Kind2 } {} };

    auto get_id(const base1& b1) noexcept { return b1.myid(); }


    struct base2
    {
      base2(int id) noexcept : hereWeGo{ id } {}
      int hereWeGo;
    };

    struct d21 : base2 { d21() noexcept : base2{ 42 } {} };
    struct d22 : base2 { d22() noexcept : base2{ 69 } {} };

    auto get_id(const base2& b2) noexcept { return b2.hereWeGo; }
  }
}

TYPE_TO_ID_ASSOCIATION(ut_tests::d11, ut_tests::base1::id::Kind1);
TYPE_TO_ID_ASSOCIATION(ut_tests::d12, ut_tests::base1::id::Kind2);
TYPE_TO_ID_ASSOCIATION(ut_tests::d21, 42);
TYPE_TO_ID_ASSOCIATION(ut_tests::d22, 69);

namespace ut_tests
{
  TEST(type_support, t_id_to_type)
  {
    using _d11 = utils::id_to_type_t<base1::id::Kind1>;
    EXPECT_TRUE((std::is_same_v<_d11, d11>));

    using _d12 = utils::id_to_type_t<base1::id::Kind2>;
    EXPECT_TRUE((std::is_same_v<_d12, d12>));

    using _d21 = utils::id_to_type_t<42>;
    EXPECT_TRUE((std::is_same_v<_d21, d21>));

    using _d22 = utils::id_to_type_t<69>;
    EXPECT_TRUE((std::is_same_v<_d22, d22>));
  }

  TEST(type_support, t_type_to_id)
  {
    static constexpr auto _d11 = utils::type_to_id_v<d11>;
    EXPECT_EQ(_d11, base1::id::Kind1);

    static constexpr auto _d12 = utils::type_to_id_v<d12>;
    EXPECT_EQ(_d12, base1::id::Kind2);

    static constexpr auto _d21 = utils::type_to_id_v<d21>;
    EXPECT_EQ(_d21, 42);

    static constexpr auto _d22 = utils::type_to_id_v<d22>;
    EXPECT_EQ(_d22, 69);
  }

  TEST(type_support, t_try_cast)
  {
    d11 _11;
    d22 _22;
    const base1* _1c = &_11;
          base1* _1  = &_11;
    const base2* _2c = &_22;
          base2* _2  = &_22;

    auto cast_11c = utils::try_cast<d11>(_1c);
    auto cast_11  = utils::try_cast<d11>(_1);
    EXPECT_NE(cast_11c, nullptr);
    EXPECT_EQ(cast_11, cast_11c);

    auto cast_12c = utils::try_cast<d12>(_1c);
    auto cast_12  = utils::try_cast<d12>(_1);
    EXPECT_EQ(cast_12c, nullptr);
    EXPECT_EQ(cast_12,  nullptr);

    auto cast_22c = utils::try_cast<69>(_2c);
    using _22_t = std::remove_pointer_t<decltype(cast_22c)>;
    EXPECT_EQ(cast_22c, &_22);
    EXPECT_TRUE((std::is_const_v<_22_t>));

    auto cast_21 = utils::try_cast<42>(_2);
    EXPECT_EQ(cast_21, nullptr);
  }
}