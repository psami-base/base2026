#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "vector2d.h"
#include "vector2d.h"  // test include guards

template <class T, class U>
inline constexpr auto kAddable = requires(T x, U y) { x + y; };

TEST_CASE("Vector2D Initialization", "[vector2d]") {
  SECTION("Initialization with double") {
    const auto v = Vector2D{3.0, 4.0};
    REQUIRE(v.x == Approx(3.0));
    REQUIRE(v.y == Approx(4.0));
    static_assert(std::is_same_v<decltype(v), const Vector2D<double>>);
  }

  SECTION("Initialization with int") {
    const auto v = Vector2D{3, 4};
    REQUIRE(v.x == 3);
    REQUIRE(v.y == 4);
    static_assert(std::is_same_v<decltype(v), const Vector2D<int>>);
  }
}

TEST_CASE("Vector2D Addition and Subtraction", "[vector2d]") {
  const auto a = Vector2D{3.0, 4.0};
  const auto b = Vector2D{1.0, 2.0};

  SECTION("Addition") {
    const auto sum = a + b;
    REQUIRE(sum.x == Approx(4.0));
    REQUIRE(sum.y == Approx(6.0));
    static_assert(std::is_same_v<decltype(sum), const Vector2D<double>>);
  }

  SECTION("Subtraction") {
    const auto diff = a - b;
    REQUIRE(diff.x == Approx(2.0));
    REQUIRE(diff.y == Approx(2.0));
    static_assert(std::is_same_v<decltype(diff), const Vector2D<double>>);
  }

  SECTION("Compound Assignment Addition") {
    auto copy = a;
    copy += b;
    REQUIRE(copy.x == Approx(4.0));
    REQUIRE(copy.y == Approx(6.0));
  }

  SECTION("Compound Assignment Subtraction") {
    auto copy = a;
    copy -= b;
    REQUIRE(copy.x == Approx(2.0));
    REQUIRE(copy.y == Approx(2.0));
  }

  SECTION("Unary Minus") {
    const auto neg = -a;
    REQUIRE(neg.x == Approx(-3.0));
    REQUIRE(neg.y == Approx(-4.0));
  }
}

TEST_CASE("Vector2D Scalar Multiplication and Division", "[vector2d]") {
  const auto v = Vector2D{3.0, 4.0};

  SECTION("Scalar Multiplication") {
    const auto scaled = v * 2.0;
    REQUIRE(scaled.x == Approx(6.0));
    REQUIRE(scaled.y == Approx(8.0));
    static_assert(std::is_same_v<decltype(scaled), const Vector2D<double>>);
  }

  SECTION("Scalar Multiplication Commutative") {
    const auto scaled = 2.0 * v;
    REQUIRE(scaled.x == Approx(6.0));
    REQUIRE(scaled.y == Approx(8.0));
    static_assert(std::is_same_v<decltype(scaled), const Vector2D<double>>);
  }

  SECTION("Scalar Division") {
    const auto divided = v / 2.0;
    REQUIRE(divided.x == Approx(1.5));
    REQUIRE(divided.y == Approx(2.0));
    static_assert(std::is_same_v<decltype(divided), const Vector2D<double>>);
  }

  SECTION("Compound Assignment Multiplication") {
    auto copy = v;
    copy *= 2.0;
    REQUIRE(copy.x == Approx(6.0));
    REQUIRE(copy.y == Approx(8.0));
  }

  SECTION("Compound Assignment Division") {
    auto copy = v;
    copy /= 2.0;
    REQUIRE(copy.x == Approx(1.5));
    REQUIRE(copy.y == Approx(2.0));
  }
}

TEST_CASE("Vector2D Equality Operators", "[vector2d]") {
  const auto a = Vector2D{3, 4};
  const auto b = Vector2D{3, 4};
  const auto c = Vector2D{1, 2};

  SECTION("Equality") {
    REQUIRE(a == b);
    REQUIRE_FALSE(a == c);
  }

  SECTION("Inequality") {
    REQUIRE(a != c);
    REQUIRE_FALSE(a != b);
  }
}

TEST_CASE("Vector2D Dot Product", "[vector2d]") {
  const auto a = Vector2D{3.0, 4.0};
  const auto b = Vector2D{1.0, 2.0};

  SECTION("Dot Product") {
    const auto result = Dot(a, b);
    REQUIRE(result == Approx(11.0));
  }
}

TEST_CASE("Vector2D Type Compatibility", "[vector2d]") {
  static_assert(!kAddable<Vector2D<int>, Vector2D<double>>);

  const auto v_int = Vector2D{3, 4};

  SECTION("Scalar Multiplication with Compatible Types") {
    const auto scaled = v_int * 2 + 2 * v_int;
    REQUIRE(scaled.x == 12);
    REQUIRE(scaled.y == 16);
    static_assert(std::is_same_v<decltype(scaled), const Vector2D<int>>);
  }

  SECTION("Scalar Multiplication with Double") {
    {
      const auto scaled = v_int * 2.0;
      REQUIRE(scaled.x == Approx(6.0));
      REQUIRE(scaled.y == Approx(8.0));
      static_assert(std::is_same_v<decltype(scaled), const Vector2D<double>>);
    }

    {
      const auto scaled = 2.0 * v_int;
      REQUIRE(scaled.x == Approx(6.0));
      REQUIRE(scaled.y == Approx(8.0));
      static_assert(std::is_same_v<decltype(scaled), const Vector2D<double>>);
    }
  }
}

TEST_CASE("Vector2D Compound Assignment Operators", "[vector2d]") {
  const auto v = Vector2D{3.0, 4.0};
  const auto delta = Vector2D{1.0, 2.0};

  SECTION("Compound Assignment Addition") {
    auto copy = v;
    copy += delta;
    REQUIRE(copy.x == Approx(4.0));
    REQUIRE(copy.y == Approx(6.0));
  }

  SECTION("Compound Assignment Subtraction") {
    auto copy = v;
    copy -= delta;
    REQUIRE(copy.x == Approx(2.0));
    REQUIRE(copy.y == Approx(2.0));
  }

  SECTION("Compound Assignment Multiplication") {
    auto copy = v;
    copy *= 2;
    REQUIRE(copy.x == Approx(6.0));
    REQUIRE(copy.y == Approx(8.0));
  }

  SECTION("Compound Assignment Division") {
    auto copy = v;
    copy /= 2;
    REQUIRE(copy.x == Approx(1.5));
    REQUIRE(copy.y == Approx(2.0));
  }
}

TEST_CASE("Vector2D Dot Product: Mixed", "[vector2d]") {
  const auto v_double = Vector2D{3.0, 4.0};
  const auto v_int = Vector2D{1, 2};

  SECTION("Dot Int") {
    const auto dot = Dot(v_int, v_int);
    REQUIRE(dot == 5);
    static_assert(std::is_same_v<decltype(dot), const int>);
  }

  SECTION("Dot Mixed") {
    const auto dot = Dot(v_double, v_int);
    REQUIRE(dot == Approx(11.0));
    static_assert(std::is_same_v<decltype(dot), const double>);
  }
}
