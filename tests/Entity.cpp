#include "ecsfy/Entity.hpp"
#include "catch2/catch.hpp"
#include "EntityTraits.hpp"

TEST_CASE( "test entity", "[entity]" ) {
  using Entity = ecsfy::Entity<EntityTraits>;

  STATIC_REQUIRE( sizeof(Entity::UnderlyingType) == 4 );
  STATIC_REQUIRE( Entity::GetMaxGeneration() == 15 );
  
  Entity e;
  e.SetIndex(3);
  e.SetTypeID(1);
  e.SetGeneration(5);

  REQUIRE( e.GetIndex() == 3 );
  REQUIRE( e.GetTypeID() == 1 );
  REQUIRE( e.GetGeneration() == 5 );
}
