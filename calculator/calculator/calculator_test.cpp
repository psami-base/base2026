#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "calculator.hpp"
#include "parser.hpp"
#include "calculator.hpp"  // check include guards
#include "parser.hpp"      // check include guards

TEST_CASE("Constant", "[ParseFactor]") {
  const auto tokens = Tokenize("4");
  size_t pos = 0;
  const auto expression = ParseFactor(tokens, pos);

  REQUIRE(pos == 1);
  REQUIRE(expression->Calculate() == 4);
}

TEST_CASE("Multipliers", "[ParseTerm]") {
  SECTION("One Multiplier") {
    const auto tokens = Tokenize("4");
    size_t pos = 0;
    const auto expression = ParseTerm(tokens, pos);

    REQUIRE(pos == 1);
    REQUIRE(expression->Calculate() == 4);
  }

  SECTION("Two Multipliers") {
    const auto tokens = Tokenize("   4*   2 ");
    size_t pos = 0;
    const auto expression = ParseTerm(tokens, pos);

    REQUIRE(pos == 3);
    REQUIRE(expression->Calculate() == 8);
  }

  SECTION("Three Multipliers") {
    const auto tokens = Tokenize("4 /2   *3 ");
    size_t pos = 0;
    const auto expression = ParseTerm(tokens, pos);

    REQUIRE(pos == 5);
    REQUIRE(expression->Calculate() == 6);
  }

  SECTION("Four Multipliers") {
    const auto tokens = Tokenize("4/2*3%5");
    size_t pos = 0;
    const auto expression = ParseTerm(tokens, pos);

    REQUIRE(pos == 7);
    REQUIRE(expression->Calculate() == 1);
  }
}

TEST_CASE("Addendums", "[ParseExpression]") {
  SECTION("One Addendum") {
    const auto tokens = Tokenize("4 / -2 * 3");
    size_t pos = 0;
    const auto expression = ParseExpression(tokens, pos);

    REQUIRE(pos == 6);
    REQUIRE(expression->Calculate() == -6);
  }

  SECTION("Two Addendums") {
    const auto tokens = Tokenize("4 / -2*3 + 5");
    size_t pos = 0;
    const auto expression = ParseExpression(tokens, pos);

    REQUIRE(pos == 8);
    REQUIRE(expression->Calculate() == -1);
  }

  SECTION("Three Addendums") {
    const auto tokens = Tokenize("4 / -2*3 + 5 - 64%9");
    size_t pos = 0;
    const auto expression = ParseExpression(tokens, pos);

    REQUIRE(pos == 12);
    REQUIRE(expression->Calculate() == -2);
  }
}

TEST_CASE("Expression", "[ParseFactor]") {
  {
    const auto tokens = Tokenize("  (4/ -2 *  3   )  ");
    size_t pos = 0;
    const auto expression = ParseFactor(tokens, pos);

    REQUIRE(pos == 8);
    REQUIRE(expression->Calculate() == -6);
  }

  {
    const auto tokens = Tokenize("(4/((-2)*3)+5)");
    size_t pos = 0;
    const auto expression = ParseFactor(tokens, pos);

    REQUIRE(pos == 14);
    REQUIRE(expression->Calculate() == 5);
  }

  {
    const auto tokens = Tokenize(" ( 4 / ( -2 ) * ( 3 + (5) ) - 64%9 )");
    size_t pos = 0;
    const auto expression = ParseFactor(tokens, pos);

    REQUIRE(pos == 20);
    REQUIRE(expression->Calculate() == -17);
  }
}

TEST_CASE("UseCases", "[Calculator]") {
  REQUIRE(CalculateExpression("  11 ") == 11);
  REQUIRE(CalculateExpression("+5") == 5);
  REQUIRE(CalculateExpression("5 + +5") == 10);
  REQUIRE(CalculateExpression(" ( ( (   ( - 100  )))  ) ") == -100);
  REQUIRE(CalculateExpression("5*-4") == -20);
  REQUIRE(CalculateExpression(" ( (10 ) / 5 ) ") == 2);
  REQUIRE(CalculateExpression(" 10 +-4 - 5 %4* 7") == -1);
  REQUIRE(CalculateExpression("((1+(2+(3+(4+(5)))))%((10)))") == 5);
  REQUIRE(CalculateExpression(" ( 5 + 3 ) * ( -5 - -7 )") == 16);
  REQUIRE(CalculateExpression(" 1*((( 2* 3 )* ( 4 +5 ) +6+7 ) * (8 * 9+ 10*11 )*12 )  ") == 146'328);
}

TEST_CASE("Exceptions", "[Calculator]") {
  REQUIRE_THROWS_AS((void)CalculateExpression(""), WrongExpressionError);                       // NOLINT
  REQUIRE_THROWS_AS((void)CalculateExpression("("), WrongExpressionError);                      // NOLINT
  REQUIRE_THROWS_AS((void)CalculateExpression("( )"), WrongExpressionError);                    // NOLINT
  REQUIRE_THROWS_AS((void)CalculateExpression("5 5"), WrongExpressionError);                    // NOLINT
  REQUIRE_THROWS_AS((void)CalculateExpression("( 10 - 5 ) ( 5 * 10 )"), WrongExpressionError);  // NOLINT
  REQUIRE_THROWS_AS((void)CalculateExpression("( 10 - 5 + ( 5 * 10 )"), WrongExpressionError);  // NOLINT
  REQUIRE_THROWS_AS((void)CalculateExpression("10 * 8 - 9 ! 9"), std::runtime_error);           // NOLINT
}
