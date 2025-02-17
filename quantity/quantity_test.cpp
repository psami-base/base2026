#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "quantity.hpp"
#include "quantity.hpp"  // test include guard

#include <type_traits>

template <class T, class U>
inline constexpr bool kAddable = requires(T a, U b) {
  a + b;
  b + a;
};

template <class T, class U>
inline constexpr bool kSubtractable = requires(T a, U b) {
  a - b;
  b - a;
};

template <class T, class U>
inline constexpr bool kComparable = requires(T a, U b) {
  a < b;
  a > b;
  a <= b;
  a >= b;
  a == b;
  a != b;
};

TEST_CASE("Static Assertions for Dimensions", "[quantity]") {

  SECTION("Basic Dimensions") {
    static_assert(std::is_same_v<Time, Quantity<1, 0, 0>>, "Time should have dimensions [T=1, L=0, M=0]");
    static_assert(std::is_same_v<Length, Quantity<0, 1, 0>>, "Length should have dimensions [T=0, L=1, M=0]");
    static_assert(std::is_same_v<Mass, Quantity<0, 0, 1>>, "Mass should have dimensions [T=0, L=0, M=1]");
    static_assert(std::is_same_v<Speed, Quantity<-1, 1, 0>>, "Speed should have dimensions [T=-1, L=1, M=0]");
    static_assert(std::is_same_v<Acceleration, Quantity<-2, 1, 0>>,
                  "Acceleration should have dimensions [T=-2, L=1, M=0]");
    static_assert(std::is_same_v<Force, Quantity<-2, 1, 1>>, "Force should have dimensions [T=-2, L=1, M=1]");
    static_assert(std::is_same_v<Energy, Quantity<-2, 2, 1>>, "Energy should have dimensions [T=-2, L=2, M=1]");
  }

  SECTION("Addition Result Type") {
    const auto l1 = Length{};
    const auto l2 = Length{};
    static_assert(std::is_same_v<decltype(l1 + l2), Length>, "Sum should be Length");
  }

  SECTION("Multiplication Result Type") {
    const auto l = Length{};
    const auto t = Time{};
    static_assert(std::is_same_v<decltype(l * t), Quantity<1, 1, 0>>, "Result should have dimensions [T=1, L=1, M=0]");
  }

  SECTION("Division Result Type") {
    const auto l = Length{};
    const auto t = Time{};
    static_assert(std::is_same_v<decltype(l / t), Speed>, "Speed should be Speed");
  }

  SECTION("Multiplication by Scalar Result Type") {
    const auto l = Length{};
    static_assert(std::is_same_v<decltype(l * 1.5), Length>, "Result should be Length");
    static_assert(std::is_same_v<decltype(1.5 * l), Length>, "Result should be Length");
  }

  SECTION("Division by Scalar Result Type") {
    const auto l = Length{};
    static_assert(std::is_same_v<decltype(l / 2.0), Length>, "Result should be Length");
    static_assert(std::is_same_v<decltype(2.0 / l), Quantity<0, -1, 0>>, "Result should be Length^{-1}");
  }

  SECTION("General Case") {
    const auto x = Quantity<1, 2, 3>{};
    const auto y = Quantity<4, 5, 6>{};
    static_assert(std::is_same_v<decltype(x * y), Quantity<5, 7, 9>>, "Result should have dimensions [T=5, L=7, M=9]");
    static_assert(std::is_same_v<decltype(x / y), Quantity<-3, -3, -3>>,
                  "Result should have dimensions [T=-3, L=-3, M=-3]");
  }
}

TEST_CASE("Quantity Initialization", "[quantity]") {
  const auto l = Length{5.0};
  REQUIRE(l.value == Approx(5.0));

  const auto m = Mass{2.0};
  REQUIRE(m.value == Approx(2.0));

  const auto t = Time{10.0};
  REQUIRE(t.value == Approx(10.0));

  const auto s = Speed{-3.5};
  REQUIRE(s.value == Approx(-3.5));
}

TEST_CASE("Quantity Addition and Subtraction", "[quantity]") {
  auto l1 = Length{5.0};
  auto l2 = Length{3.0};

  SECTION("Addition") {
    const auto sum = l1 + l2;
    REQUIRE(sum.value == Approx(8.0));
  }

  SECTION("Subtraction") {
    const auto diff = l1 - l2;
    REQUIRE(diff.value == Approx(2.0));
  }

  SECTION("Compound Assignment Addition") {
    l1 += std::as_const(l2);
    REQUIRE(l1.value == Approx(8.0));
  }

  SECTION("Compound Assignment Subtraction") {
    l1 -= std::as_const(l2);
    REQUIRE(l1.value == Approx(2.0));
  }

  SECTION("Unary Minus") {
    auto neg = -std::as_const(l1);
    REQUIRE(neg.value == Approx(-5.0));
  }

  static_assert(!kAddable<Length, Mass>, "Length and Mass should not be addable");
}

TEST_CASE("Quantity Multiplication and Division", "[quantity]") {
  auto l = Length{5.0};
  auto t = Time{2.0};
  const auto c = 2.0;

  SECTION("Multiplication") {
    const auto result = l * t;
    REQUIRE(result.value == Approx(10.0));
  }

  SECTION("Division") {
    const auto speed = l / t;
    REQUIRE(speed.value == Approx(2.5));
  }

  SECTION("Multiplication by Scalar") {
    const auto double_length = l * 2.0;
    REQUIRE(double_length.value == Approx(10.0));

    const auto triple_length = 3.0 * l;
    REQUIRE(triple_length.value == Approx(15.0));
  }

  SECTION("Division by Scalar") {
    const auto half_length = l / 2.0;
    REQUIRE(half_length.value == Approx(2.5));

    const auto frequency = 10.0 / t;
    REQUIRE(frequency.value == Approx(5.0));
  }

  SECTION("Compound Assignment Multiplication by Scalar") {
    l *= c;
    REQUIRE(l.value == Approx(10.0));
  }

  SECTION("Compound Assignment Division by Scalar") {
    l /= c;
    REQUIRE(l.value == Approx(2.5));
  }
}

TEST_CASE("Quantity Comparisons", "[quantity]") {
  const auto l1 = Length{5.0};
  const auto l2 = Length{3.0};
  const auto l3 = Length{5.0};

  SECTION("Equality") {
    REQUIRE(l1 == l3);
    REQUIRE_FALSE(l1 == l2);
  }

  SECTION("Inequality") {
    REQUIRE(l1 != l2);
    REQUIRE_FALSE(l1 != l3);
  }

  SECTION("Less Than") {
    REQUIRE(l2 < l1);
    REQUIRE_FALSE(l1 < l2);
  }

  SECTION("Greater Than") {
    REQUIRE(l1 > l2);
    REQUIRE_FALSE(l2 > l1);
  }

  SECTION("Less Than or Equal To") {
    REQUIRE(l2 <= l1);
    REQUIRE(l1 <= l3);
  }

  SECTION("Greater Than or Equal To") {
    REQUIRE(l1 >= l2);
    REQUIRE(l1 >= l3);
  }

  static_assert(!kComparable<Length, Mass>, "Length and Mass should not be comparable");
}

TEST_CASE("Physical Calculations", "[quantity]") {
  const auto m = Mass{2.0};          // kg
  const auto g = Acceleration{9.8};  // m/s^2

  SECTION("Calculating Force") {
    const auto force = m * g;  // Should be Force
    REQUIRE(force.value == Approx(19.6));
    static_assert(std::is_same_v<decltype(force), const Force>, "Mass * Acceleration should be Force");
  }

  SECTION("Calculating Energy") {
    const auto h = Length{10.0};              // meters
    const auto potential_energy = m * g * h;  // Should be Energy
    REQUIRE(potential_energy.value == Approx(196.0));
    static_assert(std::is_same_v<decltype(potential_energy), const Energy>,
                  "Mass * Acceleration * Length should be Energy");
  }
}

TEST_CASE("Units Cancellation", "[quantity]") {
  const auto v = Speed{10.0};  // m/s
  const auto t = Time{5.0};    // s

  SECTION("Distance Calculation") {
    const auto d = v * t;  // Should be Length
    REQUIRE(d.value == Approx(50.0));
    static_assert(std::is_same_v<decltype(d), const Length>, "Speed * Time should be Length");
  }

  SECTION("Speed Calculation") {
    const auto l = Length{100.0};  // meters
    const auto speed = l / t;      // Should be Speed
    REQUIRE(speed.value == Approx(20.0));
    static_assert(std::is_same_v<decltype(speed), const Speed>, "Length / Time should be Speed");
  }
}

TEST_CASE("Chained Operations", "[quantity]") {
  const auto m = Mass{1.5};          // kg
  const auto l = Length{2.0};        // m
  const auto t = Time{4.0};          // s
  const auto g = Acceleration{9.8};  // m/s^2

  SECTION("Combined Calculation") {
    const auto result = (m * g * l) / t;  // Dimensions: [T=-3, L=2, M=1]
    REQUIRE(result.value == Approx(7.35));
    static_assert(std::is_same_v<decltype(result), const Quantity<-3, 2, 1>>,
                  "Result should have dimensions [T=-3, L=2, M=1]");
  }
}

TEST_CASE("Negative Values", "[quantity]") {
  const auto neg_length = Length{-5.0};
  REQUIRE(neg_length.value == Approx(-5.0));
}

#ifdef LITERALS_IMPLEMENTED

TEST_CASE("User Defined Literals", "[quantity]") {
  const auto l = 5.0_m;
  REQUIRE(l.value == Approx(5.0));
  static_assert(std::is_same_v<decltype(l), const Length>, "5.0_m should be Length");

  const auto m = 2.0_kg;
  REQUIRE(m.value == Approx(2.0));
  static_assert(std::is_same_v<decltype(m), const Mass>, "2.0_kg should be Mass");

  const auto t = 10.0_s;
  REQUIRE(t.value == Approx(10.0));
  static_assert(std::is_same_v<decltype(t), const Time>, "10.0_s should be Time");

  const auto s = -3.5_mps;
  REQUIRE(s.value == Approx(-3.5));
  static_assert(std::is_same_v<decltype(s), const Speed>, "-3.5_mps should be Speed");

  const auto g = 9.8_mps2;
  REQUIRE(g.value == Approx(9.8));
  static_assert(std::is_same_v<decltype(g), const Acceleration>, "9.8_mps2 should be Acceleration");

  const auto f = 19.6_N;
  REQUIRE(f.value == Approx(19.6));
  static_assert(std::is_same_v<decltype(f), const Force>, "19.6_N should be Force");

  const auto e = 196.0_J;
  REQUIRE(e.value == Approx(196.0));
  static_assert(std::is_same_v<decltype(e), const Energy>, "196.0_J should be Energy");
}

#endif  // LITERALS_IMPLEMENTED