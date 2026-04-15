#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "shared_ptr.hpp"
#include "shared_ptr.hpp"  // check include guards

#include <utility>

TEST_CASE("Shared Constructors", "[SharedPtr]") {
  SECTION("Default") {
    const auto a = SharedPtr<int>();
    const auto b = SharedPtr<int>(nullptr);

    REQUIRE(a.Get() == nullptr);
    REQUIRE_FALSE(a);
    REQUIRE(b.Get() == nullptr);
    REQUIRE_FALSE(b);
  }

  SECTION("Parametric") {
    const auto ptr = new int;
    const auto a = SharedPtr(ptr);

    REQUIRE(a.Get() == ptr);
    REQUIRE(a);
  }

  SECTION("Copy") {
    const auto ptr = new int;
    const auto a = SharedPtr(ptr);
    const auto b = a;

    REQUIRE(a.Get() == ptr);
    REQUIRE(a);
    REQUIRE(b.Get() == ptr);
    REQUIRE(b);

    const auto empty = SharedPtr<int>();
    const auto c = empty;
    REQUIRE(c.Get() == nullptr);
    REQUIRE_FALSE(c);
    REQUIRE(empty.Get() == nullptr);
    REQUIRE_FALSE(empty);
  }

  SECTION("Move") {
    static_assert(std::is_nothrow_move_constructible_v<SharedPtr<int>>, "Move constructor must be marked noexcept");

    const auto ptr = new int;
    auto a = SharedPtr(ptr);
    const auto b = std::move(a);

    REQUIRE(a.Get() == nullptr);  // NOLINT check "move-from" is valid state
    REQUIRE_FALSE(a);             // NOLINT check "move-from" is valid state
    REQUIRE(b.Get() == ptr);
    REQUIRE(b);

    auto empty = SharedPtr<int>();
    const auto c = std::move(empty);
    REQUIRE(c.Get() == nullptr);
    REQUIRE_FALSE(c);
    REQUIRE(empty.Get() == nullptr);  // NOLINT check "move-from" is valid state
    REQUIRE_FALSE(empty);             // NOLINT check "move-from" is valid state
  }
}

TEST_CASE("Assignment", "[SharedPtr]") {
  SECTION("Copy") {
    const auto ptr = new int;
    const auto a = SharedPtr(ptr);
    auto b = SharedPtr<int>();
    b = a;
    REQUIRE(a.Get() == ptr);
    REQUIRE(b.Get() == ptr);
    static_assert(std::is_same_v<decltype(b = a), SharedPtr<int>&>, "Copy assignment must return *this");
  }

  SECTION("Move") {
    static_assert(std::is_nothrow_move_assignable_v<SharedPtr<int>>, "Move assignment must be marked noexcept");
    const auto ptr = new int;
    auto a = SharedPtr(ptr);
    auto b = SharedPtr<int>();
    b = std::move(a);
    REQUIRE(a.Get() == nullptr);
    REQUIRE(b.Get() == ptr);
    static_assert(std::is_same_v<decltype(b = std::move(a)), SharedPtr<int>&>, "Move assignment must return *this");
  }

  SECTION("Copy self-assignment") {
    const auto ptr = new int;
    auto a = SharedPtr(ptr);
    a = std::as_const(a);
    REQUIRE(a.Get() == ptr);
  }

  SECTION("Non-empty pointers") {
    const auto ptr1 = new int;
    const auto ptr2 = new int;
    auto a = SharedPtr(ptr1);
    const auto b = SharedPtr(ptr2);

    a = b;
    REQUIRE(a.Get() == ptr2);
    REQUIRE(b.Get() == ptr2);
  }

  SECTION("Chaining assignment") {
    const auto ptr = new int;
    const auto a = SharedPtr(ptr);
    auto b = SharedPtr(new int);
    auto c = SharedPtr<int>();
    c = b = a;
    REQUIRE(a.Get() == ptr);
    REQUIRE(b.Get() == ptr);
    REQUIRE(c.Get() == ptr);
  }

  SECTION("Assignment with empty pointers") {
    const auto ptr = new int;
    auto a = SharedPtr<int>();
    const auto b = SharedPtr(ptr);
    a = b;
    REQUIRE(a.Get() == ptr);
    REQUIRE(b.Get() == ptr);

    a = SharedPtr<int>();
    REQUIRE(a.Get() == nullptr);
  }

  SECTION("Move assignment from empty") {
    auto a = SharedPtr(new int);
    auto empty = SharedPtr<int>();
    a = std::move(empty);
    REQUIRE(a.Get() == nullptr);
    REQUIRE(empty.Get() == nullptr);
  }
}

TEST_CASE("UseCount", "[SharedPtr]") {
  auto a = SharedPtr<int>();
  const auto b = SharedPtr(new int);

  REQUIRE(a.UseCount() == 0);
  REQUIRE(b.UseCount() == 1);

  a = b;
  REQUIRE(a.UseCount() == 2);
  REQUIRE(b.UseCount() == 2);

  {  // copy/move
    auto c = a;
    REQUIRE(a.UseCount() == 3);
    REQUIRE(b.UseCount() == 3);
    REQUIRE(c.UseCount() == 3);

    const auto d = std::move(c);
    REQUIRE(a.UseCount() == 3);
    REQUIRE(b.UseCount() == 3);
    REQUIRE(c.UseCount() == 0);  // NOLINT check "moved-from" is valid state
    REQUIRE(d.UseCount() == 3);
  }

  REQUIRE(a.UseCount() == 2);
  REQUIRE(b.UseCount() == 2);

  a = nullptr;
  REQUIRE(a.UseCount() == 0);
  REQUIRE(b.UseCount() == 1);
}

TEST_CASE("Swap", "[SharedPtr]") {
  const auto ptr1 = new int;
  const auto ptr2 = new int;
  auto a = SharedPtr<int>();
  auto b = SharedPtr(ptr1);
  auto c = SharedPtr(ptr2);
  const auto d = c;

  a.Swap(a);
  REQUIRE_FALSE(a);
  REQUIRE(a.Get() == nullptr);
  REQUIRE(a.UseCount() == 0);

  b.Swap(b);
  REQUIRE(b);
  REQUIRE(b.Get() == ptr1);
  REQUIRE(b.UseCount() == 1);

  a.Swap(b);
  REQUIRE(a);
  REQUIRE(a.Get() == ptr1);
  REQUIRE(a.UseCount() == 1);
  REQUIRE_FALSE(b);
  REQUIRE(b.Get() == nullptr);
  REQUIRE(b.UseCount() == 0);

  b.Swap(c);
  REQUIRE(b);
  REQUIRE(b.Get() == ptr2);
  REQUIRE(b.UseCount() == 2);
  REQUIRE_FALSE(c);
  REQUIRE(c.Get() == nullptr);
  REQUIRE(c.UseCount() == 0);

  a.Swap(b);
  REQUIRE(a);
  REQUIRE(a.Get() == ptr2);
  REQUIRE(a.UseCount() == 2);
  REQUIRE(b);
  REQUIRE(b.Get() == ptr1);
  REQUIRE(b.UseCount() == 1);

  REQUIRE(d);
  REQUIRE(d.Get() == ptr2);
  REQUIRE(d.UseCount() == 2);
}

TEST_CASE("Shared Reset", "[SharedPtr]") {
  SECTION("Empty") {
    auto a = SharedPtr<int>();
    a.Reset();
    REQUIRE(a.UseCount() == 0);

    a.Reset(new int);
    REQUIRE(a.UseCount() == 1);

    a.Reset();
    REQUIRE(a.UseCount() == 0);
  }

  SECTION("Non-empty") {
    const auto ptr1 = new int;
    const auto ptr2 = new int;
    auto a = SharedPtr(ptr1);
    auto b = a;

    b.Reset();
    REQUIRE(a.UseCount() == 1);
    REQUIRE(a.Get() == ptr1);
    REQUIRE(b.UseCount() == 0);
    REQUIRE(b.Get() == nullptr);

    b = a;
    a.Reset(ptr2);
    REQUIRE(a.UseCount() == 1);
    REQUIRE(a.Get() == ptr2);
    REQUIRE(b.UseCount() == 1);
    REQUIRE(b.Get() == ptr1);
  }
}

TEST_CASE("Operators", "[SharedPtr]") {
  SECTION("operator*") {
    const auto a = SharedPtr(new int(19));
    REQUIRE(*a == 19);

    *a = 11;
    REQUIRE(*a == 11);

    *a.Get() = -11;
    REQUIRE(*a == -11);
  }

  SECTION("operator->") {
    const auto ptr = new int(11);
    const auto a = SharedPtr(new SharedPtr(ptr));

    REQUIRE(a->UseCount() == 1);
    REQUIRE(a->Get() == ptr);

    a->Reset();
    REQUIRE(a->UseCount() == 0);
    REQUIRE(a->Get() == nullptr);
  }
}

#ifdef WEAK_PTR_IMPLEMENTED

TEST_CASE("Weak Constructors", "[WeakPtr]") {
  const auto shared = SharedPtr(new int);
  const auto a = WeakPtr<int>();
  auto b = a;
  const auto c = WeakPtr(shared);
  const auto d = std::move(b);
}

TEST_CASE("UseCountAndExpired", "[WeakPtr]") {
  {
    const auto a = WeakPtr<int>();
    auto b = a;
    const auto c = std::move(b);

    REQUIRE(a.UseCount() == 0);
    REQUIRE(b.UseCount() == 0);  // NOLINT check "moved-from" is valid state
    REQUIRE(c.UseCount() == 0);
    REQUIRE(a.Expired());
    REQUIRE(b.Expired());
    REQUIRE(c.Expired());
  }

  auto a = WeakPtr<int>();
  {
    auto ptr1 = SharedPtr(new int);
    const auto ptr2 = ptr1;
    auto ptr3 = ptr2;
    a = ptr2;
    const auto b = WeakPtr(ptr3);

    REQUIRE(a.UseCount() == 3);
    REQUIRE(b.UseCount() == 3);
    REQUIRE(ptr1.UseCount() == 3);
    REQUIRE(ptr2.UseCount() == 3);
    REQUIRE(ptr3.UseCount() == 3);
    REQUIRE_FALSE(a.Expired());
    REQUIRE_FALSE(b.Expired());

    ptr1.Reset();
    ptr3.Reset();
    REQUIRE(a.UseCount() == 1);
    REQUIRE(b.UseCount() == 1);
    REQUIRE(ptr1.UseCount() == 0);
    REQUIRE(ptr2.UseCount() == 1);
    REQUIRE(ptr3.UseCount() == 0);
    REQUIRE_FALSE(a.Expired());
    REQUIRE_FALSE(b.Expired());
  }
  REQUIRE(a.Expired());
}

TEST_CASE("Weak Reset", "WeakPtr") {
  auto a = WeakPtr<int>();
  a.Reset();
  REQUIRE(a.UseCount() == 0);
  REQUIRE(a.Expired());

  const auto ptr = SharedPtr(new int);
  auto b = WeakPtr(ptr);
  a = b;
  b.Reset();
  REQUIRE(ptr.UseCount() == 1);
  REQUIRE(a.UseCount() == 1);
  REQUIRE(b.UseCount() == 0);
  REQUIRE(b.Expired());
  REQUIRE_FALSE(a.Expired());

  auto empty_ptr = SharedPtr<int>();
  const auto c = WeakPtr(empty_ptr);
  REQUIRE(c.Expired());

  empty_ptr.Reset(new int);
  REQUIRE(c.Expired());
}

TEST_CASE("Lock", "[WeakPtr]") {
  auto a = WeakPtr<int>();
  a.Reset();
  REQUIRE(a.Lock().Get() == nullptr);

  {
    const auto p = new int;
    const auto ptr = SharedPtr(p);
    auto b = WeakPtr(ptr);
    a = b;
    b.Reset();
    REQUIRE(b.Lock().Get() == nullptr);
    REQUIRE(a.Lock().Get() == p);
  }

  {
    const auto p = new int;
    auto ptr = SharedPtr(p);
    const auto b = WeakPtr(ptr);
    const auto ptr_tmp = b.Lock();

    ptr.Reset();
    REQUIRE(ptr_tmp.Get() == p);
    REQUIRE(ptr_tmp.UseCount() == 1);
  }
}

TEST_CASE("ConstructFromWeak", "[SharedPtr]") {
  {
    const auto wptr = WeakPtr<int>();
    REQUIRE_THROWS_AS(SharedPtr(wptr), BadWeakPtr);  // NOLINT
  }

  {
    const auto ptr = new int;
    const auto init = SharedPtr(ptr);
    const auto weak = WeakPtr(init);
    const auto a = SharedPtr(weak);
    const auto b = SharedPtr(weak);

    REQUIRE_FALSE(weak.Expired());
    REQUIRE(weak.UseCount() == 3);
    REQUIRE(init.UseCount() == 3);
    REQUIRE(a.UseCount() == 3);
    REQUIRE(b.UseCount() == 3);
  }

  {
    const auto x = new SharedPtr(new int);
    const auto y = WeakPtr(*x);
    delete x;
    REQUIRE(y.Expired());
    REQUIRE_THROWS_AS(SharedPtr(y), BadWeakPtr);  // NOLINT
  }
}

TEST_CASE("Cycle", "[WeakPtr]") {
  struct Node {
    SharedPtr<Node> next;
    WeakPtr<Node> prev;
  };

  auto front = SharedPtr(new Node{});
  auto back = SharedPtr(new Node{});
  front->next = back;
  back->prev = front;
  REQUIRE(front.UseCount() == 1);
  REQUIRE(back.UseCount() == 2);
}

#endif  // WEAK_PTR_IMPLEMENTED

#ifdef MAKE_SHARED_IMPLEMENTED

TEST_CASE("MakeShared", "[SharedPtr]") {
  {
    const auto ptr = MakeShared<SharedPtr<int>>();
    REQUIRE(ptr->UseCount() == 0);
    REQUIRE(ptr->Get() == nullptr);
  }

  {
    const auto ptr = MakeShared<std::pair<int, double>>(11, 0.5);
    REQUIRE(ptr->first == 11);
    REQUIRE(ptr->second == 0.5);
  }

  {
    int x = 11;
    const auto ptr = MakeShared<std::pair<int&, std::unique_ptr<int>>>(x, std::make_unique<int>(11));
    REQUIRE(ptr->first == 11);
    REQUIRE(*(ptr->second) == 11);

    x = -11;
    *(ptr->second) = -11;
    REQUIRE(ptr->first == -11);
    REQUIRE(*(ptr->second) == -11);
  }
}

#endif
