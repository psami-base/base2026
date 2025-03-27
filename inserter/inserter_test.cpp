#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include "inserter.hpp"
#include "inserter.hpp"  // check include guards

#include <deque>
#include <string>
#include <memory>

TEST_CASE("BackInserter", "[Inserter]") {
  {
    auto d = std::deque<std::string>();
    auto back_inserter = BackInsertIterator(d);
    ++back_inserter = "a";
    back_inserter++ = d[0];
    *back_inserter = "c";
    back_inserter = "d";
    REQUIRE(d == std::deque<std::string>{"a", "a", "c", "d"});

    auto dd = std::deque<std::string>{"x", "y"};
    std::copy(d.begin() + 2, d.end(), BackInserter(dd));
    REQUIRE(dd == (std::deque<std::string>{"x", "y", "c", "d"}));
  }

  {
    auto d = std::deque<std::unique_ptr<int>>();
    auto back_inserter = BackInsertIterator(d);
    ++back_inserter = std::make_unique<int>(1);
    back_inserter++ = std::make_unique<int>(2);
    *back_inserter = std::make_unique<int>(3);
    back_inserter = std::make_unique<int>(4);
    REQUIRE(*d[0] == 1);
    REQUIRE(*d[1] == 2);
    REQUIRE(*d[2] == 3);
    REQUIRE(*d[3] == 4);

    auto dd = std::deque<std::unique_ptr<int>>();
    dd.emplace_back(std::make_unique<int>(5));
    dd.emplace_back(std::make_unique<int>(6));
    std::move(d.begin() + 2, d.end(), BackInserter(dd));
    REQUIRE(*dd[0] == 5);
    REQUIRE(*dd[1] == 6);
    REQUIRE(*dd[2] == 3);
    REQUIRE(*dd[3] == 4);
  }
}

TEST_CASE("FrontInserter", "[Inserter]") {
  {
    auto d = std::deque<std::string>{};
    auto front_inserter = FrontInsertIterator(d);
    ++front_inserter = "a";
    front_inserter++ = d[0];
    *front_inserter = "c";
    front_inserter = "d";
    REQUIRE(d == (std::deque<std::string>{"d", "c", "a", "a"}));

    auto b = std::deque<std::string>{"x", "y"};
    std::copy(d.begin() + 2, d.end(), FrontInserter(b));
    REQUIRE(b == (std::deque<std::string>{"a", "a", "x", "y"}));
  }

  {
    auto d = std::deque<std::unique_ptr<int>>{};
    auto front_inserter = FrontInsertIterator(d);
    ++front_inserter = std::make_unique<int>(1);
    front_inserter++ = std::make_unique<int>(2);
    *front_inserter = std::make_unique<int>(3);
    front_inserter = std::make_unique<int>(4);
    REQUIRE(*d[0] == 4);
    REQUIRE(*d[1] == 3);
    REQUIRE(*d[2] == 2);
    REQUIRE(*d[3] == 1);

    auto dd = std::deque<std::unique_ptr<int>>();
    dd.emplace_back(std::make_unique<int>(5));
    dd.emplace_back(std::make_unique<int>(6));
    std::move(d.begin() + 2, d.end(), FrontInserter(dd));
    REQUIRE(*dd[0] == 1);
    REQUIRE(*dd[1] == 2);
    REQUIRE(*dd[2] == 5);
    REQUIRE(*dd[3] == 6);
  }
}

TEST_CASE("Inserter", "[Inserter]") {
  auto s = std::string("ab");
  auto inserter = InsertIterator(s, std::next(s.begin()));
  ++inserter = 'a';
  inserter++ = 'b';
  *inserter = 'c';
  inserter = 'd';
  REQUIRE(s == "aabcdb");

  std::fill_n(Inserter(s, s.begin() + 2), 100, 'e');
  REQUIRE(s == "aa" + std::string(100, 'e') + "bcdb");
}
