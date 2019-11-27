#include "ecsfy/Registry.hpp"
#include "catch2/catch.hpp"
#include <string>

TEST_CASE( "component registry", "[registry]" ) {
  STATIC_REQUIRE( ecsfy::ComponentRegistry<int, double, char, std::string, std::vector<int>>::Size == 5 );
  STATIC_REQUIRE( ecsfy::ComponentRegistry<int, double, std::string, std::vector<int>>::Size == 4 );

  using ComponentRegistry = ecsfy::ComponentRegistry<int, double, char, unsigned, unsigned char>;
  SECTION( "get a component's ID." ) {
    STATIC_REQUIRE( ComponentRegistry::GetID<int>() == 0 );
    STATIC_REQUIRE( ComponentRegistry::GetID<double>() == 1 );
    STATIC_REQUIRE( ComponentRegistry::GetID<char>() == 2 );
    STATIC_REQUIRE( ComponentRegistry::GetID<unsigned>() == 3 );
    STATIC_REQUIRE( ComponentRegistry::GetID<unsigned char>() == 4 );
    STATIC_REQUIRE( ComponentRegistry::GetID<std::string>() == -1 );
  }

  SECTION( "check component's size." ) {
    STATIC_REQUIRE( ComponentRegistry::GetSize<0>() == sizeof(int) );
    STATIC_REQUIRE( ComponentRegistry::GetSize<1>() == sizeof(double) );
    STATIC_REQUIRE( ComponentRegistry::GetSize<2>() == sizeof(char) );
  
    REQUIRE( ComponentRegistry::GetSize(0) == sizeof(int) );
    REQUIRE( ComponentRegistry::GetSize(1) == sizeof(double) );
    REQUIRE( ComponentRegistry::GetSize(2) == sizeof(char) );
  }
}
