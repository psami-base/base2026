#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "sum.hpp"
#include "sum.hpp"  // check include guards

#include <limits>

template <class F, class... Args>
inline constexpr auto kTakesArguments = false;

template <class R, class... Args>
inline constexpr auto kTakesArguments<R(Args...), Args...> = true;

template <class R, class... Args>
inline constexpr auto kTakesArguments<R(Args...) noexcept, Args...> = true;

inline constexpr auto kMaxInt = std::numeric_limits<int>::max();

TEST_CASE("Interface", "[Sum]") {
  static_assert(kTakesArguments<decltype(Sum), int, int>, "Sum must take ints as arguments");
}

TEST_CASE("PositiveSmall", "[Sum]") {
  REQUIRE(Sum(2, 5) == 7);
  REQUIRE(Sum(6, 4) == 10);
}

TEST_CASE("NegativeSmall", "[Sum]") {
  REQUIRE(Sum(-3, -6) == -9);
  REQUIRE(Sum(-7, -5) == -12);
}

TEST_CASE("MixSmall", "[Sum]") {
  REQUIRE(Sum(-4, 5) == 1);
  REQUIRE(Sum(4, -6) == -2);
  REQUIRE(Sum(-11, 11) == 0);
}

TEST_CASE("PositiveLarge", "[Sum]") {
  REQUIRE(Sum(kMaxInt, kMaxInt) == int64_t{kMaxInt} + int64_t{kMaxInt});
  REQUIRE(Sum(kMaxInt, kMaxInt / 2) == int64_t{kMaxInt / 2} + int64_t{kMaxInt});
  REQUIRE(Sum(5, kMaxInt) == int64_t{kMaxInt} + int64_t{5});
}

TEST_CASE("NegativeLarge", "[Sum]") {
  REQUIRE(Sum(-kMaxInt, -kMaxInt) == int64_t{-kMaxInt} + int64_t{-kMaxInt});
  REQUIRE(Sum(-kMaxInt / 2, -kMaxInt) == int64_t{-kMaxInt / 2} + int64_t{-kMaxInt});
  REQUIRE(Sum(-5, -kMaxInt) == int64_t{-kMaxInt} + int64_t{-5});
}

TEST_CASE("MixLarge", "[Sum]") {
  REQUIRE(Sum(-kMaxInt, kMaxInt) == int64_t{-kMaxInt} + int64_t{kMaxInt});
  REQUIRE(Sum(-kMaxInt / 2, kMaxInt) == int64_t{-kMaxInt / 2} + int64_t{kMaxInt});
  REQUIRE(Sum(-kMaxInt, kMaxInt / 2) == int64_t{kMaxInt / 2} + int64_t{-kMaxInt});
  REQUIRE(Sum(5, -kMaxInt) == int64_t{-kMaxInt} + int64_t{5});
  REQUIRE(Sum(kMaxInt, -5) == int64_t{-5} + int64_t{kMaxInt});
}
