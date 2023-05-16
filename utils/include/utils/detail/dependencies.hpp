#pragma once

// std headers we'll most likely be using
#include <type_traits>
#include <concepts>
#include <utility>
#include <algorithm>
#include <ranges>

#include <bit>
#include <numeric>
#include <numbers>
#include <random>
#include <future>
#include <chrono>
#include <atomic>
#include <initializer_list>

#include <source_location>
#include <iostream>
#include <fstream>

#include <string>
#include <string_view>
#include <array>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>

#include <filesystem>
#include <memory>

namespace utils
{
  // just because it is often needed everywhere
  using namespace std::literals;

  namespace ranges = std::ranges;
  namespace views  = std::views;
  namespace chrono = std::chrono;
}

// Class boilerplate reduction

#define CLASS_DEFAULT = default

#define CLASS_DELETE = delete

#define CLASS_DEFAULT_CTOR(clName)   constexpr clName() noexcept

#define CLASS_COPY_CTOR(clName)      constexpr clName(const clName&) noexcept
#define CLASS_COPY_OPERATOR(clName)  constexpr clName& operator=(const clName&) noexcept

#define CLASS_MOVE_CTOR(clName)      constexpr clName(clName&&) noexcept
#define CLASS_MOVE_OPERATOR(clName)  constexpr clName& operator=(clName&&) noexcept

#define CLASS_SPECIALS_ALL(clName)\
  CLASS_DEFAULT_CTOR(clName)  CLASS_DEFAULT;\
  CLASS_COPY_CTOR(clName)     CLASS_DEFAULT;\
  CLASS_COPY_OPERATOR(clName) CLASS_DEFAULT;\
  CLASS_MOVE_CTOR(clName)     CLASS_DEFAULT;\
  CLASS_MOVE_OPERATOR(clName) CLASS_DEFAULT

#define CLASS_SPECIALS_ALL_CUSTOM(clName)\
  CLASS_COPY_CTOR(clName)     CLASS_DEFAULT;\
  CLASS_COPY_OPERATOR(clName) CLASS_DEFAULT;\
  CLASS_MOVE_CTOR(clName)     CLASS_DEFAULT;\
  CLASS_MOVE_OPERATOR(clName) CLASS_DEFAULT

#define CLASS_SPECIALS_NOCOPY(clName)\
  CLASS_DEFAULT_CTOR(clName)  CLASS_DEFAULT;\
  CLASS_COPY_CTOR(clName)     CLASS_DELETE;\
  CLASS_COPY_OPERATOR(clName) CLASS_DELETE;\
  CLASS_MOVE_CTOR(clName)     CLASS_DEFAULT;\
  CLASS_MOVE_OPERATOR(clName) CLASS_DEFAULT

#define CLASS_SPECIALS_NOCOPY_CUSTOM(clName)\
  CLASS_COPY_CTOR(clName)     CLASS_DELETE;\
  CLASS_COPY_OPERATOR(clName) CLASS_DELETE;\
  CLASS_MOVE_CTOR(clName)     CLASS_DEFAULT;\
  CLASS_MOVE_OPERATOR(clName) CLASS_DEFAULT

#define CLASS_SPECIALS_NODEFAULT(clName)\
  CLASS_DEFAULT_CTOR(clName)  CLASS_DELETE;\
  CLASS_COPY_CTOR(clName)     CLASS_DEFAULT;\
  CLASS_COPY_OPERATOR(clName) CLASS_DEFAULT;\
  CLASS_MOVE_CTOR(clName)     CLASS_DEFAULT;\
  CLASS_MOVE_OPERATOR(clName) CLASS_DEFAULT

#define CLASS_SPECIALS_NODEFAULT_NOCOPY(clName)\
  CLASS_DEFAULT_CTOR(clName)  CLASS_DELETE;\
  CLASS_COPY_CTOR(clName)     CLASS_DELETE;\
  CLASS_COPY_OPERATOR(clName) CLASS_DELETE;\
  CLASS_MOVE_CTOR(clName)     CLASS_DEFAULT;\
  CLASS_MOVE_OPERATOR(clName) CLASS_DEFAULT

#define CLASS_SPECIALS_NONE(clName)\
  CLASS_DEFAULT_CTOR(clName)  CLASS_DELETE;\
  CLASS_COPY_CTOR(clName)     CLASS_DELETE;\
  CLASS_COPY_OPERATOR(clName) CLASS_DELETE;\
  CLASS_MOVE_CTOR(clName)     CLASS_DELETE;\
  CLASS_MOVE_OPERATOR(clName) CLASS_DELETE

#define CLASS_SPECIALS_NONE_CUSTOM(clName)\
  CLASS_COPY_CTOR(clName)     CLASS_DELETE;\
  CLASS_COPY_OPERATOR(clName) CLASS_DELETE;\
  CLASS_MOVE_CTOR(clName)     CLASS_DELETE;\
  CLASS_MOVE_OPERATOR(clName) CLASS_DELETE