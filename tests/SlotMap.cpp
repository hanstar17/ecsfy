#include "ecsfy/SlotMap.hpp"
#include "catch2/catch.hpp"
#include <iostream>

TEST_CASE( "test slot map", "[slot map]" ) {
  using SlotMap = ecsfy::SlotMap<unsigned, int>;

  SlotMap slotmap;

  SECTION( "add." ) {
    auto [allocated, slot] = slotmap.EmplaceDetail(1);
    REQUIRE( allocated );
    REQUIRE( slot == 0 );
    REQUIRE( slotmap[slot] == 1 );
    REQUIRE( slotmap.Size() == 1 );
    REQUIRE( slotmap.SlotSize() == 1 );
  }

  SECTION( "add, remove, add." ) {
    auto [_1, slot1] = slotmap.EmplaceDetail(1);
    slotmap.RemoveAt(slot1);
    auto [allocated, slot2] = slotmap.EmplaceDetail(2);
    REQUIRE( !allocated );
    REQUIRE( slot2 == 0 );
    REQUIRE( slotmap[slot2] == 2 );
  }

  SECTION( "test destructor count." ) {
    struct A {
      A(int &counter_) : counter(counter_) { ++counter; }
      A(A&& rhs) : counter(rhs.counter) {}
      A(const A& rhs) : counter(rhs.counter) { ++counter; }
      ~A() { --counter; }
      int &counter;
    };
    
    int counter = 0;
    {
      ecsfy::SlotMap<int, A> test;
      test.Emplace(counter);
      auto i2 = test.Emplace(counter);
      auto i3 = test.Emplace(counter);
      REQUIRE(counter == 3);
      test.RemoveAt(i2);
      REQUIRE(counter == 2);
      test.RemoveAt(i3);
      REQUIRE(counter == 1);
    }
    REQUIRE(counter == 0);
  }
}
