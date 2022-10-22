#include "ecsfy/CompositeStorage.hpp"
#include "ecsfy/Registry.hpp"
#include "EntityTraits.hpp"
#include "catch2/catch.hpp"

TEST_CASE( "test composite storage", "[composite storage]" ) {
  using ComponentRegistry = ecsfy::ComponentRegistry<
    int,
    double,
    char,
    unsigned char
  >;
  using CompositeStorage = ecsfy::CompositeStorage<ComponentRegistry, 256>;
  using Composition = CompositeStorage::CompositionType;

  // Composition comp;
  // comp.set(0, true);
  // comp.set(1, true);
  // comp.set(3, true);
  // CompositeStorage storage;
}