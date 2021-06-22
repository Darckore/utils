#pragma once

#include <type_traits>
#include <concepts>
#include <algorithm>
#include <ranges>
#include <bit>
#include <numeric>
#include <string_view>
#include <array>

namespace rng = std::ranges;
namespace rvi = std::views;

#include "detail/common.hpp"
#include "detail/strings.hpp"