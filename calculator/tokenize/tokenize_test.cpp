#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "tokenize.hpp"
#include "tokenize.hpp"  // check include guards

void Equal(const std::vector<Token>& fact, const std::vector<Token>& expected) {
  REQUIRE(fact.size() == expected.size());
  for (size_t i = 0; i < fact.size(); ++i) {
    REQUIRE(fact[i] == expected[i]);
  }
}

TEST_CASE("Empty", "[Tokenize]") {
  Equal(Tokenize(""), {});
  Equal(Tokenize("    "), {});
}

TEST_CASE("Basic", "[Tokenize]") {
  Equal(Tokenize("+"), {PlusToken{}});
  Equal(Tokenize("   +  "), {PlusToken{}});
  Equal(Tokenize("+ +"), {PlusToken{}, PlusToken{}});
  Equal(Tokenize("   +   +   "), {PlusToken{}, PlusToken{}});
  Equal(Tokenize("  +  - *   / %  min max   abs (   sqr 1 )     "),
        {PlusToken{}, MinusToken{}, MultiplyToken{}, DivideToken{}, ResidualToken{}, MinToken{}, MaxToken{}, AbsToken{},
         OpeningBracketToken{}, SqrToken{}, NumberToken{1}, ClosingBracketToken{}});
}

TEST_CASE("Unknown", "[Tokenize]") {
  REQUIRE_FALSE(Token(UnknownToken{"a"}) == Token(UnknownToken{"c"}));
  REQUIRE(Token(UnknownToken{"abacaba"}) == Token(UnknownToken{"abacaba"}));
  REQUIRE(UnknownToken{"abacaba"}.value == "abacaba");

  Equal(Tokenize("  aba caba  min     "), {UnknownToken{"aba"}, UnknownToken{"caba"}, MinToken()});
  Equal(Tokenize(" ++ -- *+ absmin minmax *3 sqr1 "),
        {PlusToken{}, PlusToken{}, MinusToken{}, MinusToken{}, MultiplyToken{}, PlusToken{}, UnknownToken{"absmin"},
         UnknownToken{"minmax"}, MultiplyToken{}, NumberToken{3}, SqrToken{}, NumberToken{1}});
  Equal(Tokenize(" Min MAX AbS +  sqR  abs   "),
        {UnknownToken{"Min"}, UnknownToken{"MAX"}, UnknownToken{"AbS"}, PlusToken(), UnknownToken{"sqR"}, AbsToken()});
}

TEST_CASE("Numbers", "[Tokenize]") {
  REQUIRE_FALSE(Token(NumberToken{11}) == Token(NumberToken{42}));
  REQUIRE(Token(NumberToken{11}) == Token(NumberToken{11}));
  REQUIRE(NumberToken{11}.value == 11);

  Equal(Tokenize("  123 1  2 3 12  "),
        {NumberToken{123}, NumberToken{1}, NumberToken{2}, NumberToken{3}, NumberToken{12}});
  Equal(Tokenize("  +11  -33 -0  +0  11+  12- 23-4 1+1-1 "),
        {PlusToken{},    NumberToken{11}, MinusToken{}, NumberToken{33}, MinusToken{}, NumberToken{0},  PlusToken{},
         NumberToken{0}, NumberToken{11}, PlusToken{},  NumberToken{12}, MinusToken{}, NumberToken{23}, MinusToken{},
         NumberToken{4}, NumberToken{1},  PlusToken{},  NumberToken{1},  MinusToken{}, NumberToken{1}});
  Equal(Tokenize(" + 11 - 12 + +11 - -12"),
        {PlusToken{}, NumberToken{11}, MinusToken{}, NumberToken{12}, PlusToken{}, PlusToken{}, NumberToken{11},
         MinusToken{}, MinusToken{}, NumberToken{12}});
}
