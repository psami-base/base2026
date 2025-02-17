#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "string_view.hpp"
#include "string_view.hpp"  // Test include guard

#include <type_traits>

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
  const auto sv = StringView();
  Equal(sv, nullptr, 0);
}

TEST_CASE("From C String", "[StringView]") {
  static_assert(std::is_convertible_v<const char*, StringView>,
                "StringView must be implicitly constructible from const char*");

  const auto str = "aba";
  const auto sv = StringView(str);
  Equal(sv, str, 3);
}

TEST_CASE("From C Substring", "[StringView]") {
  const auto str = "abacaba";
  const auto sv = StringView(str + 2, 4);
  Equal(sv, str + 2, 4);
}

TEST_CASE("Copy", "[StringView]") {
  const auto str = "abacaba";
  const auto sv = StringView(str + 2, 4);
  const auto copy = sv;
  Equal(sv, str + 2, 4);
  Equal(copy, str + 2, 4);
}

TEST_CASE("Data access", "[StringView]") {
  const StringView sv = "abcdef";

  REQUIRE(sv.Front() == 'a');
  REQUIRE(sv.Back() == 'f');
  REQUIRE(sv[1] == 'b');

  static_assert(std::is_same_v<decltype(sv.Front()), const char&>, "Front() must return const char&");
  static_assert(std::is_same_v<decltype(sv.Back()), const char&>, "Back() must return const char&");
  static_assert(std::is_same_v<decltype(sv[0]), const char&>, "operator[] must return const char&");
}

TEST_CASE("RemovePrefix", "[StringView]") {
  const auto str = "some_test_string";
  auto sv = StringView(str);

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
  const auto str = "some_test_string";
  auto sv = StringView(str);

  sv.RemoveSuffix(5);
  Equal(sv, str, 11);

  sv.RemoveSuffix(0);
  Equal(sv, str, 11);

  sv.RemoveSuffix(4);
  Equal(sv, str, 7);

  sv.RemoveSuffix(7);
  Equal(sv, str, 0);
}

TEST_CASE("Substr", "[StringView]") {
  const auto str = "some_test_string";
  const auto sv = StringView(str);

  Equal(sv.Substr(0, 5), str, 5);
  Equal(sv, str, 16);

  Equal(sv.Substr(0, 16), str, 16);
  Equal(sv, str, 16);

  Equal(sv.Substr(2, 7), str + 2, 7);
  Equal(sv, str, 16);

  Equal(sv.Substr(7, 9), str + 7, 9);
  Equal(sv, str, 16);
}

TEST_CASE("Swap", "[StringView]") {
  const auto str = "some_test_string";
  auto sv = StringView(str);

  sv.Substr(2, 7).Swap(sv);
  Equal(sv, str + 2, 7);
}
