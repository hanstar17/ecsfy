#include "ecsfy/SlotMap.hpp"
#include "catch2/catch.hpp"

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
}
