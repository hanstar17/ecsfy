#include <cstdint>
#include <vector>
#include "Entity.hpp"
#include "TypeTraits.hpp"

namespace ecsfy {

// Entity Traits determines the efficiency of entity's underlying
//  struture. To maximize performance and minimize waste,
//  set this carefully.
//
// MaxEntityCount lets the system know the maximum number of entities possible
//  at any point in runtime. To minimize waste you can specify it to be a
//  number tht is a pow of 2 minus 1 (e.g. 65535 = 2^16 - 1), where one entity
//  is consumed for null entity.
//
// MaxTypeCount tells how many different "types" of entities can exist at
//  any point in runtime. The same rule applies as above.
//
// MaxGeneration specifies entity's wrap-around generations.
//  The same rule applies as above.
template <typename _Traits>
struct Handle {
  using IndexType = typename Index<_Traits::MaxEntityCount>::Type;
  using GenerationType = typename Index<_Traits::MaxGeneration>::Type;

  Handle &operator=(const Handle &rhs) = default;
  Handle &operator=(std::nullptr_t null) { generation = 0; }
  operator bool() const { return generation > 0; }

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
    assert(entities[handle.index].generation() == handle.generation);
    return &entities[handle.index];
  }

  HandleType Create() { return HasStale() ? Reuse() : Allocate(); }

  void Destroy(HandleType &handle) {
    Stale(handle);
    handle.generation = 0;
    handle.index = 0;
  }

 private:
  HandleType Allocate() {
    assert(entities.size() < _EntityTraits::MaxEntityCount);
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
    if (++handle.generation == EntityType::GetMaxGeneration())
      handle.generation = 1;
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