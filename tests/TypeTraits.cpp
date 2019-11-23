#include "ecsfy/TypeTraits.hpp"
#include "catch2/catch.hpp"

TEST_CASE( "test CountBit", "[typetraits]" ) {
  static_assert(ecsfy::CountBit(0) == 0);
  static_assert(ecsfy::CountBit(1) == 1);
  static_assert(ecsfy::CountBit(2) == 2);
  static_assert(ecsfy::CountBit(3) == 2);
  static_assert(ecsfy::CountBit(4) == 3);
  static_assert(ecsfy::CountBit(15) == 4);
  static_assert(ecsfy::CountBit(16) == 5);
}

TEST_CASE( "test Sum", "[typetraits]" ) {
  static_assert(ecsfy::Sum(1,2,3,4,5) == 15);
  static_assert(ecsfy::Sum() == 0);
}

TEST_CASE( "test AccSum", "[typetraits]" ) {
  static_assert(ecsfy::AccSum<-1, 0, 1, 2, 3, 4, 5>() == 0);
  static_assert(ecsfy::AccSum<0, 0, 1, 2, 3, 4, 5>() == 1);
  static_assert(ecsfy::AccSum<1, 0, 1, 2, 3, 4, 5>() == 3);
  static_assert(ecsfy::AccSum<2, 0, 1, 2, 3, 4, 5>() == 6);
  static_assert(ecsfy::AccSum<3, 0, 1, 2, 3, 4, 5>() == 10);
  static_assert(ecsfy::AccSum<4, 0, 1, 2, 3, 4, 5>() == 15);
}

TEST_CASE( "test GetAt", "[typetraits]" ) {
  static_assert(ecsfy::GetAt<0, 0, 1, 2>() == 0);
  static_assert(ecsfy::GetAt<1, 0, 1, 2>() == 1);
  static_assert(ecsfy::GetAt<2, 0, 1, 2>() == 2);
}

TEST_CASE( "test CompactTuple", "[typetraits]" ) {
  using CompactTuple = ecsfy::CompactTuple<15, 15, 15>;
  static_assert(CompactTuple::Size == 12);
  static_assert(CompactTuple::UnderlyingSize == 16);

  static_assert(CompactTuple::GetLocation<0>() == 0);
  static_assert(CompactTuple::GetLocation<1>() == 4);
  static_assert(CompactTuple::GetLocation<2>() == 8);

  static_assert(CompactTuple::GetLength<0>() == 4);
  static_assert(CompactTuple::GetLength<1>() == 4);
  static_assert(CompactTuple::GetLength<2>() == 4);

  CompactTuple tuple;
  tuple.Set<0>(0);
  tuple.Set<1>(1);
  tuple.Set<2>(2);
  REQUIRE( tuple.Get<0>() == 0 );
  REQUIRE( tuple.Get<1>() == 1 );
  REQUIRE( tuple.Get<2>() == 2 );
}