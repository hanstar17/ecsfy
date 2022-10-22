#include "ecsfy/ComponentVector.hpp"
#include "ecsfy/Registry.hpp"
#include "catch2/catch.hpp"
#include <string>

TEST_CASE( "compnoent vector", "[]" ) {
  
  SECTION( "check composition." ) {
    using ComponentRegistry = ecsfy::ComponentRegistry<
        int, double, char, std::string, std::vector<int>>;
    using CompositeVector = ecsfy::CompositeVector<ComponentRegistry>;

    CompositeVector vector;
    
  }
}
