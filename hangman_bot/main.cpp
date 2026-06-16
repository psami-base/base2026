#include "bot/bot.hpp"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <random>

int main(int argc, char* argv[]) {
  if (argc < 4) {
    std::cerr << "Usage: " << argv[0] << " <telegram_token_path> <words_path> <state_path> [seed]\n";
    return 1;
  }
  const auto seed = argc >= 5 ? std::stoull(argv[4]) : std::random_device{}();
  auto token = std::string();
  {
    auto token_istream = std::ifstream(argv[1]);
    if (!token_istream.is_open()) {
      std::cerr << "Failed to open Telegram API token file: " << argv[1] << '\n';
      return 1;
    }
    std::getline(token_istream, token);
    if (token.empty()) {
      std::cerr << "Telegram API token file is empty: " << argv[1] << '\n';
      return 1;
    }
  }
  auto bot = HangmanTgBot(token, argv[2], argv[3], seed);
  bot.Run();
  return 0;
}
