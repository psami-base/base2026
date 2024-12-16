#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include "rational.h"
#include "rational.h"  // check include guards

void RationalEqual(const Rational& rational, const int64_t numerator, const int64_t denominator) {
  REQUIRE(rational.GetNumerator() == numerator);
  REQUIRE(rational.GetDenominator() == denominator);
}

TEST_CASE("Constructors", "[Rational]") {

  SECTION("Default Constructor") {
    const auto r = Rational();
    RationalEqual(r, 0, 1);
  }

  SECTION("Converting Constructor") {
    const auto r = Rational(2);
    RationalEqual(r, 2, 1);

    const Rational q = -2;
    RationalEqual(q, -2, 1);
  }

  SECTION("Parametric Constructor") {
    const auto x = Rational(0, 4);
    RationalEqual(x, 0, 1);

    const auto y = Rational(0, -2);
    RationalEqual(y, 0, 1);

    const auto r = Rational(-2, 3);
    RationalEqual(r, -2, 3);

    const auto p = Rational(-2, -4);
    RationalEqual(p, 1, 2);

    const auto q = Rational{27, -9};
    RationalEqual(q, -3, 1);

    const auto s = Rational{-36, 24};
    RationalEqual(s, -3, 2);
  }
}

TEST_CASE("GettersAndSetters", "[Rational]") {

  SECTION("From Default") {
    auto r = Rational();

    r.SetDenominator(2);
    RationalEqual(r, 0, 1);

    r.SetNumerator(-3);
    RationalEqual(r, -3, 1);

    r.SetDenominator(-4);
    RationalEqual(r, 3, 4);

    r.SetNumerator(14);
    RationalEqual(r, 7, 2);

    r.SetDenominator(-49);
    RationalEqual(r, -1, 7);
  }

  SECTION("From Fractional") {
    auto r = Rational(3, -9);

    r.SetDenominator(2);
    RationalEqual(r, -1, 2);

    r.SetNumerator(-3);
    RationalEqual(r, -3, 2);

    r.SetDenominator(-4);
    RationalEqual(r, 3, 4);

    r.SetNumerator(14);
    RationalEqual(r, 7, 2);

    r.SetDenominator(-49);
    RationalEqual(r, -1, 7);
  }
}

TEST_CASE("Assignment", "[Rational]") {
  SECTION("+=") {
    auto r = Rational(1, 3);
    auto q = Rational{1, 6};

    r += q;
    RationalEqual(r, 1, 2);
    RationalEqual(q, 1, 6);

    (r += std::as_const(q)) = {-13, 3};
    RationalEqual(r, -13, 3);
    RationalEqual(q, 1, 6);
  }

  SECTION("-=") {
    auto r = Rational{2, 3};
    auto q = Rational{1, 6};

    r -= q;
    RationalEqual(r, 1, 2);
    RationalEqual(q, 1, 6);

    (r -= std::as_const(q)) = {-14, 3};
    RationalEqual(r, -14, 3);
    RationalEqual(q, 1, 6);
  }

  SECTION("*=") {
    auto r = Rational{-2, 3};
    const auto positive = Rational{5, 6};
    const auto negative = Rational{-3, 20};
    const auto zero = Rational{};

    r *= positive;
    RationalEqual(r, -5, 9);

    r *= negative;
    RationalEqual(r, 1, 12);

    r *= zero;
    RationalEqual(r, 0, 1);

    r *= positive;
    RationalEqual(r, 0, 1);

    r *= negative;
    RationalEqual(r, 0, 1);

    (r *= r) = {7, 8};
    RationalEqual(r, 7, 8);
  }

  SECTION("/=") {
    auto r = Rational{-2, 3};
    const auto positive = Rational{6, 5};
    const auto negative = Rational{-20, 3};

    r /= positive;
    RationalEqual(r, -5, 9);

    r /= negative;
    RationalEqual(r, 1, 12);

    (r /= r) = {7, 8};
    RationalEqual(r, 7, 8);
  }
}

TEST_CASE("Arithmetic", "[Rational]") {
  SECTION("Unary +") {
    const auto positive = Rational{4, 7};
    const auto negative = Rational{-7, 4};
    const auto zero = Rational{};

    RationalEqual(+positive, 4, 7);
    RationalEqual(+negative, -7, 4);
    RationalEqual(+zero, 0, 1);
  }

  SECTION("Unary -") {
    const auto positive = Rational{4, 7};
    const auto negative = Rational{-7, 4};
    const auto zero = Rational{};

    RationalEqual(-positive, -4, 7);
    RationalEqual(positive, 4, 7);
    RationalEqual(-negative, 7, 4);
    RationalEqual(negative, -7, 4);
    RationalEqual(-zero, 0, 1);
  }

  SECTION("Binary +") {
    const auto r = Rational{1, 14};
    const auto q = Rational{-5, 18};

    RationalEqual(r + q, -13, 63);
    RationalEqual(q + r, -13, 63);

    RationalEqual(r + Rational{-1, 7}, -1, 14);
    RationalEqual(Rational{6, 13} + q, 43, 234);

    RationalEqual(Rational{2, 3} + Rational{-2, 3}, 0, 1);
    RationalEqual(Rational{2, 3} + Rational{2, 3}, 4, 3);
  }

  SECTION("Binary -") {
    const auto r = Rational{1, 14};
    const auto q = Rational{-5, 18};

    RationalEqual(r - q, 22, 63);
    RationalEqual(q - r, -22, 63);

    RationalEqual(q - Rational{-1, 7}, -17, 126);
    RationalEqual(Rational{6, 13} - r, 71, 182);

    RationalEqual(Rational{2, 3} - Rational{2, 3}, 0, 1);
    RationalEqual(Rational{2, 3} - Rational{-2, 3}, 4, 3);
  }

  SECTION("Binary *") {
    const auto positive = Rational{18, 35};
    const auto negative = Rational{-5, 3};
    const auto zero = Rational{};

    RationalEqual(positive * negative, -6, 7);
    RationalEqual(negative * positive, -6, 7);

    RationalEqual(positive * zero, 0, 1);
    RationalEqual(zero * negative, 0, 1);

    RationalEqual(positive * Rational{13, 11}, 234, 385);
    RationalEqual(Rational{-7, 11} * negative, 35, 33);

    RationalEqual(Rational{2, 3} * Rational{3, 2}, 1, 1);
    RationalEqual(Rational{-1, 1} * Rational{-1, 1}, 1, 1);
  }

  SECTION("Binary /") {
    const auto positive = Rational{18, 35};
    const auto negative = Rational{-3, 5};
    const auto zero = Rational{};

    RationalEqual(positive / negative, -6, 7);
    RationalEqual(negative / positive, -7, 6);

    RationalEqual(zero / positive, 0, 1);
    RationalEqual(zero / negative, 0, 1);

    RationalEqual(positive / Rational{7, 11}, 198, 245);
    RationalEqual(Rational{-7, 11} / negative, 35, 33);

    RationalEqual(Rational{2, 3} / Rational{2, 3}, 1, 1);
    RationalEqual(Rational{-1, 1} / Rational{-1, 1}, 1, 1);
  }

  SECTION("Prefix ++") {
    auto r = Rational{-1, 2};

    ++r;
    RationalEqual(r, 1, 2);
    RationalEqual(++r, 3, 2);
    ++r = -1;
    RationalEqual(r, -1, 1);
  }

  SECTION("Postfix ++") {
    auto r = Rational{-1, 2};

    r++;
    RationalEqual(r, 1, 2);
    RationalEqual(r++, 1, 2);
    RationalEqual(r, 3, 2);
  }

  SECTION("Prefix --") {
    auto r = Rational{1, 2};

    --r;
    RationalEqual(r, -1, 2);
    RationalEqual(--r, -3, 2);
    --r = -1;
    RationalEqual(r, -1, 1);
  }

  SECTION("Postfix --") {
    auto r = Rational{1, 2};

    r--;
    RationalEqual(r, -1, 2);
    RationalEqual(r--, -1, 2);
    RationalEqual(r, -3, 2);
  }
}

template <class T>
void CheckComparisonEqual(const T& lhs, const T& rhs) {
  REQUIRE(lhs == rhs);
  REQUIRE(lhs <= rhs);
  REQUIRE(lhs >= rhs);
  REQUIRE_FALSE(lhs != rhs);
  REQUIRE_FALSE(lhs < rhs);
  REQUIRE_FALSE(lhs > rhs);
}

template <class T>
void CheckComparisonLess(const T& lhs, const T& rhs) {
  REQUIRE_FALSE(lhs == rhs);
  REQUIRE(lhs <= rhs);
  REQUIRE_FALSE(lhs >= rhs);
  REQUIRE(lhs != rhs);
  REQUIRE(lhs < rhs);
  REQUIRE_FALSE(lhs > rhs);
}

template <class T>
void CheckComparisonGreater(const T& lhs, const T& rhs) {
  REQUIRE_FALSE(lhs == rhs);
  REQUIRE_FALSE(lhs <= rhs);
  REQUIRE(lhs >= rhs);
  REQUIRE(lhs != rhs);
  REQUIRE_FALSE(lhs < rhs);
  REQUIRE(lhs > rhs);
}

TEST_CASE("Comparisons", "[Rational]") {
  const auto p_4_9 = Rational{4, 9};
  const auto p_5_8 = Rational{5, 8};
  const auto n_4_9 = Rational{-4, 9};
  const auto n_5_8 = Rational{-5, 8};
  const auto zero = Rational{};
  const auto one = Rational{1};

  CheckComparisonLess(p_4_9, one);
  CheckComparisonGreater(one, p_4_9);

  CheckComparisonLess(p_4_9, p_5_8);
  CheckComparisonGreater(p_5_8, p_4_9);

  CheckComparisonLess(n_5_8, n_4_9);
  CheckComparisonGreater(n_4_9, n_5_8);

  CheckComparisonEqual(n_4_9, n_4_9);
  CheckComparisonEqual(p_5_8, p_5_8);

  CheckComparisonLess(n_5_8, p_4_9);
  CheckComparisonGreater(p_4_9, n_5_8);

  CheckComparisonLess(zero, p_4_9);
  CheckComparisonGreater(p_4_9, zero);

  CheckComparisonLess(n_5_8, zero);
  CheckComparisonGreater(zero, n_5_8);

  CheckComparisonEqual(zero, zero);

  REQUIRE(n_4_9 < 0);
  REQUIRE_FALSE(0 > p_4_9);
}

TEST_CASE("IO", "[Rational]") {

  SECTION("Input Rational") {
    auto ss = std::stringstream("-7/3 4/6 -4/-8 0/4 +5/-3 -3/5");
    auto p = Rational{};
    auto q = Rational{};

    ss >> p >> q;
    RationalEqual(p, -7, 3);
    RationalEqual(q, 2, 3);

    ss >> p >> q;
    RationalEqual(p, 1, 2);
    RationalEqual(q, 0, 1);

    ss >> p >> q;
    RationalEqual(p, -5, 3);
    RationalEqual(q, -3, 5);
  }

  SECTION("Input Integer") {
    auto ss = std::stringstream("7 -4 0 -0 +1");
    auto p = Rational{};
    auto q = Rational{};

    ss >> p >> q;
    RationalEqual(p, 7, 1);
    RationalEqual(q, -4, 1);

    ss >> p >> q;
    RationalEqual(p, 0, 1);
    RationalEqual(q, 0, 1);

    ss >> p;
    RationalEqual(p, 1, 1);
  }

  SECTION("Output") {
    auto ss = std::stringstream();

    ss << Rational{} << ' ' << Rational{4} << ' ' << Rational{-5} << ' ' << Rational{3, 5} << ' ' << Rational{-3, 6}
       << ' ' << Rational{-8, 6} << ' ' << Rational{-4, -6};

    REQUIRE(ss.str() == "0 4 -5 3/5 -1/2 -4/3 2/3");
  }
}

TEST_CASE("DivisionByZero", "[Rational]") {

  SECTION("on construction") {
    REQUIRE_THROWS_AS(Rational(11, 0), RationalDivisionByZero);  // NOLINT
  }

  SECTION("on division") {
    auto r = Rational{3, 5};
    REQUIRE_THROWS_AS((r /= 0), RationalDivisionByZero);                    // NOLINT
    REQUIRE_THROWS_AS((void)(Rational{8, 7} / 0), RationalDivisionByZero);  // NOLINT
  }

  SECTION("on setter") {
    auto r = Rational{3, 5};
    REQUIRE_THROWS_AS(r.SetDenominator(0), RationalDivisionByZero);  // NOLINT
  }

  SECTION("on input") {
    auto ss = std::stringstream("1/0");
    auto r = Rational{};

    REQUIRE_THROWS_AS((ss >> r), RationalDivisionByZero);  // NOLINT
  }
}
