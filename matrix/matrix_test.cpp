#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "rational.hpp"

#include "matrix.hpp"
#include "matrix.hpp"  // check include guards

#include <array>
#include <sstream>
#include <cstdint>
#include <type_traits>
#include <utility>

template <class T, class U>
inline constexpr auto kAddable = requires(T a, U b) {
  a + b;
  b + a;
  a += b;
};

template <class T, class U>
inline constexpr auto kSubtractable = requires(T a, U b) {
  a - b;
  b - a;
  a -= b;
};

template <class T, class U>
inline constexpr auto kMultipliable = requires(T a, U b) {
  a* b;
  b* a;
};

template <class T, class U>
inline constexpr auto kInplaceMultipliable = requires(T a, U b) {
  a *= b;
};

template <class T, class U>
inline constexpr auto kDivisible = requires(T a, U b) {
  a / b;
};

template <class T, class U>
inline constexpr auto kInplaceDivisible = requires(T a, U b) {
  a /= b;
};

template <class T, class U>
inline constexpr auto kEquatable = requires(T a, U b) {
  a == b;
  b == a;
  a != b;
  b != a;
};

template <class T, size_t N, size_t M>
void EqualMatrix(const Matrix<T, N, M>& matrix, const std::array<std::array<T, M>, N>& arr) {
  for (size_t i = 0u; i < N; ++i) {
    for (size_t j = 0u; j < M; ++j) {
      REQUIRE(matrix(i, j) == arr[i][j]);
    }
  }
}

TEST_CASE("AutomaticStorage", "[Matrix]") {
  static_assert(std::is_aggregate_v<Matrix<int, 2, 3>>, "Matrix must support aggregate initialization");
  static_assert(sizeof(Matrix<int, 1, 1>) == sizeof(int),
                "Size of Matrix<int, 1, 1> must be equal to size of 1x1 ints");
  static_assert(sizeof(Matrix<int, 17, 2>) == sizeof(int) * 34,
                "Size of Matrix<int, 17, 2> must be equal to size of 17x2 ints");
  static_assert(sizeof(Matrix<double, 13, 3>) == sizeof(double) * 39,
                "Size of Matrix<double, 13, 3> must be equal to size of 13x3 doubles");
}

TEST_CASE("Size", "[Matrix]") {
  const auto matrix = Matrix<int, 6, 7>{};
  REQUIRE(matrix.RowsNumber() == 6);
  REQUIRE(matrix.ColumnsNumber() == 7);
}

TEST_CASE("Indexing", "[Matrix]") {
  auto matrix = Matrix<int, 2, 3>{};
  matrix(0, 0) = 1;
  matrix(1, 1) = -1;
  matrix(0, 2) = 7;
  EqualMatrix(std::as_const(matrix), std::array<std::array<int, 3>, 2>{1, 0, 7, 0, -1, 0});

  static_assert(std::is_same_v<decltype(std::as_const(matrix)(0, 0)), const int&>,
                "operator() must return const reference for a constant Matrix");
}

TEST_CASE("Aggregate Initialization", "[Matrix]") {
  const auto a = Matrix<int, 2, 2>{1, 2, -2, -1};
  EqualMatrix(a, std::array<std::array<int, 2>, 2>{1, 2, -2, -1});

  const auto b = Matrix<char, 1, 3>{{'a', 'c'}};
  EqualMatrix(b, std::array<std::array<char, 3>, 1>{'a', 'c', '\0'});

  const auto c = Matrix<int16_t, 3, 1>{{{-1}, 1}};
  EqualMatrix(c, std::array<std::array<int16_t, 1>, 3>{-1, 1, 0});
}

TEST_CASE("Sum", "[Matrix]") {
  static_assert(!kAddable<Matrix<int, 2, 2>, Matrix<int, 2, 3>>, "Matrices of different sizes must not be addable");
  static_assert(!kAddable<Matrix<int, 2, 2>, Matrix<int, 3, 2>>, "Matrices of different sizes must not be addable");

  auto matrix = Matrix<int, 2, 3>{1, 2, 3, 4, 5, 6};
  const auto delta = Matrix<int, 2, 3>{2, -1, 3, -2, 1, -3};

  {
    matrix += delta;
    EqualMatrix(matrix, std::array<std::array<int, 3>, 2>{{3, 1, 6, 2, 6, 3}});
    EqualMatrix(matrix += delta, std::array<std::array<int, 3>, 2>{{5, 0, 9, 0, 7, 0}});

    (matrix += delta) = delta;
    EqualMatrix(matrix, std::array<std::array<int, 3>, 2>{2, -1, 3, -2, 1, -3});
  }

  {
    EqualMatrix(delta + delta, std::array<std::array<int, 3>, 2>{4, -2, 6, -4, 2, -6});
    EqualMatrix(delta + Matrix<int, 2, 3>{1, 2, 3, 4, 5, 6}, std::array<std::array<int, 3>, 2>{{3, 1, 6, 2, 6, 3}});
    EqualMatrix(Matrix<int, 2, 3>{1, 2, 3, 4, 5, 6} + delta, std::array<std::array<int, 3>, 2>{{3, 1, 6, 2, 6, 3}});
  }

  using ReturnType = std::remove_const_t<decltype(matrix + matrix)>;
  static_assert(std::is_same_v<ReturnType, Matrix<int, 2, 3>>, "operator+ must return a Matrix");
  static_assert(std::is_same_v<decltype(matrix += matrix), Matrix<int, 2, 3>&>, "operator+= must return a Matrix&");
}

TEST_CASE("Subtraction", "[Matrix]") {
  static_assert(!kSubtractable<Matrix<int, 2, 2>, Matrix<int, 2, 3>>,
                "Matrices of different sizes must not be subtractable");
  static_assert(!kSubtractable<Matrix<int, 2, 2>, Matrix<int, 3, 2>>,
                "Matrices of different sizes must not be subtractable");

  auto matrix = Matrix<int, 2, 3>{1, 2, 3, 4, 5, 6};
  const auto delta = Matrix<int, 2, 3>{2, -1, 3, -2, 1, -3};

  {
    matrix -= delta;
    EqualMatrix(matrix, std::array<std::array<int, 3>, 2>{{-1, 3, 0, 6, 4, 9}});
    EqualMatrix(matrix -= delta, std::array<std::array<int, 3>, 2>{{-3, 4, -3, 8, 3, 12}});

    (matrix -= delta) = delta;
    EqualMatrix(matrix, std::array<std::array<int, 3>, 2>{2, -1, 3, -2, 1, -3});
  }

  {
    EqualMatrix(delta - delta, std::array<std::array<int, 3>, 2>{});
    EqualMatrix(delta - Matrix<int, 2, 3>{1, 2, 3, 4, 5, 6}, std::array<std::array<int, 3>, 2>{{1, -3, 0, -6, -4, -9}});
    EqualMatrix(Matrix<int, 2, 3>{1, 2, 3, 4, 5, 6} - delta, std::array<std::array<int, 3>, 2>{{-1, 3, 0, 6, 4, 9}});
  }

  using ReturnType = std::remove_const_t<decltype(matrix - matrix)>;
  static_assert(std::is_same_v<ReturnType, Matrix<int, 2, 3>>, "operator- must return a Matrix");
  static_assert(std::is_same_v<decltype(matrix -= matrix), Matrix<int, 2, 3>&>, "operator-= must return a Matrix&");
}

TEST_CASE("Multiplication", "[Matrix]") {
  static_assert(!kMultipliable<Matrix<int, 2, 3>, Matrix<int, 2, 2>>,
                "Matrices of incompatible sizes must not be multipliable");
  static_assert(!kInplaceMultipliable<Matrix<int, 3, 2>, Matrix<int, 2, 3>>,
                "Matrix must not be inplace multipliable by a non-square matrix");

  auto matrix = Matrix<int, 3, 2>{1, 2, 3, 4, 5, 6};
  const auto delta = Matrix<int, 2, 2>{2, -1, 3, -2};

  {
    matrix *= delta;
    EqualMatrix(matrix, std::array<std::array<int, 2>, 3>{{8, -5, 18, -11, 28, -17}});
    EqualMatrix(matrix *= delta, std::array<std::array<int, 2>, 3>{{1, 2, 3, 4, 5, 6}});

    (matrix *= delta) = Matrix<int, 3, 2>{};
    EqualMatrix(matrix, std::array<std::array<int, 2>, 3>{});
  }

  {
    EqualMatrix(delta * delta, std::array<std::array<int, 2>, 2>{1, 0, 0, 1});
    EqualMatrix(delta * Matrix<int, 2, 1>{1, 2}, std::array<std::array<int, 1>, 2>{0, -1});
    EqualMatrix(Matrix<int, 3, 2>{1, 2, 3, 4, 5, 6} * delta,
                std::array<std::array<int, 2>, 3>{{8, -5, 18, -11, 28, -17}});
  }

  using ReturnType = std::remove_const_t<decltype(matrix * delta)>;
  static_assert(std::is_same_v<ReturnType, Matrix<int, 3, 2>>, "operator* must return a Matrix");
  static_assert(std::is_same_v<decltype(matrix *= delta), Matrix<int, 3, 2>&>, "operator*= must return a Matrix&");
}

TEST_CASE("ScalarMultiplication", "[Matrix]") {
  static_assert(kMultipliable<Matrix<int, 2, 3>, int>, "Matrix must be multipliable by a scalar");
  static_assert(kInplaceMultipliable<Matrix<double, 3, 2>, double>, "Matrix must be inplace multipliable by a scalar");

  auto matrix = Matrix<int, 3, 2>{1, 2, 3, 4, 5, 6};
  const auto delta = -2;

  {
    matrix *= delta;
    EqualMatrix(matrix, std::array<std::array<int, 2>, 3>{{-2, -4, -6, -8, -10, -12}});
    EqualMatrix(matrix *= delta, std::array<std::array<int, 2>, 3>{{4, 8, 12, 16, 20, 24}});

    (matrix *= delta) = Matrix<int, 3, 2>{};
    EqualMatrix(matrix, std::array<std::array<int, 2>, 3>{});
  }

  {
    EqualMatrix(delta * Matrix<int, 2, 1>{1, 2}, std::array<std::array<int, 1>, 2>{-2, -4});
    EqualMatrix(Matrix<int, 3, 2>{1, 2, 3, 4, 5, 6} * delta,
                std::array<std::array<int, 2>, 3>{{-2, -4, -6, -8, -10, -12}});
  }

  using ReturnType = std::remove_const_t<decltype(matrix * delta)>;
  static_assert(std::is_same_v<ReturnType, Matrix<int, 3, 2>>, "operator* must return a Matrix");
  static_assert(std::is_same_v<decltype(matrix *= delta), Matrix<int, 3, 2>&>, "operator*= must return a Matrix&");
}

TEST_CASE("ScalarDivision", "[Matrix]") {
  static_assert(kDivisible<Matrix<int, 2, 3>, int>, "Matrix must be divisible by a scalar");
  static_assert(kInplaceDivisible<Matrix<double, 3, 2>, double>, "Matrix must be inplace divisible by a scalar");

  auto matrix = Matrix<int, 3, 2>{1, 2, 3, 4, 5, 6};
  const auto delta = -2;

  {
    matrix /= delta;
    EqualMatrix(matrix, std::array<std::array<int, 2>, 3>{{0, -1, -1, -2, -2, -3}});
    EqualMatrix(matrix /= delta, std::array<std::array<int, 2>, 3>{{0, 0, 0, 1, 1, 1}});

    (matrix /= delta) = Matrix<int, 3, 2>{1, 1, 1, 1, 1, 1};
    EqualMatrix(matrix, std::array<std::array<int, 2>, 3>{1, 1, 1, 1, 1, 1});
  }

  {
    EqualMatrix(Matrix<int, 3, 2>{1, 2, 3, 4, 5, 6} / delta,
                std::array<std::array<int, 2>, 3>{{0, -1, -1, -2, -2, -3}});
  }

  using ReturnType = std::remove_const_t<decltype(matrix / delta)>;
  static_assert(std::is_same_v<ReturnType, Matrix<int, 3, 2>>, "operator/ must return a Matrix");
  static_assert(std::is_same_v<decltype(matrix /= delta), Matrix<int, 3, 2>&>, "operator/= must return a Matrix&");
}

void CheckEqual(const auto& a, const auto& b) {
  REQUIRE(a == b);
  REQUIRE(b == a);
  REQUIRE_FALSE(a != b);
  REQUIRE_FALSE(b != a);
}

void CheckNotEqual(const auto& a, const auto& b) {
  REQUIRE(a != b);
  REQUIRE(b != a);
  REQUIRE_FALSE(a == b);
  REQUIRE_FALSE(b == a);
}

TEST_CASE("Equality", "[Matrix]") {
  static_assert(!kEquatable<Matrix<int, 2, 2>, Matrix<int, 2, 3>>, "Matrices of different sizes must not be equatable");

  const auto a = Matrix<int, 3, 3>{1, 2, 3, 4, 5, 6, 7, 8, 9};
  const auto b = a;
  const auto c = Matrix<int, 3, 3>{-1, 2, 3, 4, 5, 6, 7, 8, 9};

  CheckEqual(a, a);
  CheckEqual(b, b);
  CheckEqual(c, c);
  CheckEqual(a, b);
  CheckNotEqual(a, c);
  CheckNotEqual(b, c);
}

TEST_CASE("Input", "[Matrix]") {
  {
    auto ss = std::stringstream{"-5"};

    auto matrix = Matrix<int, 1, 1>();
    ss >> matrix;
    EqualMatrix(matrix, std::array<std::array<int, 1>, 1>{-5});
  }

  {
    auto ss = std::stringstream{"-5 1\n0 10"};

    auto matrix = Matrix<int, 2, 2>();
    ss >> matrix;
    EqualMatrix(matrix, std::array<std::array<int, 2>, 2>{-5, 1, 0, 10});
  }

  {
    auto ss = std::stringstream{"-5 1\n10 0\n-7 -1\na b"};

    auto a = Matrix<int, 3, 2>();
    auto b = Matrix<char, 1, 2>();
    ss >> a >> b;
    EqualMatrix(a, std::array<std::array<int, 2>, 3>{-5, 1, 10, 0, -7, -1});
    EqualMatrix(b, std::array<std::array<char, 2>, 1>{'a', 'b'});
  }
}

TEST_CASE("Output", "[Matrix]") {
  {
    const auto matrix = Matrix<int, 1, 1>{-5};

    auto ss = std::stringstream();
    ss << matrix;
    REQUIRE(ss.str() == "-5\n");
  }

  {
    const auto matrix = Matrix<int, 2, 2>{-5, 1, 0, 10};

    auto ss = std::stringstream();
    ss << matrix;
    REQUIRE(ss.str() == "-5 1\n0 10\n");
  }

  {
    const auto a = Matrix<int, 3, 2>{-5, 1, 10, 0, -7, -1};
    const auto b = Matrix<char, 1, 2>{'a', 'b'};

    auto ss = std::stringstream();
    ss << a << '\n' << b;
    REQUIRE(ss.str() == "-5 1\n10 0\n-7 -1\n\na b\n");
  }
}

TEST_CASE("GetTransposed", "[Matrix]") {
  {
    const auto matrix = Matrix<int, 1, 1>{-1};
    REQUIRE(matrix == GetTransposed(matrix));
  }

  {
    const auto matrix = Matrix<int, 2, 2>{1, 2, 3, 4};
    EqualMatrix(GetTransposed(matrix), std::array<std::array<int, 2>, 2>{1, 3, 2, 4});
  }

  {
    const auto matrix = Matrix<int, 3, 2>{1, 2, 3, 4, 5, 6};
    EqualMatrix(GetTransposed(matrix), std::array<std::array<int, 3>, 2>{1, 3, 5, 2, 4, 6});

    using ReturnType = std::remove_const_t<decltype(GetTransposed(matrix))>;
    static_assert(std::is_same_v<ReturnType, Matrix<int, 2, 3>>, "GetTransposed must return a Matrix");
  }
}

#ifdef MATRIX_SQUARE_MATRIX_IMPLEMENTED

template <class T>
inline constexpr auto kInplaceTransposable = requires(T a) {
  Transpose(a);
};

template <class T>
inline constexpr auto kTraceable = requires(T a) {
  Trace(a);
};

template <class T>
inline constexpr auto kDeterminantable = requires(T a) {
  Determinant(a);
};

template <class T>
inline constexpr auto kInversible = requires(T a) {
  Inverse(a);
  GetInversed(a);
};

TEST_CASE("Transpose", "[SquareMatrix]") {
  static_assert(!kInplaceTransposable<Matrix<int, 2, 3>>, "Non-square matrices must not be inplace transposable");

  {
    auto matrix = Matrix<int, 2, 2>{-1, 4, 9, 2};
    Transpose(matrix);
    EqualMatrix(matrix, std::array<std::array<int, 2>, 2>{-1, 9, 4, 2});
  }

  {
    auto matrix = Matrix<int, 3, 3>{-1, 4, 9, 2, 5, -7, 0, 2, 0};
    Transpose(matrix);
    EqualMatrix(matrix, std::array<std::array<int, 3>, 3>{-1, 2, 0, 4, 5, 2, 9, -7, 0});
  }
}

TEST_CASE("Trace", "[SquareMatrix]") {
  static_assert(!kTraceable<Matrix<int, 2, 3>>, "Trace must not be available for non-square matrices");

  {
    const auto matrix = Matrix<int, 2, 2>{-1, 4, 9, 2};
    REQUIRE(Trace(matrix) == 1);
  }

  {
    const auto matrix = Matrix<int, 3, 3>{-1, 4, 9, 2, 5, -7, 0, 2, 0};
    REQUIRE(Trace(matrix) == 4);
  }
}

TEST_CASE("Determinant", "[SquareMatrix]") {
  static_assert(!kDeterminantable<Matrix<int, 2, 3>>, "Determinant must not be available for non-square matrices");

  {
    const auto matrix = Matrix<int, 1, 1>{3};
    REQUIRE(Determinant(matrix) == 3);
  }

  {
    const auto matrix = Matrix<int, 2, 2>{-1, 4, 9, 2};
    REQUIRE(Determinant(matrix) == -38);
  }

  {
    const auto matrix = Matrix<int, 3, 3>{-1, 4, 9, 2, 5, -7, 0, 2, 0};
    REQUIRE(Determinant(matrix) == 22);
  }

  {
    const auto matrix =
        Matrix<Rational, 3, 3>{Rational{1},    Rational{1, 2}, Rational{1, 3}, Rational{1, 4}, Rational{1, 5},
                               Rational{1, 6}, Rational{1, 7}, Rational{1, 8}, Rational{1, 9}};
    REQUIRE(Determinant(matrix) == Rational{1, 3360});
  }
}

TEST_CASE("Inverse", "[SquareMatrix]") {
  static_assert(!kInversible<Matrix<int, 2, 3>>, "Only square matrices must be inversible");

  {
    auto matrix = Matrix<Rational, 1, 1>{3};
    Inverse(matrix);
    EqualMatrix(matrix, std::array<std::array<Rational, 1>, 1>{Rational{1, 3}});
  }

  {
    auto matrix = Matrix<Rational, 2, 2>{-1, 4, 9, 2};
    Inverse(matrix);
    EqualMatrix(matrix, std::array<std::array<Rational, 2>, 2>{Rational{-1, 19}, Rational{2, 19}, Rational{9, 38},
                                                               Rational{1, 38}});
  }

  {
    auto matrix = Matrix<Rational, 3, 3>{-1, 4, 9, 2, 5, -7, 0, 2, 0};
    Inverse(matrix);
    EqualMatrix(matrix, std::array<std::array<Rational, 3>, 3>{Rational{7, 11}, Rational{9, 11}, Rational{-73, 22},
                                                               Rational{0}, Rational{0}, Rational{1, 2},
                                                               Rational{2, 11}, Rational{1, 11}, Rational{-13, 22}});
  }

  {
    auto matrix = Matrix<Rational, 3, 3>{Rational{1},    Rational{1, 2}, Rational{1, 3}, Rational{1, 4}, Rational{1, 5},
                                         Rational{1, 6}, Rational{1, 7}, Rational{1, 8}, Rational{1, 9}};
    Inverse(matrix);
    EqualMatrix(matrix, std::array<std::array<Rational, 3>, 3>{Rational{14, 3}, Rational{-140, 3}, Rational{56},
                                                               Rational{-40, 3}, Rational{640, 3}, Rational{-280},
                                                               Rational{9}, Rational{-180}, Rational{252}});
  }
}

TEST_CASE("GetInversed", "[SquareMatrix]") {
  {
    const auto matrix = Matrix<Rational, 1, 1>{3};
    EqualMatrix(GetInversed(matrix), std::array<std::array<Rational, 1>, 1>{Rational{1, 3}});
  }

  {
    const auto matrix = Matrix<Rational, 2, 2>{-1, 4, 9, 2};
    EqualMatrix(GetInversed(matrix), std::array<std::array<Rational, 2>, 2>{Rational{-1, 19}, Rational{2, 19},
                                                                            Rational{9, 38}, Rational{1, 38}});
  }

  {
    const auto matrix = Matrix<Rational, 3, 3>{-1, 4, 9, 2, 5, -7, 0, 2, 0};
    EqualMatrix(GetInversed(matrix), std::array<std::array<Rational, 3>, 3>{
                                         Rational{7, 11}, Rational{9, 11}, Rational{-73, 22}, Rational{0}, Rational{0},
                                         Rational{1, 2}, Rational{2, 11}, Rational{1, 11}, Rational{-13, 22}});
  }

  {
    const auto matrix =
        Matrix<Rational, 3, 3>{Rational{1},    Rational{1, 2}, Rational{1, 3}, Rational{1, 4}, Rational{1, 5},
                               Rational{1, 6}, Rational{1, 7}, Rational{1, 8}, Rational{1, 9}};
    EqualMatrix(GetInversed(matrix), std::array<std::array<Rational, 3>, 3>{
                                         Rational{14, 3}, Rational{-140, 3}, Rational{56}, Rational{-40, 3},
                                         Rational{640, 3}, Rational{-280}, Rational{9}, Rational{-180}, Rational{252}});

    using ReturnType = std::remove_const_t<decltype(GetInversed(matrix))>;
    static_assert(std::is_same_v<ReturnType, Matrix<Rational, 3, 3>>, "GetInversed must return a Matrix");
  }
}
#endif  // MATRIX_SQUARE_MATRIX_IMPLEMENTED
