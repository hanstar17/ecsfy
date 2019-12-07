#include "ecsfy/TypeTraits.hpp"
#include "catch2/catch.hpp"

TEST_CASE( "test CountBit", "[typetraits]" ) {
  STATIC_REQUIRE(ecsfy::CountBit(0) == 0);
  STATIC_REQUIRE(ecsfy::CountBit(1) == 1);
  STATIC_REQUIRE(ecsfy::CountBit(2) == 2);
  STATIC_REQUIRE(ecsfy::CountBit(3) == 2);
  STATIC_REQUIRE(ecsfy::CountBit(4) == 3);
  STATIC_REQUIRE(ecsfy::CountBit(15) == 4);
  STATIC_REQUIRE(ecsfy::CountBit(16) == 5);
}

TEST_CASE( "test Sum", "[typetraits]" ) {
  STATIC_REQUIRE(ecsfy::Sum(1,2,3,4,5) == 15);
  STATIC_REQUIRE(ecsfy::Sum() == 0);
}

TEST_CASE( "test AccSum", "[typetraits]" ) {
  STATIC_REQUIRE(ecsfy::AccSum<-1, 0, 1, 2, 3, 4, 5>() == 0);
  STATIC_REQUIRE(ecsfy::AccSum<0, 0, 1, 2, 3, 4, 5>() == 1);
  STATIC_REQUIRE(ecsfy::AccSum<1, 0, 1, 2, 3, 4, 5>() == 3);
  STATIC_REQUIRE(ecsfy::AccSum<2, 0, 1, 2, 3, 4, 5>() == 6);
  STATIC_REQUIRE(ecsfy::AccSum<3, 0, 1, 2, 3, 4, 5>() == 10);
  STATIC_REQUIRE(ecsfy::AccSum<4, 0, 1, 2, 3, 4, 5>() == 15);
}

TEST_CASE( "test GetAt", "[typetraits]" ) {
  STATIC_REQUIRE(ecsfy::GetAt<0, 0, 1, 2>() == 0);
  STATIC_REQUIRE(ecsfy::GetAt<1, 0, 1, 2>() == 1);
  STATIC_REQUIRE(ecsfy::GetAt<2, 0, 1, 2>() == 2);
}

TEST_CASE( "test CompactTuple", "[typetraits]" ) {
  using CompactTuple = ecsfy::CompactTuple<16, 16, 16>;
  STATIC_REQUIRE(CompactTuple::Size == 12);
  STATIC_REQUIRE(CompactTuple::UnderlyingSize == 16);

  STATIC_REQUIRE(CompactTuple::GetLocation<0>() == 0);
  STATIC_REQUIRE(CompactTuple::GetLocation<1>() == 4);
  STATIC_REQUIRE(CompactTuple::GetLocation<2>() == 8);

  STATIC_REQUIRE(CompactTuple::GetLength<0>() == 4);
  STATIC_REQUIRE(CompactTuple::GetLength<1>() == 4);
  STATIC_REQUIRE(CompactTuple::GetLength<2>() == 4);

  CompactTuple tuple{0};
  tuple.Set<0>(0);
  tuple.Set<1>(1);
  tuple.Set<2>(2);
  REQUIRE( tuple.Get<0>() == 0 );
  REQUIRE( tuple.Get<1>() == 1 );
  REQUIRE( tuple.Get<2>() == 2 );
  REQUIRE(tuple.value == 512+16);

  tuple.Set<0>(3);
  REQUIRE( tuple.Get<0>() == 3 );
  REQUIRE( tuple.Get<1>() == 1 );
  REQUIRE( tuple.Get<2>() == 2 );
  tuple.Set<1>(5);
  REQUIRE( tuple.Get<0>() == 3 );
  REQUIRE( tuple.Get<1>() == 5 );
  REQUIRE( tuple.Get<2>() == 2 );
  tuple.Set<2>(4);
  REQUIRE( tuple.Get<0>() == 3 );
  REQUIRE( tuple.Get<1>() == 5 );
  REQUIRE( tuple.Get<2>() == 4 );
  
  tuple.value = 0;
  REQUIRE( tuple.Get<0>() == 0 );
  REQUIRE( tuple.Get<1>() == 0 );
  REQUIRE( tuple.Get<2>() == 0 );
}

TEST_CASE( "is in test", "[typetraits]" ) {
  STATIC_REQUIRE( ecsfy::IsIn<float, int, float, double>() );
  STATIC_REQUIRE( ecsfy::IsIn<float, float, int, double>() );
  STATIC_REQUIRE( ecsfy::IsIn<float, int, double, float>() );
  STATIC_REQUIRE( !ecsfy::IsIn<float, int, double>() );
  STATIC_REQUIRE( !ecsfy::IsIn<float>() );
}

TEST_CASE( "is unique sequence test", "[typetraits]" ) {
  STATIC_REQUIRE( ecsfy::IsUniqueSequence<float, int, double, char>::value );
  STATIC_REQUIRE( !ecsfy::IsUniqueSequence<float, int, float, char>::value );
  STATIC_REQUIRE( !ecsfy::IsUniqueSequence<float, int, double, float>::value );
  STATIC_REQUIRE( ecsfy::IsUniqueSequence<>::value );
  STATIC_REQUIRE( ecsfy::IsUniqueSequence<float>::value );
}
