#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "vector2d.h"
#include "vector2d.h"  // test include guards

#include "quantity.h"
#include "quantity.h"  // test include guards

template <class T, class U>
inline constexpr auto kAddable = requires(T x, U y) { x + y; };

template <class T, class U>
inline constexpr auto kSubtractable = requires(T x, U y) { x - y; };

TEST_CASE("Quantity Vector initialization", "[quantity vector]") {
  SECTION("Initialization with Mass") {
    const auto v = Vector2D<Mass>{3.0, 4.0};
    REQUIRE(v.x.value == Approx(3.0));
    REQUIRE(v.y.value == Approx(4.0));
    static_assert(std::is_same_v<decltype(v), const Vector2D<Mass>>);
  }

  SECTION("Initialization with Quantity") {
    const auto v = Vector2D<Quantity<1, 2, 3>>{3, 4};
    REQUIRE(v.x.value == Approx(3.0));
    REQUIRE(v.y.value == Approx(4.0));
    static_assert(std::is_same_v<decltype(v), const Vector2D<Quantity<1, 2, 3>>>);
  }
}

TEST_CASE("Quantity Vector addition and subtraction", "[quantity vector]") {
  static_assert(!kAddable<Vector2D<Mass>, Vector2D<Length>>);
  static_assert(!kSubtractable<Vector2D<Mass>, Vector2D<Length>>);

  const auto a = Vector2D<Mass>{3.0, 4.0};
  const auto b = Vector2D<Mass>{1.0, 2.0};

  SECTION("Addition") {
    const auto sum = a + b;
    REQUIRE(sum.x.value == Approx(4.0));
    REQUIRE(sum.y.value == Approx(6.0));
    static_assert(std::is_same_v<decltype(sum), const Vector2D<Mass>>);
  }

  SECTION("Subtraction") {
    const auto diff = a - b;
    REQUIRE(diff.x.value == Approx(2.0));
    REQUIRE(diff.y.value == Approx(2.0));
    static_assert(std::is_same_v<decltype(diff), const Vector2D<Mass>>);
  }
}

TEST_CASE("Quantity Vector unary operators", "[quantity vector]") {
  const auto v = Vector2D<Time>{3.0, 4.0};
  const auto plus = +v;
  const auto minus = -v;
  REQUIRE(plus.x.value == Approx(3.0));
  REQUIRE(plus.y.value == Approx(4.0));
  REQUIRE(minus.x.value == Approx(-3.0));
  REQUIRE(minus.y.value == Approx(-4.0));
  static_assert(std::is_same_v<decltype(plus), const Vector2D<Time>>);
  static_assert(std::is_same_v<decltype(minus), const Vector2D<Time>>);
}

TEST_CASE("Quantity Vector scalar multiplication", "[quantity vector]") {
  const auto v = Vector2D<Length>{3.0, 4.0};

  SECTION("Scalar Multiplication") {
    const auto scaled = v * 2;
    REQUIRE(scaled.x.value == Approx(6.0));
    REQUIRE(scaled.y.value == Approx(8.0));
    static_assert(std::is_same_v<decltype(scaled), const Vector2D<Length>>);
  }

  SECTION("Scalart Multiplication reversed") {
    const auto scaled = 2 * v;
    REQUIRE(scaled.x.value == Approx(6.0));
    REQUIRE(scaled.y.value == Approx(8.0));
    static_assert(std::is_same_v<decltype(scaled), const Vector2D<Length>>);
  }

  SECTION("Scalar Division") {
    const auto scaled = v / 2;
    REQUIRE(scaled.x.value == Approx(1.5));
    REQUIRE(scaled.y.value == Approx(2.0));
    static_assert(std::is_same_v<decltype(scaled), const Vector2D<Length>>);
  }
}

TEST_CASE("Quantity Vector quantity multiplication", "[quantity vector]") {
  const auto v = Vector2D<Speed>{3.0, 4.0};
  const auto t = Time{0.5};

  SECTION("Quantity Multiplication") {
    const auto distance = v * t;
    REQUIRE(distance.x.value == Approx(1.5));
    REQUIRE(distance.y.value == Approx(2.0));
    static_assert(std::is_same_v<decltype(distance), const Vector2D<Length>>);
  }

  SECTION("Quantity Multiplication reversed") {
    const auto distance = t * v;
    REQUIRE(distance.x.value == Approx(1.5));
    REQUIRE(distance.y.value == Approx(2.0));
    static_assert(std::is_same_v<decltype(distance), const Vector2D<Length>>);
  }

  SECTION("Quantity Division") {
    const auto acceleration = v / t;
    REQUIRE(acceleration.x.value == Approx(6.0));
    REQUIRE(acceleration.y.value == Approx(8.0));
    static_assert(std::is_same_v<decltype(acceleration), const Vector2D<Acceleration>>);
  }
}

TEST_CASE("Quantity Vector compound operators", "[quantity vector]") {
  const auto v = Vector2D<Length>{3.0, 4.0};

  SECTION("Compound Assignment Addition") {
    auto copy = v;
    copy += v;
    REQUIRE(copy.x.value == Approx(6.0));
    REQUIRE(copy.y.value == Approx(8.0));
  }

  SECTION("Compound Assignment Subtraction") {
    auto copy = v;
    copy -= v;
    REQUIRE(copy.x.value == Approx(0.0));
    REQUIRE(copy.y.value == Approx(0.0));
  }

  SECTION("Compound Assignment Multiplication") {
    auto copy = v;
    copy *= 2;
    REQUIRE(copy.x.value == Approx(6.0));
    REQUIRE(copy.y.value == Approx(8.0));
  }

  SECTION("Compound Assignment Division") {
    auto copy = v;
    copy /= 2;
    REQUIRE(copy.x.value == Approx(1.5));
    REQUIRE(copy.y.value == Approx(2.0));
  }
}

TEST_CASE("Quantity Vector Dot and Length calculation", "[quantity vector]") {
  const auto v = Vector2D<Speed>{3.0, 4.0};

  SECTION("Dot Product") {
    const auto dot = Dot(v, v);
    REQUIRE(dot.value == Approx(25.0));
    static_assert(std::is_same_v<decltype(dot), const Quantity<-2, 2, 0>>);
  }
}