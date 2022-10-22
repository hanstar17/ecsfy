#include <cassert>
#include <bitset>
#include <unordered_map>
#include "ecsfy/CompositeVector.hpp"
#include "ecsfy/SlotMap.hpp"
#include "ecsfy/TypeTraits.hpp"

namespace ecsfy {

template <typename _ComponentRegistry, int Capacity>
class CompositeStorage {
 public:
  using ComponentRegistryType = _ComponentRegistry;
  using CompositionType = typename ComponentRegistryType::CompositionType;
  using CompositionIndexType = typename Index<Capacity>::Type;
  using SlotMapType = SlotMap<CompositeVector<ComponentRegistryType>, CompositionIndexType>;

 private:
  CompositionIndexType CreateComposition(const CompositionType &composition) {
    assert( indexMap.find(composition) == indexMap.end() );
    auto slot = slotmap.Emplace(GetCompositionSize(composition));
    indexMap[composition] = slot;
    return slot;
  }

  size_t GetCompositionSize(const CompositionType &composition) const {
    size_t size = 0;
    for (size_t i = 0; i < composition.size(); ++i) {
      if (composition.test(i))
        size += ComponentRegistryType::GetSize(i);
    }
    return size;
  }

 private:
  SlotMapType slotmap;
  std::unordered_map<CompositionType, CompositionIndexType> indexMap;
};
}  // namespace ecsfy