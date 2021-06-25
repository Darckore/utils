#pragma once

#include <type_traits>
#include <concepts>
#include <algorithm>
#include <ranges>
#include <bit>
#include <numeric>
#include <string_view>
#include <array>
#include <random>
#include <future>
#include <chrono>

namespace ranges = std::ranges;
namespace views = std::views;
namespace chrono = std::chrono;

#include "detail/common.hpp"
#include "detail/strings.hpp"
#include "detail/random.hpp"
#include "detail/clock.hpp"