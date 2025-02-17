#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "array.hpp"
#include "array.hpp"  // check include guards

#include <array>
#include <utility>

template <class T, class U>
inline constexpr auto kSwappable = requires(T t, U u) {
  t.Swap(u);
};

template <class T, class U, size_t N>
void Equals(const Array<T, N>& actual, const std::array<U, N>& required) {
  for (size_t i = 0; i < N; ++i) {
    REQUIRE(actual[i] == required[i]);
  }
}

TEST_CASE("Automatic Storage", "[Array]") {
  static_assert(std::is_aggregate_v<Array<double, 3>>, "Array must support aggregate initialization");
  static_assert(sizeof(Array<int, 1>) == sizeof(int), "Size of Array<int, 1> must contain 1 int");
  static_assert(sizeof(Array<int, 17>) == sizeof(int[17]), "Size of Array<int, 17> must be equal to size of int[17]");
  static_assert(sizeof(Array<double, 13>) == sizeof(double[13]),
                "Size of Array<double, 13> must be equal to size of double[13]");
}

TEST_CASE("Size and Empty", "[Array]") {
  const auto array = Array<int, 7>();
  REQUIRE(array.Size() == 7);
  REQUIRE_FALSE(array.Empty());
}

TEST_CASE("Indexing", "[Array]") {
  auto a = Array<int, 4>{};
  a[0] = 1;
  a[1] = -1;
  a[3] = 0;
  (a[2] = a[3]) = 987;
  Equals(a, std::array{1, -1, 987, 0});

  static_assert(std::is_same_v<decltype(std::as_const(a)[0]), const int&>,
                "operator[] must return const reference for a constant array");
}

TEST_CASE("Front", "[Array]") {
  auto a = Array<char, 3>{};
  a[0] = '1';
  a[1] = 'z';
  a[2] = '*';

  REQUIRE(std::as_const(a).Front() == '1');
  a.Front() = '+';
  REQUIRE(std::as_const(a).Front() == '+');

  static_assert(std::is_same_v<decltype(std::as_const(a).Front()), const char&>,
                "Front() must return const reference for a constant array");
}

TEST_CASE("Back", "[Array]") {
  auto a = Array<char, 3>{};
  a[0] = '1';
  a[1] = 'z';
  a[2] = '*';

  REQUIRE(std::as_const(a).Back() == '*');
  a.Back() = '+';
  REQUIRE(std::as_const(a).Back() == '+');

  static_assert(std::is_same_v<decltype(std::as_const(a).Back()), const char&>,
                "Back() must return const reference for a constant array");
}

TEST_CASE("Data", "[Array]") {
  auto a = Array<int, 3>{};

  REQUIRE(std::as_const(a).Data() == &(a[0]));

  *(a.Data() + 1) = 2;
  REQUIRE(a[1] == 2);

  static_assert((std::is_same_v<decltype(a.Data()), int*>), "Data() must return a pointer");
  static_assert((std::is_same_v<decltype(std::as_const(a).Data()), const int*>),
                "Data() must return a pointer to const for a constant array");
}

TEST_CASE("Fill", "[Array]") {
  auto a = Array<int, 7>{};
  a.Fill(-11);
  Equals(a, std::array{-11, -11, -11, -11, -11, -11, -11});
  a.Fill(1);
  Equals(a, std::array{1, 1, 1, 1, 1, 1, 1});
}

TEST_CASE("Aggregate Initialization", "[Array]") {

  SECTION("Full Fill") {
    const auto a = Array<int, 3>{5, 4, -9};
    Equals(a, std::array{5, 4, -9});
  }

  SECTION("No Fill") {
    const auto a = Array<int16_t, 5>{};
    Equals(a, std::array{0, 0, 0, 0, 0});
  }

  SECTION("Partial Fill") {
    const auto a = Array<int64_t, 4>{9, -5};
    Equals(a, std::array{9, -5, 0, 0});
  }

  SECTION("Struct") {
    struct S {
      int i;
      char c;
    };

    auto arr = Array<S, 3>{{{1, 'a'}, {2, 'b'}}};
    REQUIRE(arr[0].i == 1);
    REQUIRE(arr[0].c == 'a');
    REQUIRE(arr[1].i == 2);
    REQUIRE(arr[1].c == 'b');
    REQUIRE(arr[2].i == 0);
    REQUIRE(arr[2].c == '\0');
  }
}

TEST_CASE("Swap", "[Array]") {
  auto a = Array<int, 3>{1, 2, 3};
  auto b = Array<int, 3>{-1, -2, -3};

  a.Swap(b);
  Equals(a, std::array{-1, -2, -3});
  Equals(b, std::array{1, 2, 3});

  static_assert(!kSwappable<Array<int, 3>, Array<float, 3>>, "Arrays of different types must not be swappable");
  static_assert(!kSwappable<Array<int, 3>, Array<int, 4>>, "Arrays of different sizes must not be swappable");
}

#ifdef ARRAY_TRAITS_IMPLEMENTED

TEST_CASE("GetSize", "[Array Traits]") {
  SECTION("Not Array") {
    auto x = 8;
    REQUIRE(GetSize(x) == 0);
    REQUIRE(GetSize(&x) == 0);
    REQUIRE(GetSize(nullptr) == 0);
  }

  SECTION("1D Array") {
    int single[1];
    REQUIRE(GetSize(single) == 1);

    int arr[19];
    REQUIRE(GetSize(arr) == 19);
  }

  SECTION("ND Array") {
    double single[1][1];
    REQUIRE(GetSize(single) == 1);

    double arr[18][10][5];
    REQUIRE(GetSize(arr) == 18);
  }
}

TEST_CASE("GetRank", "[Array Traits]") {
  SECTION("Not Array") {
    auto x = 8;
    REQUIRE(GetRank(x) == 0);
    REQUIRE(GetRank(&x) == 0);
    REQUIRE(GetRank(nullptr) == 0);
  }

  SECTION("1D Array") {
    int single[1];
    REQUIRE(GetRank(single) == 1);

    int arr[19];
    REQUIRE(GetRank(arr) == 1);
  }

  SECTION("ND Array") {
    double single[1][1][1];
    REQUIRE(GetRank(single) == 3);

    double arr[18][10][5];
    REQUIRE(GetRank(arr) == 3);
  }
}

TEST_CASE("GetNumElements", "[Array Traits]") {
  SECTION("Not Array") {
    auto x = 8;
    REQUIRE(GetNumElements(x) == 1);
    REQUIRE(GetNumElements(&x) == 1);
    REQUIRE(GetNumElements(nullptr) == 1);
  }

  SECTION("1D Array") {
    int single[1];
    REQUIRE(GetNumElements(single) == 1);

    int arr[19];
    REQUIRE(GetNumElements(arr) == 19);
  }

  SECTION("ND Array") {
    double single[1][1][1];
    REQUIRE(GetNumElements(single) == 1);

    double arr[18][10][5];
    REQUIRE(GetNumElements(arr) == 900);
  }
}

#endif  // ARRAY_TRAITS_IMPLEMENTED
