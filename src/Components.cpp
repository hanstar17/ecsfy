#include "ecsfy/EntityManager.hpp"
struct Position {
  float x;
  float y;
  float z;
};

struct Rotation {
  float x;
  float y;
  float z;
  float w;
};

// template<typename _HandleTraits>
// struct Target {
//   ecsfy::Handle<_HandleTraits> value;
// };
