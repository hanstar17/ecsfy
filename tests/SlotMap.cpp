#include "ecsfy/SlotMap.hpp"
#include "catch2/catch.hpp"

TEST_CASE( "test slot map", "[slot map]" ) {
  using SlotMap = ecsfy::SlotMap<unsigned, int>;
  using ValueType = SlotMap::ValueType;
  using SlotType = SlotMap::SlotType;

  SlotMap slotmap;
  ValueType a = 1;
  SlotType aSlot = slotmap.Insert(a);
  ValueType &aVal = slotmap.Get(aSlot);

  REQUIRE( a == 1 );

  slotmap.Erase(aSlot);

  ValueType b = 2;
  SlotType bSlot = slotmap.Insert(b);
  REQUIRE( aSlot == bSlot );
  ValueType &bVal = slotmap.Get(bSlot);
  REQUIRE( bVal == 2 );
}
