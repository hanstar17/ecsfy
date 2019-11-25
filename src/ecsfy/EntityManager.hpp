#include <cstdint>
#include <vector>
#include "Entity.hpp"
#include "TypeTraits.hpp"

namespace ecsfy {

// Entity Traits determines the efficiency of entity's underlying
//  struture. To maximize performance and minimize waste,
//  set them carefully.
// 
// * EntityCount - the number of entities that can coexist on run-time.
// * TypeCount - the number of entity types that can coexist on run-time.
// * GenerationCount - the number of generations each entity slot has.
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

template <typename _EntityTraits>
class EntityManager {
 public:
  using EntityType = Entity<_EntityTraits>;
  using HandleType = Handle<_EntityTraits>;
  using IndexType = typename EntityType::UnderlyingType;

 public:
  EntityType *Get(HandleType handle) {
    assert(handle.index < entities.size());
    assert(entities[handle.index].GetGeneration() == handle.generation);
    return &entities[handle.index];
  }

  HandleType Create() { return HasStale() ? Reuse() : Allocate(); }

  void Destroy(HandleType handle) { Stale(handle); }

 private:
  HandleType Allocate() {
    assert(entities.size() < _EntityTraits::EntityCount);
    entities.push_back(EntityType{});
    entities.back().SetGeneration(1);
    return {static_cast<typename HandleType::IndexType>(entities.size() - 1),
            static_cast<typename HandleType::GenerationType>(1)};
  }

  HandleType Reuse() {
    assert(HasStale());
    auto stale = entities[head].GetIndex();
    auto next = entities[stale].GetIndex();
    entities[head].SetIndex(next);

    if (stale == tail) tail = head;
    entities[stale].SetTypeID(0);
    return {static_cast<typename HandleType::IndexType>(stale),
            static_cast<typename HandleType::GenerationType>(
                entities[stale].GetGeneration())};
  }

  bool HasStale() const { return head != tail; }

  void Stale(HandleType handle) {
    assert(handle.index < entities.size());
    assert(handle.generation == entities[handle.index].GetGeneration());
    if (handle.generation == EntityType::GetMaxGeneration())
      handle.generation = 1;
    else
      ++handle.generation;

    entities[handle.index].SetGeneration(handle.generation);

    entities[tail].SetIndex(handle.index);
    tail = handle.index;
  }

 private:
  std::vector<EntityType> entities = {EntityType{0}};
  IndexType head = 0;
  IndexType tail = 0;
};

}  // namespace ecsfy