#pragma once

namespace utils
{
  namespace detail
  {
    template <typename From, typename To>
    concept static_convertible = requires(From& from, To& to)
    {
      static_cast<To&>(from);
    };

    template <typename T> struct type_to_id;
    template <auto TypeId> struct id_to_type;
  }

  template <typename T>
  inline constexpr auto type_to_id_v = detail::type_to_id<T>::value;

  template <auto TypeId>
  using id_to_type_t = typename detail::id_to_type<TypeId>::type;

  template <typename T> auto get_id(const T&) noexcept;

  template <typename To>
  decltype(auto) cast(const auto& src) noexcept
    requires (detail::static_convertible<std::remove_cvref_t<decltype(src)>, To>)
  {
    return static_cast<const To&>(src);
  }

  template <typename To>
  decltype(auto) cast(auto& src) noexcept
    requires (!std::is_const_v<std::remove_reference_t<decltype(src)>>)
  {
    return const_cast<To&>(cast<To>(std::as_const(src)));
  }

  template <auto Id>
  decltype(auto) cast(const auto& src) noexcept
  {
    return cast<id_to_type_t<Id>>(src);
  }

  template <auto Id>
  decltype(auto) cast(auto& src) noexcept
  {
    return cast<id_to_type_t<Id>>(src);
  }

  template <typename To>
  auto try_cast(const auto* src) noexcept
  {
    using res_t = const To*;
    using utils::get_id;
    if (!src || get_id(*src) != type_to_id_v<To>)
      return res_t{};

    return &cast<To>(*src);
  }

  template <typename To>
  auto try_cast(auto* src) noexcept
  {
    using from_type = std::remove_pointer_t<decltype(src)>;
    return const_cast<To*>(try_cast<To>(static_cast<const from_type*>(src)));
  }

  template <auto Id>
  auto try_cast(const auto* src) noexcept
  {
    return try_cast<id_to_type_t<Id>>(src);
  }

  template <auto Id>
  auto try_cast(auto* src) noexcept
  {
    return try_cast<id_to_type_t<Id>>(src);
  }
}