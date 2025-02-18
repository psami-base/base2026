#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "string.hpp"
#include "string.hpp"  // check include guards

#include <string_view>
#include <utility>

void CheckEqual(const String& actual, const std::string_view expected) {
  REQUIRE(actual.Capacity() >= actual.Size());
  REQUIRE(actual.Size() == expected.size());
  REQUIRE(actual.Length() == expected.size());
  for (size_t i = 0; i < expected.size(); ++i) {
    REQUIRE(actual[i] == expected[i]);
  }
}

TEST_CASE("Default Constructor", "[String]") {
  const auto s = String();
  CheckEqual(s, "");
  REQUIRE(s.Data() == nullptr);
  REQUIRE(s.Empty());
}

TEST_CASE("FillInitialization", "[String]") {
  {
    const auto s = String(0, 'a');
    REQUIRE(s.Size() == 0u);
    REQUIRE(s.Capacity() == 0u);
    REQUIRE(s.Data() == nullptr);
  }

  {
    const auto s = String(5, 'a');
    CheckEqual(s, "aaaaa");
  }
}

TEST_CASE("From C String", "[String]") {
  {
    const String s = "abacaba";
    CheckEqual(s, "abacaba");
  }

  {
    const String s = "";
    REQUIRE(s.Capacity() == 0u);
    CheckEqual(s, "");
  }

  {
    const auto s = String("abacaba", 5u);
    CheckEqual(s, "abaca");
  }

  {
    const auto s = String("abacaba", 0u);
    REQUIRE(s.Capacity() == 0u);
    CheckEqual(s, "");
  }
}

TEST_CASE("Copy Constructor", "[String]") {
  {
    const auto empty = String();
    const auto copy = empty;
    REQUIRE(empty.Size() == 0u);
    REQUIRE(empty.Capacity() == 0u);
    REQUIRE(empty.Data() == nullptr);
    REQUIRE(copy.Size() == 0u);
    REQUIRE(copy.Capacity() == 0u);
    REQUIRE(copy.Data() == nullptr);
  }

  {
    const auto s = String("abacaba");
    const auto copy = s;
    CheckEqual(copy, "abacaba");
    CheckEqual(s, "abacaba");
    REQUIRE(copy.Data() != s.Data());
  }
}

TEST_CASE("Copy Assignment", "[String]") {
  SECTION("Empty to empty") {
    const auto empty = String();
    auto s = String();
    s = empty;
    REQUIRE(empty.Size() == 0u);
    REQUIRE(empty.Capacity() == 0u);
    REQUIRE(empty.Data() == nullptr);
    REQUIRE(s.Size() == 0u);
    REQUIRE(s.Capacity() == 0u);
    REQUIRE(s.Data() == nullptr);

    s = s;
    REQUIRE(s.Size() == 0u);
    REQUIRE(s.Capacity() == 0u);
    REQUIRE(s.Data() == nullptr);
  }

  SECTION("Empty to filled") {
    const auto empty = String();
    auto s = String("abacaba");
    s = empty;
    REQUIRE(empty.Size() == 0u);
    REQUIRE(empty.Capacity() == 0u);
    REQUIRE(empty.Data() == nullptr);
    REQUIRE(s.Size() == 0u);
  }

  SECTION("Filled to empty") {
    const auto init = String("abacaba");
    auto s = String();
    s = init;
    CheckEqual(s, "abacaba");
    CheckEqual(init, "abacaba");
    REQUIRE(s.Data() != init.Data());

    s = s;
    CheckEqual(s, "abacaba");
    CheckEqual(init, "abacaba");
  }

  SECTION("Small to large") {
    const auto small = String(10, 'a');
    auto large = String(1000, 'b');
    large = small;
    CheckEqual(large, std::string(10, 'a'));
    CheckEqual(small, std::string(10, 'a'));
    REQUIRE(large.Data() != small.Data());
  }

  SECTION("Large to small") {
    const auto large = String(1000, 'b');
    auto small = String(10, 'a');
    small = large;
    CheckEqual(large, std::string(1000, 'b'));
    CheckEqual(small, std::string(1000, 'b'));
    REQUIRE(large.Data() != small.Data());
  }
}

TEST_CASE("Data Access", "[String]") {
  auto s = String("abcdef");

  {
    REQUIRE(s.Front() == 'a');
    s.Front() = 'z';
    REQUIRE(std::as_const(s).Front() == 'z');
    static_assert(std::is_same_v<decltype(std::as_const(s).Front()), const char&>,
                  "Front() const must return const reference");
  }

  {
    REQUIRE(s.Back() == 'f');
    s.Back() = 'y';
    REQUIRE(std::as_const(s).Back() == 'y');
    static_assert(std::is_same_v<decltype(std::as_const(s).Back()), const char&>,
                  "Back() const must return const reference");
  }

  {
    REQUIRE(s[1] == 'b');
    s[1] = 'x';
    REQUIRE(std::as_const(s)[1] == 'x');
    static_assert(std::is_same_v<decltype(std::as_const(s)[1]), const char&>,
                  "operator[] const must return const reference");
  }

  {
    REQUIRE(std::as_const(s).CStr()[3] == 'd');
    REQUIRE(std::as_const(s).Data()[3] == 'd');
    s.CStr()[3] = 'v';
    REQUIRE(std::as_const(s).CStr()[3] == 'v');
    REQUIRE(std::as_const(s).Data()[3] == 'v');
    s.Data()[3] = 'u';
    REQUIRE(std::as_const(s).CStr()[3] == 'u');
    REQUIRE(std::as_const(s).Data()[3] == 'u');

    static_assert(std::is_same_v<decltype(std::as_const(s).CStr()), const char*>,
                  "CStr() const must return const pointer");
    static_assert(std::is_same_v<decltype(std::as_const(s).Data()), const char*>,
                  "Data() const must return const pointer");
  }
}

TEST_CASE("Swap", "[String]") {

  SECTION("Empty to empty") {
    auto a = String();
    auto b = String();
    a.Swap(b);
    REQUIRE(a.Data() == nullptr);
    REQUIRE(b.Data() == nullptr);
  }

  SECTION("Empty to filled") {
    auto a = String();
    auto b = String("abacaba");
    const auto b_data = b.Data();
    a.Swap(b);
    CheckEqual(a, "abacaba");
    CheckEqual(b, "");
    REQUIRE(a.Data() == b_data);
    REQUIRE(b.Data() == nullptr);
  }

  SECTION("Filled to filled") {
    auto a = String("abacaba");
    const auto a_data = a.Data();
    auto b = String("aba");
    const auto b_data = b.Data();
    a.Swap(b);
    CheckEqual(a, "aba");
    CheckEqual(b, "abacaba");
    REQUIRE(a.Data() == b_data);
    REQUIRE(b.Data() == a_data);
  }
}

TEST_CASE("Append", "[String]") {
  SECTION("PushBack") {
    auto s = String();
    auto actual = std::string("a");
    s.PushBack('a');
    for (size_t size = 1u; size < 1'000'000; size *= 2u) {
      for (size_t i = 0u; i < size; ++i) {
        s.PushBack(static_cast<char>('a' + i % 26));
        actual.push_back(static_cast<char>('a' + i % 26));
      }
      REQUIRE(s.Capacity() >= size * 2);
      CheckEqual(s, actual);
    }
  }

  SECTION("+=") {
    const auto other = String("abacaba");
    auto s = String();
    (s += other) += other;
    CheckEqual(s, "abacabaabacaba");
    auto actual = std::string("abacabaabacaba");
    for (size_t i = 0; i < 100000; ++i) {
      s += other;
      actual += "abacaba";
    }
    CheckEqual(s, actual);
  }

  SECTION("+") {
    {
      const auto a = String();
      const auto b = String();
      CheckEqual(a + b, "");

      using ReturnType = std::remove_const_t<decltype(a + b)>;
      static_assert(std::is_same_v<ReturnType, String>, "operator+ must return String");
    }

    {
      const auto a = String();
      const auto b = String("caba");
      CheckEqual(a + b, "caba");
    }

    {
      const auto a = String("aba");
      const auto b = String();
      CheckEqual(a + b, "aba");
    }

    {
      const auto a = String("aba");
      const auto b = String("caba");
      CheckEqual(a + b, "abacaba");
    }

    {
      const auto a = String("aba");
      CheckEqual(a + "caba", "abacaba");
    }

    {
      const auto b = String("caba");
      CheckEqual("" + b, "caba");
    }
  }
}

TEST_CASE("Clear", "[String]") {
  SECTION("Clear") {
    auto s = String();
    s.Clear();
    CheckEqual(s, "");

    for (size_t size = 1u; size < 1'000'000; size *= 2u) {
      for (size_t i = 0u; i < size; ++i) {
        s.PushBack(static_cast<char>('a' + i % 26));
      }
      s.Clear();
      CheckEqual(s, "");
      REQUIRE(s.Capacity() >= size);
    }
  }

  SECTION("PopBack") {
    auto s = String("abacaba");
    s.PopBack();
    s.PopBack();
    CheckEqual(s, "abaca");
    REQUIRE(s.Capacity() >= 7u);

    for (size_t size = 1u; size < 1'000'000; size *= 2u) {
      for (size_t i = 0u; i < size; ++i) {
        s.PushBack(static_cast<char>('a' + i % 26));
      }
      for (size_t i = 0u; i < size; ++i) {
        s.PopBack();
      }
      CheckEqual(s, "abaca");
      REQUIRE(s.Capacity() >= size);
    }
  }
}

TEST_CASE("Resize", "[String]") {
  SECTION("Resize") {
    auto a = String();
    a.Resize(5, 'a');
    CheckEqual(a, "aaaaa");
    REQUIRE(a.Capacity() >= 5u);

    a.Resize(3, 'b');
    CheckEqual(a, "aaa");
    REQUIRE(a.Capacity() >= 5u);

    a.Resize(7, 'c');
    CheckEqual(a, "aaacccc");
    REQUIRE(a.Capacity() >= 7u);

    a.Resize(0, 'a');
    CheckEqual(a, "");
    REQUIRE(a.Capacity() >= 7u);
  }

  SECTION("Reserve") {
    auto a = String("abacaba");
    REQUIRE(a.Capacity() < 15);

    a.Reserve(20);
    REQUIRE(a.Capacity() >= 20);
    CheckEqual(a, "abacaba");

    a.Reserve(1'000);
    REQUIRE(a.Capacity() >= 1'000);
    CheckEqual(a, "abacaba");

    a.Reserve(1'000'000);
    REQUIRE(a.Capacity() >= 1'000'000);
    CheckEqual(a, "abacaba");

    a.Reserve(10);
    REQUIRE(a.Capacity() >= 1'000'000);
    CheckEqual(a, "abacaba");
  }

  SECTION("ShrinkToFit") {
    auto a = String("abacaba");
    a.Reserve(100);
    REQUIRE(a.Capacity() > a.Size());
    a.ShrinkToFit();
    REQUIRE(a.Capacity() == a.Size());
    CheckEqual(a, "abacaba");

    a.Resize(0, 'a');
    CheckEqual(a, "");
    REQUIRE(a.Capacity() > 0u);
    a.ShrinkToFit();
    REQUIRE(a.Size() == 0u);
    REQUIRE(a.Capacity() == 0u);

    a = "aba";
    CheckEqual(a, "aba");
    REQUIRE(a.Capacity() >= 3u);
  }
}

void CheckComparisonEqual(const String& lhs, const String& rhs) {
  REQUIRE(lhs == rhs);
  REQUIRE(lhs <= rhs);
  REQUIRE(lhs >= rhs);
  REQUIRE_FALSE(lhs != rhs);
  REQUIRE_FALSE(lhs < rhs);
  REQUIRE_FALSE(lhs > rhs);
}

void CheckComparisonLess(const String& lhs, const String& rhs) {
  REQUIRE_FALSE(lhs == rhs);
  REQUIRE(lhs <= rhs);
  REQUIRE_FALSE(lhs >= rhs);
  REQUIRE(lhs != rhs);
  REQUIRE(lhs < rhs);
  REQUIRE_FALSE(lhs > rhs);
}

void CheckComparisonGreater(const String& lhs, const String& rhs) {
  REQUIRE_FALSE(lhs == rhs);
  REQUIRE_FALSE(lhs <= rhs);
  REQUIRE(lhs >= rhs);
  REQUIRE(lhs != rhs);
  REQUIRE_FALSE(lhs < rhs);
  REQUIRE(lhs > rhs);
}

TEST_CASE("Comparisons", "[String]") {
  {
    const auto a = String();
    const auto b = String();
    CheckComparisonEqual(a, b);
  }

  {
    const auto a = String();
    const auto b = String(1, 'a');
    CheckComparisonLess(a, b);
    CheckComparisonGreater(b, a);
  }

  {
    const auto a = String("ac");
    const auto b = String("b");
    CheckComparisonLess(a, b);
    CheckComparisonGreater(b, a);
  }

  {
    const auto a = String("abc");
    const auto b = String("aa");
    CheckComparisonLess(b, a);
    CheckComparisonGreater(a, b);
  }

  {
    const auto a = String("abcd");
    const auto b = String("abcd");
    CheckComparisonEqual(a, b);
  }

  {
    const auto a = String("abcd");
    const auto b = String("abc");
    CheckComparisonLess(b, a);
    CheckComparisonGreater(a, b);
  }

  {
    const auto a = String("adfh");
    const auto b = String("bceg");
    CheckComparisonLess(a, b);
    CheckComparisonGreater(b, a);
  }

  {
    const auto a = String("abce");
    const auto b = String("abde");
    CheckComparisonLess(a, b);
    CheckComparisonGreater(b, a);
  }
}

TEST_CASE("Output", "[String]") {
  auto oss = std::ostringstream();
  oss << String("abacaba") << ' ' << String() << ' ' << String(5, 'a');
  REQUIRE(oss.str() == "abacaba  aaaaa");
}

TEST_CASE("Input", "[String]") {
  {
    auto iss = std::istringstream("abacaba");
    auto s = String();
    iss >> s;
    CheckEqual(s, "abacaba");
  }

  {
    auto iss = std::istringstream("abacaba\t \n");
    auto s = String();
    iss >> s;
    CheckEqual(s, "abacaba");
  }

  {
    auto iss = std::istringstream("\r\n\tabacaba");
    auto s = String("caba");
    iss >> s;
    CheckEqual(s, "abacaba");
  }

  {
    auto iss = std::istringstream(" \t  \nabacaba\t\n\n\naba\r\r\r");
    auto s = String("caba");
    iss >> s;
    CheckEqual(s, "abacaba");
    iss >> s;
    CheckEqual(s, "aba");
  }

  {
    auto iss = std::istringstream(std::string(1'000'000, ' ') + std::string(1'000'000, 'a'));
    auto s = String("caba");
    iss >> s;
    CheckEqual(s, std::string(1'000'000, 'a'));
    iss >> s;
    CheckEqual(s, "");
  }
}
