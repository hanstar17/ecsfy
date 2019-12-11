#include <cstdint>
#include <vector>
#include "Entity.hpp"
#include "SlotMap.hpp"
#include "TypeTraits.hpp"

namespace ecsfy {

template <typename _Traits>
struct Handle {
  using IndexType = typename Index<_Traits::EntityCount>::Type;
  using GenerationType = typename Index<_Traits::GenerationCount>::Type;

  Handle &operator=(const Handle &rhs) = default;
  Handle &operator=(std::nullptr_t null) { generation = 0; return *this; }
  operator bool() const { return generation > 0; }

  bool operator!=(const Handle &rhs) const {
    const bool valid = *this;
    const bool rValid = rhs;
    if (valid && rValid)
      return index != rhs.index || generation != rhs.generation;
    return valid || rValid;
  }
  bool operator==(const Handle &rhs) const { return !(*this != rhs); }

  IndexType index;
  GenerationType generation = 0;
};


// Entity Traits determines the efficiency of entity's underlying
//  struture. To maximize performance and minimize waste,
//  set them carefully.
// 
// * EntityCount - the number of entities that can coexist on run-time.
// * TypeCount - the number of entity types that can coexist on run-time.
// * GenerationCount - the number of generations each entity slot has.
template <typename _EntityTraits>
class EntityManager {
 public:
  using EntityType = Entity<_EntityTraits>;
  using HandleType = Handle<_EntityTraits>;
  using IndexType = typename EntityType::UnderlyingType;

  struct SlotGetter {
    IndexType operator()(const EntityType &entity) const { return entity.GetIndex(); }
  };

  struct SlotSetter {
    void operator()(EntityType &entity, IndexType slot) const { return entity.SetIndex(slot); }
  };  
  using SlotMapType = SlotMap<EntityType, IndexType, SlotGetter, SlotSetter>;

 public:
  EntityType &Get(HandleType handle) {
    assert(handle.index < slotmap.SlotSize());
    assert(slotmap[handle.index].GetGeneration() == handle.generation);
    return slotmap[handle.index];
  }

  HandleType Create() {
    auto [newSlot, slot] = slotmap.EmplaceDetail();

    auto &entity = slotmap[slot];
    entity.SetTypeID(0);
    if (newSlot)
      entity.SetGeneration(1);

    return {slot, entity.GetGeneration()};
  }

  void Destroy(HandleType handle) {
    assert(handle.generation == slotmap[handle.index].GetGeneration());
    if (handle.generation == EntityType::GetMaxGeneration())
      handle.generation = 1;
    else
      ++handle.generation;
    
    slotmap[handle.index].SetGeneration(handle.generation);
    slotmap.RemoveAt(handle.index);
  }

 private:
  SlotMapType slotmap;
};

}  // namespace ecsfy