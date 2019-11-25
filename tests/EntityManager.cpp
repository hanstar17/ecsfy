#include "ecsfy/EntityManager.hpp"
#include "EntityTraits.hpp"
#include "catch2/catch.hpp"

namespace {
  using Handle = ecsfy::Handle<EntityTraits>;
 Handle MakeHandle(Handle::IndexType index, Handle::GenerationType generation = 1) {
  return {index, generation};
}
}

TEST_CASE( "test handle", "[entity handle]" ) {
  using Handle = ecsfy::Handle<EntityTraits>;

  SECTION( "default handle nullity." ) {
    Handle defaultHandle;
    REQUIRE_FALSE(defaultHandle);
  }

  SECTION( "valid handle nullity." ) {
    Handle validHandle = MakeHandle(0);
    REQUIRE(validHandle);
  }

  SECTION( "comparison test - index" ) {
    Handle validHandle1 = MakeHandle(1);
    Handle validHandle2 = MakeHandle(2);
    REQUIRE( validHandle1 != validHandle2 );
    REQUIRE_FALSE( validHandle1 == validHandle2 );
  }

  SECTION( "comparison test - generation" ) {
    Handle validHandle1 = MakeHandle(1, 1);
    Handle validHandle2 = MakeHandle(1, 2);
    REQUIRE( validHandle1 != validHandle2 );
    REQUIRE_FALSE( validHandle1 == validHandle2 );
  }

  SECTION( "comparison test - null" ) {
    Handle validHandle1 = MakeHandle(1, 0);
    Handle validHandle2 = MakeHandle(2, 0);
    Handle validHandle3 = MakeHandle(3, 1);
    Handle validHandle4 = MakeHandle(1, 0);
    REQUIRE( validHandle1 == validHandle2 ); // null and null
    REQUIRE( validHandle2 != validHandle3 ); // null and valid
    REQUIRE( validHandle3 != validHandle4 ); // valid and null
    REQUIRE( validHandle4 == validHandle1 ); // null and null
  }

  SECTION( "assign valid handle" ) {
    Handle handle = MakeHandle(0);
    handle = nullptr;

    Handle validHandle1 = MakeHandle(1);
    Handle validHandle2 = MakeHandle(2);
    validHandle1 = validHandle2;
    REQUIRE(validHandle1 == validHandle2);
  }

  SECTION( "assign null handle" ) {
    Handle validHandle1 = MakeHandle(1);
    Handle validHandle2;
    validHandle1 = validHandle2;
    REQUIRE_FALSE( validHandle1 );
  }
}

TEST_CASE( "test manager", "[entity manager]" ) {
  using Manager = ecsfy::EntityManager<SmallEntityTraits>;
  using Handle = Manager::HandleType;
  Manager manager;
  std::vector<Handle> handles;
  // create test
  handles.push_back(manager.Create());
  REQUIRE( handles.back().index == 1 );
  REQUIRE( handles.back().generation == 1 );

  handles.push_back(manager.Create());
  REQUIRE( handles.back().index == 2 );
  REQUIRE( handles.back().generation == 1 );

  handles.push_back(manager.Create());
  REQUIRE( handles.back().index == 3 );
  REQUIRE( handles.back().generation == 1 );

  // get
  REQUIRE( manager.Get(handles[0]) );
  REQUIRE( manager.Get(handles[0])->GetGeneration() == 1 );
  
  REQUIRE( manager.Get(handles[1]) );
  REQUIRE( manager.Get(handles[1])->GetGeneration() == 1 );

  REQUIRE( manager.Get(handles[2]) );
  REQUIRE( manager.Get(handles[2])->GetGeneration() == 1 );

  // destroy
  manager.Destroy(handles[0]);
  manager.Destroy(handles[1]);
  manager.Destroy(handles[2]);

  // recreate
  handles[0] = manager.Create();
  REQUIRE( handles[0].index == 1 );
  REQUIRE( handles[0].generation == 2 );
  handles[1] = manager.Create();
  REQUIRE( handles[1].index == 2 );
  REQUIRE( handles[1].generation == 2 );
  handles[2] = manager.Create();
  REQUIRE( handles[2].index == 3 );
  REQUIRE( handles[2].generation == 2 );

  // generation wraps around
  {
    int repeat = 2;
    while (repeat--) {
      manager.Destroy(handles[0]);
      handles[0] = manager.Create();
    }
    REQUIRE( handles[0].generation == 1 );
  }
}