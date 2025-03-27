#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include "optional.hpp"
#include "optional.hpp"  // check include guards

#include <vector>

struct InstanceCounter {
  inline static size_t counter = 0;

  InstanceCounter() noexcept {
    ++counter;
  }

  InstanceCounter(const InstanceCounter&) : InstanceCounter() {
  }

  InstanceCounter(InstanceCounter&&) noexcept : InstanceCounter() {
  }

  InstanceCounter& operator=(const InstanceCounter&) = default;
  InstanceCounter& operator=(InstanceCounter&&) noexcept = default;

  ~InstanceCounter() {
    --counter;
  }
};

TEST_CASE("ConstructorsAndDestructor", "[Optional]") {
  InstanceCounter::counter = 0;

  {
    const auto opt = Optional<InstanceCounter>();
    REQUIRE_FALSE(opt.HasValue());
    REQUIRE(InstanceCounter::counter == 0);
  }
  REQUIRE(InstanceCounter::counter == 0);

  {
    const auto tmp = InstanceCounter();
    const auto opt = Optional(tmp);
    REQUIRE(opt.HasValue());
    REQUIRE(InstanceCounter::counter == 2);
  }
  REQUIRE(InstanceCounter::counter == 0);

  {
    const auto opt = Optional(InstanceCounter{});
    REQUIRE(opt.HasValue());
    REQUIRE(InstanceCounter::counter == 1);
  }
  REQUIRE(InstanceCounter::counter == 0);

  {
    const auto empty = Optional<InstanceCounter>();
    const auto empty_copy = empty;
    REQUIRE_FALSE(empty_copy.HasValue());
    REQUIRE(InstanceCounter::counter == 0);
  }
  REQUIRE(InstanceCounter::counter == 0);

  {
    const auto full = Optional(InstanceCounter{});
    const auto full_copy = full;
    REQUIRE(full_copy.HasValue());
    REQUIRE(full.HasValue());
    REQUIRE(InstanceCounter::counter == 2);
  }
  REQUIRE(InstanceCounter::counter == 0);

  {
    auto empty = Optional<InstanceCounter>();
    const auto empty_moved = std::move(empty);
    REQUIRE_FALSE(empty_moved.HasValue());
    REQUIRE_FALSE(empty.HasValue());
    REQUIRE(InstanceCounter::counter == 0);
  }
  REQUIRE(InstanceCounter::counter == 0);

  {
    auto full = Optional(InstanceCounter{});
    const auto full_moved = std::move(full);
    REQUIRE(full_moved.HasValue());
    REQUIRE(full.HasValue());
    REQUIRE(InstanceCounter::counter == 2);
  }
  REQUIRE(InstanceCounter::counter == 0);

  {
    auto v = Optional(std::vector<int>(100));
    const auto v_p = (*v).data();
    const auto moved = std::move(v);
    REQUIRE((*moved).data() == v_p);
    REQUIRE(v.HasValue());
  }

  auto opt = Optional(std::vector<int>(100));
  auto v = std::vector<int>(200);
  const auto v_p = v.data();
  opt = std::move(v);
  REQUIRE(opt.HasValue());
  REQUIRE((*opt).data() == v_p);
}

TEST_CASE("Assignment", "[Optional]") {
  InstanceCounter::counter = 0;

  auto full = Optional(InstanceCounter{});
  auto empty = Optional<InstanceCounter>();

  REQUIRE(InstanceCounter::counter == 1);
  {
    auto tmp = Optional<InstanceCounter>();
    REQUIRE(InstanceCounter::counter == 1);

    tmp = full;  // full to empty
    REQUIRE(tmp.HasValue());
    REQUIRE(InstanceCounter::counter == 2);

    tmp = full;  // full to full
    REQUIRE(tmp.HasValue());
    REQUIRE(InstanceCounter::counter == 2);

    tmp = empty;  // empty to full
    REQUIRE_FALSE(tmp.HasValue());
    REQUIRE(InstanceCounter::counter == 1);

    tmp = empty;  // empty to empty
    REQUIRE_FALSE(tmp.HasValue());
    REQUIRE(InstanceCounter::counter == 1);
  }

  REQUIRE(InstanceCounter::counter == 1);
  {
    auto tmp = Optional<InstanceCounter>();
    REQUIRE(InstanceCounter::counter == 1);

    tmp = std::move(full);  // full to empty
    REQUIRE(InstanceCounter::counter == 2);
    REQUIRE(tmp.HasValue());

    tmp = std::move(full);  // full to full
    REQUIRE(InstanceCounter::counter == 2);
    REQUIRE(tmp.HasValue());

    tmp = std::move(empty);  // empty to full
    REQUIRE(InstanceCounter::counter == 1);
    REQUIRE_FALSE(tmp.HasValue());

    tmp = std::move(empty);  // empty to empty
    REQUIRE(InstanceCounter::counter == 1);
    REQUIRE_FALSE(tmp.HasValue());
  }

  REQUIRE(InstanceCounter::counter == 1);
  {
    const auto cnt = InstanceCounter();
    full = cnt;
    REQUIRE(InstanceCounter::counter == 2);

    full = InstanceCounter{};
    REQUIRE(InstanceCounter::counter == 2);

    empty = cnt;
    REQUIRE(InstanceCounter::counter == 3);

    empty = Optional<InstanceCounter>{};
    REQUIRE(InstanceCounter::counter == 2);

    empty = InstanceCounter{};
    REQUIRE(InstanceCounter::counter == 3);

    full = full;
    REQUIRE(InstanceCounter::counter == 3);
  }

  {  // no leak check
    auto opt = Optional<std::vector<int>>();
    opt = std::vector<int>(100);
    opt = Optional<std::vector<int>>{};
    auto v = std::vector<int>(100);
    opt = v;
    opt = std::move(v);
  }
}

TEST_CASE("DataAccess", "[Optional]") {
  const auto full = Optional(std::vector{1, 2, 3, 4, 5});
  const auto empty = Optional<std::vector<int>>{};

  REQUIRE(full.HasValue());
  REQUIRE_FALSE(empty.HasValue());

  REQUIRE(static_cast<bool>(full));
  REQUIRE_FALSE(static_cast<bool>(empty));

  REQUIRE(full.Value() == std::vector{1, 2, 3, 4, 5});
  REQUIRE_THROWS_AS(empty.Value(), BadOptionalAccess);  // NOLINT
  REQUIRE(*full == std::vector{1, 2, 3, 4, 5});

  auto copy = full;
  REQUIRE(*copy == std::vector{1, 2, 3, 4, 5});
  copy.Value() = std::vector{5, 4, 3, 2, 1};
  REQUIRE(*copy == std::vector{5, 4, 3, 2, 1});
  *copy = std::vector{1, 3, 5};
  REQUIRE(*copy == std::vector{1, 3, 5});
}

TEST_CASE("Swap", "[Optional]") {
  auto full_a = Optional(std::vector{1, 2, 3});
  auto full_b = Optional(std::vector{0, 1, -1});
  auto empty_a = Optional<std::vector<int>>{};
  auto empty_b = Optional<std::vector<int>>{};

  {
    full_a.Swap(full_b);
    REQUIRE(full_a.HasValue());
    REQUIRE(full_b.HasValue());
    REQUIRE(*full_a == std::vector{0, 1, -1});
    REQUIRE(*full_b == std::vector{1, 2, 3});
  }

  {
    empty_a.Swap(empty_b);
    REQUIRE_FALSE(empty_a.HasValue());
    REQUIRE_FALSE(empty_b.HasValue());
  }

  {
    full_a.Swap(empty_a);
    REQUIRE_FALSE(full_a.HasValue());
    REQUIRE(empty_a.HasValue());
    REQUIRE(*empty_a == std::vector{0, 1, -1});
  }

  {
    empty_b.Swap(full_b);
    REQUIRE_FALSE(full_a.HasValue());
    REQUIRE(empty_b.HasValue());
    REQUIRE(*empty_b == std::vector{1, 2, 3});
  }

  {
    struct Self {
      Self* self;

      Self() : self(this) {
      }

      Self(const Self& other) : self(this) {
        REQUIRE(&other == other.self);
      }

      Self(Self&& other) : self(this) {
        REQUIRE(&other == other.self);
      }

      Self& operator=(const Self& other) {
        REQUIRE(&other == other.self);
        return *this;
      }

      Self& operator=(Self&& other) {
        REQUIRE(&other == other.self);
        return *this;
      }

      operator Self*() const {  // NOLINT
        REQUIRE(this == self);
        return self;
      }
    };

    auto a = Optional<Self>();
    auto b = Optional<Self>();
    a.Swap(b);
    REQUIRE_FALSE(a.HasValue());
    REQUIRE_FALSE(b.HasValue());

    a = Self();
    b = Self();
    a.Swap(b);
    REQUIRE(*a);
    REQUIRE(*b);
  }
}

TEST_CASE("Reset", "[Optional]") {
  InstanceCounter::counter = 0;

  {
    auto opt = Optional(InstanceCounter{});
    REQUIRE(InstanceCounter::counter == 1);
    REQUIRE(opt.HasValue());

    opt.Reset();
    REQUIRE(InstanceCounter::counter == 0);
    REQUIRE_FALSE(opt.HasValue());

    opt.Reset();
    REQUIRE(InstanceCounter::counter == 0);
    REQUIRE_FALSE(opt.HasValue());
  }

  {
    auto opt = Optional(std::vector{1, 2, 3, 4, 5});
    REQUIRE(*opt == std::vector{1, 2, 3, 4, 5});
    REQUIRE(opt.HasValue());

    opt.Reset();
    REQUIRE_FALSE(opt.HasValue());

    opt.Reset();
    REQUIRE_FALSE(opt.HasValue());
  }
}

TEST_CASE("Emplace", "[Optional]") {
  InstanceCounter::counter = 0;

  {
    auto opt = Optional<InstanceCounter>();
    REQUIRE(InstanceCounter::counter == 0);
    REQUIRE_FALSE(opt.HasValue());

    opt.Emplace();
    REQUIRE(InstanceCounter::counter == 1);
    REQUIRE(opt.HasValue());

    opt.Emplace();
    REQUIRE(InstanceCounter::counter == 1);
    REQUIRE(opt.HasValue());
  }

  {
    auto opt = Optional<std::vector<int>>();
    REQUIRE_FALSE(opt.HasValue());

    opt.Emplace(5, 1);
    REQUIRE(opt.HasValue());
    REQUIRE(*opt == std::vector{1, 1, 1, 1, 1});

    opt.Emplace(3, -1);
    REQUIRE(opt.HasValue());
    REQUIRE(*opt == std::vector{-1, -1, -1});
  }

  {
    auto opt = Optional<std::vector<int>>();

    opt.Reset();
    REQUIRE_FALSE(opt.HasValue());

    opt.Emplace(2, 11);
    REQUIRE(opt.HasValue());
    REQUIRE(*opt == std::vector{11, 11});

    opt.Reset();
    REQUIRE_FALSE(opt.HasValue());

    opt.Emplace(3, -2);
    REQUIRE(opt.HasValue());
    REQUIRE(*opt == std::vector{-2, -2, -2});
  }

  {
    struct A {
      std::unique_ptr<int> ptr;
      int& ref;

      A(std::unique_ptr<int> p, int& r) : ptr(std::move(p)), ref(r) {
      }
    };

    auto opt = Optional<A>();
    const auto p = new int{11};
    opt.Emplace(std::unique_ptr<int>(p), *p);
    REQUIRE((*opt).ptr.get() == p);
    REQUIRE(*(*opt).ptr == (*opt).ref);
    (*opt).ref = 139;
    REQUIRE(*p == 139);
  }
}
