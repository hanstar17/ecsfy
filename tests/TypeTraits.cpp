#include "ecsfy/TypeTraits.hpp"
#include "catch2/catch.hpp"

static_assert(ecsfy::CountBit(0) == 0);
static_assert(ecsfy::CountBit(1) == 1);
static_assert(ecsfy::CountBit(2) == 2);
static_assert(ecsfy::CountBit(3) == 2);
static_assert(ecsfy::CountBit(4) == 3);
static_assert(ecsfy::CountBit(15) == 4);
static_assert(ecsfy::CountBit(16) == 5);

static_assert(ecsfy::Sum(1,2,3,4,5) == 15);
static_assert(ecsfy::Sum() == 0);

static_assert(ecsfy::AccSum<-1, 0, 1, 2, 3, 4, 5>() == 0);
static_assert(ecsfy::AccSum<0, 0, 1, 2, 3, 4, 5>() == 1);
static_assert(ecsfy::AccSum<1, 0, 1, 2, 3, 4, 5>() == 3);
static_assert(ecsfy::AccSum<2, 0, 1, 2, 3, 4, 5>() == 6);
static_assert(ecsfy::AccSum<3, 0, 1, 2, 3, 4, 5>() == 10);
static_assert(ecsfy::AccSum<4, 0, 1, 2, 3, 4, 5>() == 15);

TEST_CASE( "test", "[comparisons]" ) {
    REQUIRE( 3 == 3 );
}
