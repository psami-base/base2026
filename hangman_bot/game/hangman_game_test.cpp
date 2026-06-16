#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "hangman.hpp"
#include "hangman.hpp"

TEST_CASE("Constructor initializes masked word", "[HangmanGame]") {
  const auto game = HangmanGame("robot");

  REQUIRE(game.GetWord() == "robot");
  REQUIRE(game.GetGuess() == "*****");
  REQUIRE(game.GetWrongGuesses() == 0);
  REQUIRE_FALSE(game.IsWin());
  REQUIRE_FALSE(game.IsLose());
}

TEST_CASE("Guess reveals all matching letters", "[HangmanGame]") {
  auto game = HangmanGame("letter");

  REQUIRE(game.Guess('e'));
  REQUIRE(game.GetGuess() == "*e**e*");
  REQUIRE(game.GetWrongGuesses() == 0);
}

TEST_CASE("Wrong guess increments counter", "[HangmanGame]") {
  auto game = HangmanGame("robot");

  REQUIRE_FALSE(game.Guess('x'));
  REQUIRE(game.GetGuess() == "*****");
  REQUIRE(game.GetWrongGuesses() == 1);
}

TEST_CASE("Game reaches win and lose states", "[HangmanGame]") {
  auto winning_game = HangmanGame("aba");
  REQUIRE(winning_game.Guess('a'));
  REQUIRE(winning_game.Guess('b'));
  REQUIRE(winning_game.IsWin());
  REQUIRE_FALSE(winning_game.IsLose());
  REQUIRE(winning_game.GetGuess() == "aba");

  auto losing_game = HangmanGame("aba");
  for (const char letter : {'c', 'd', 'e', 'f', 'g', 'h', 'i'}) {
    REQUIRE_FALSE(losing_game.Guess(letter));
  }
  REQUIRE(losing_game.IsLose());
  REQUIRE_FALSE(losing_game.IsWin());
  REQUIRE(losing_game.GetGuess() == "***");
}
