#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "word_generator.hpp"
#include "word_generator.hpp"

#include <filesystem>
#include <fstream>
#include <string>

namespace {

std::filesystem::path MakeTempPath(const std::string_view suffix) {
  return std::filesystem::temp_directory_path() / ("word_generator_test_" + std::string(suffix));
}

void WriteFile(const std::filesystem::path& path, const std::string_view content) {
  auto out = std::ofstream(path);
  REQUIRE(out.is_open());
  out << content;
  out.close();
}

}  // namespace

TEST_CASE("Loads alphabetic words and normalizes to lowercase", "[WordGenerator]") {
  const auto words_file = MakeTempPath("normalize.txt");
  WriteFile(words_file, "HELLO\nworld\nCPlusPlus\nabc123\n42\n");

  const auto cleanup = [&words_file] { std::filesystem::remove(words_file); };

  auto generator = WordGenerator(words_file, 123);
  const auto generated_words = std::vector<std::string_view>{"hello", "world", "cplusplus", "cplusplus", "hello"};
  for (int i = 0; i < 5; ++i) {
    REQUIRE(generator.GetRandomWord() == generated_words[i]);
  }

  cleanup();
}

TEST_CASE("Throws when file cannot be opened", "[WordGenerator]") {
  REQUIRE_THROWS_AS(WordGenerator("/why/did/you/create/such/strange/path/for/words.txt", 1), std::runtime_error);
}

TEST_CASE("Throws when file has no valid words", "[WordGenerator]") {
  const auto words_file = MakeTempPath("empty_valid.txt");
  WriteFile(words_file, "123\n_-_\n9999\n");

  const auto cleanup = [&words_file] { std::filesystem::remove(words_file); };

  REQUIRE_THROWS_AS(WordGenerator(words_file, 1), std::runtime_error);

  cleanup();
}

TEST_CASE("Same seed produces deterministic random sequence", "[WordGenerator]") {
  const auto words_file = MakeTempPath("seed.txt");
  WriteFile(words_file, "alpha\nbeta\ngamma\ndelta\n");

  const auto cleanup = [&words_file] { std::filesystem::remove(words_file); };

  auto lhs = WordGenerator(words_file, 2026);
  auto rhs = WordGenerator(words_file, 2026);

  for (int i = 0; i < 20; ++i) {
    REQUIRE(lhs.GetRandomWord() == rhs.GetRandomWord());
  }

  cleanup();
}

TEST_CASE("LoadWordsFromFile replaces words on success", "[WordGenerator]") {
  const auto initial_file = MakeTempPath("reload_initial.txt");
  const auto updated_file = MakeTempPath("reload_updated.txt");
  WriteFile(initial_file, "alpha\nbeta\n");
  WriteFile(updated_file, "DOG\ncat\n123\n");

  const auto cleanup = [&initial_file, &updated_file] {
    std::filesystem::remove(initial_file);
    std::filesystem::remove(updated_file);
  };

  auto generator = WordGenerator(initial_file, 11);
  generator.LoadWordsFromFile(updated_file);

  for (int i = 0; i < 20; ++i) {
    const auto word = generator.GetRandomWord();
    REQUIRE((word == "dog" || word == "cat"));
  }

  cleanup();
}

TEST_CASE("LoadWordsFromFile has strong exception guarantee on open failure", "[WordGenerator]") {
  const auto initial_file = MakeTempPath("strong_open_initial.txt");
  WriteFile(initial_file, "alpha\nbeta\ngamma\n");

  const auto cleanup = [&initial_file] { std::filesystem::remove(initial_file); };

  auto generator = WordGenerator(initial_file, 777);
  auto control = WordGenerator(initial_file, 777);

  REQUIRE_THROWS_AS(generator.LoadWordsFromFile("/definitely/nonexistent/reload_words.txt"), std::runtime_error);

  for (int i = 0; i < 30; ++i) {
    REQUIRE(generator.GetRandomWord() == control.GetRandomWord());
  }

  cleanup();
}

TEST_CASE("LoadWordsFromFile has strong exception guarantee on invalid content", "[WordGenerator]") {
  const auto initial_file = MakeTempPath("strong_invalid_initial.txt");
  const auto invalid_file = MakeTempPath("strong_invalid_bad.txt");
  WriteFile(initial_file, "alpha\nbeta\ngamma\n");
  WriteFile(invalid_file, "123\n---\n456\n");

  const auto cleanup = [&initial_file, &invalid_file] {
    std::filesystem::remove(initial_file);
    std::filesystem::remove(invalid_file);
  };

  auto generator = WordGenerator(initial_file, 888);
  auto control = WordGenerator(initial_file, 888);

  REQUIRE_THROWS_AS(generator.LoadWordsFromFile(invalid_file), std::runtime_error);

  for (int i = 0; i < 30; ++i) {
    REQUIRE(generator.GetRandomWord() == control.GetRandomWord());
  }

  cleanup();
}
