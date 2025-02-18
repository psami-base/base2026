#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "any.hpp"
#include "any.hpp"  // check include guards

#include <vector>
#include <utility>

TEST_CASE("Constructors", "[Any]") {
  static_assert(std::is_default_constructible_v<Any>, "Any should be default constructible");

  const auto b = Any(11);
  REQUIRE(AnyCast<int>(b) == 11);

  auto c = b;
  REQUIRE(AnyCast<int>(b) == 11);
  REQUIRE(AnyCast<int>(c) == 11);

  const auto d = std::move(c);
  REQUIRE(AnyCast<int>(d) == 11);

  const auto e = Any(std::vector<int>(10));
  REQUIRE(AnyCast<std::vector<int>>(e).size() == 10);
}

TEST_CASE("Assignment") {
  auto a = Any();
  auto b = Any();

  a = 11;
  REQUIRE(AnyCast<int>(a) == 11);

  a = &a;
  REQUIRE(AnyCast<Any*>(a) == &a);

  b = std::as_const(a);
  REQUIRE(AnyCast<Any*>(b) == &a);

  a = 1.0;
  REQUIRE(AnyCast<double>(a) == 1.0);
  REQUIRE(AnyCast<Any*>(b) == &a);

  b = std::move(a);
  REQUIRE(AnyCast<double>(b) == 1.0);

  b = b;
  REQUIRE(AnyCast<double>(b) == 1.0);
}

TEST_CASE("Swap") {
  auto a = Any(1);
  auto b = Any(&a);

  a.Swap(b);
  REQUIRE(AnyCast<Any*>(a) == &a);
  REQUIRE(AnyCast<int>(b) == 1);

  b.Swap(b);
  REQUIRE(AnyCast<int>(b) == 1);
}

TEST_CASE("HasValue") {
  auto a = Any();
  REQUIRE_FALSE(std::as_const(a).HasValue());

  auto b = Any(11);
  REQUIRE(b.HasValue());

  a = b;
  REQUIRE(a.HasValue());
  REQUIRE(b.HasValue());

  a = std::move(b);
  REQUIRE(a.HasValue());
  REQUIRE_FALSE(b.HasValue());  // NOLINT check "moved-from" is valid state

  a.Reset();
  REQUIRE_FALSE(a.HasValue());
}

TEST_CASE("BadAnyCast") {
  auto a = Any();
  REQUIRE_THROWS_AS(AnyCast<int>(a), BadAnyCast);  // NOLINT

  a = 11;
  REQUIRE_THROWS_AS(AnyCast<char*>(a), BadAnyCast);  // NOLINT
}
