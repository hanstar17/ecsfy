#include <iostream>
#include "ecsfy/EntityManager.hpp"

struct EntityTraits {
  static const int EntityCount = 65536;  //16 bits
  static const int TypeCount = 256;      //8 bits
  static const int GenerationCount = 16; //4 bits
};
 
template<typename _Traits>
using CompactType = ecsfy::CompactTuple<_Traits::EntityCount, 
                                        _Traits::TypeCount,
                                        _Traits::GenerationCount>;
int main() {
  ecsfy::EntityManager<EntityTraits> map;
  ecsfy::Handle<EntityTraits> handle;

  auto entity = map.Create();
  map.Destroy(entity);
  auto entity2 = map.Create();
  map.Destroy(entity2);

  using Tuple = ecsfy::CompactTuple<65535, 255, 15>;
  Tuple t;
  std::cout << Tuple::Size << std::endl;
  std::cout << sizeof(Tuple::UnderlyingType) << std::endl;
  t.Set<0>(35000);
  t.Set<1>(245);
  t.Set<2>(4);
  std::cout << t.Get<0>() << std::endl;
  std::cout << t.Get<1>() << std::endl;
  std::cout << t.Get<2>() << std::endl;
  return 0;
}
