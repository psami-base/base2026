#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <type_traits>

#include "string_view.h"
#include "string_view.h"

void Equal(const StringView& actual, const char* expected, size_t n) {
  REQUIRE(actual.Data() == expected);
  REQUIRE(actual.Size() == n);
  REQUIRE(actual.Length() == n);
  REQUIRE(actual.Empty() == (n == 0));
  for (size_t i = 0; i < n; ++i) {
    REQUIRE(actual[i] == expected[i]);
  }
}

TEST_CASE("Default Constructor", "[StringView]") {
  const StringView sv;
  Equal(sv, nullptr, 0);
}

TEST_CASE("From C String", "[StringView]") {
  const char* str = "aba";
  const StringView sv = str;
  Equal(sv, str, 3);
}

TEST_CASE("From C Substring", "[StringView]") {
  const char* str = "abacaba";
  const StringView sv(str + 2, 4);
  Equal(sv, str + 2, 4);
}

TEST_CASE("Copy", "[StringView]") {
  const char* str = "abacaba";
  const StringView sv(str + 2, 4);
  const auto copy = sv;
  Equal(sv, str + 2, 4);
  Equal(copy, str + 2, 4);
}

TEST_CASE("Data access", "[StringView]") {
  static_assert(!std::is_assignable_v<decltype(std::declval<StringView&>().Front()), char>);
  static_assert(!std::is_assignable_v<decltype(std::declval<StringView&>().Back()), char>);
  static_assert(!std::is_assignable_v<decltype(std::declval<StringView&>()[0]), char>);

  const StringView sv = "abcdef";

  REQUIRE(sv.Front() == 'a');
  REQUIRE(sv.Back() == 'f');
  REQUIRE(sv[1] == 'b');
}

TEST_CASE("RemovePrefix", "[StringView]") {
  const char* str = "some_test_string";
  StringView sv = str;

  sv.RemovePrefix(5);
  Equal(sv, str + 5, 11);

  sv.RemovePrefix(0);
  Equal(sv, str + 5, 11);

  sv.RemovePrefix(4);
  Equal(sv, str + 9, 7);

  sv.RemovePrefix(7);
  Equal(sv, str + 16, 0);
}

TEST_CASE("RemoveSuffix", "[StringView]") {
  const char* str = "some_test_string";
  StringView sv = str;

  sv.RemoveSuffix(5);
  Equal(sv, str, 11);

  sv.RemoveSuffix(0);
  Equal(sv, str, 11);

  sv.RemoveSuffix(4);
  Equal(sv, str, 7);

  sv.RemoveSuffix(7);
  Equal(sv, str, 0);
}

TEST_CASE("Substr", "[Modifications]") {
  const char* str = "some_test_string";
  StringView sv = str;

  Equal(sv.Substr(0, 5), str, 5);
  Equal(sv, str, 16);

  Equal(sv.Substr(0, 16), str, 16);
  Equal(sv, str, 16);

  Equal(sv.Substr(2, 7), str + 2, 7);
  Equal(sv, str, 16);

  Equal(sv.Substr(7, 9), str + 7, 9);
  Equal(sv, str, 16);
}

TEST_CASE("Swap", "[Modifications]") {
  const char* str = "some_test_string";
  StringView sv = str;

  sv.Substr(2, 7).Swap(sv);
  Equal(sv, str + 2, 7);
}
