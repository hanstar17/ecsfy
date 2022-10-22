#include "ecsfy/EntityManager.hpp"
#include "ecsfy/CompositeStorage.hpp"

namespace ecsfy {
template <typename _EntityTraits, typename _ComponentRegistry, typename _SystemRegistry>
class World {
 public:
  static constexpr int EntityCount = _EntityTraits::EntityCount;
  static constexpr int ComponentCount = _ComponentRegistry::Size;
  static constexpr int SystemCount = _SystemRegistry::Size;

  using EntityManagerType = EntityManager<_EntityTraits>;
  using CompositeStorageType = CompositeStorage<_ComponentRegistry, EntityCount>;

  Handle CreateEntity() {
    auto handle = entityManager_.Create();
    return handle;
  }

  void DestroyEntity(Handle handle) {
    entityManager_.Destroy(handle);
  }
  
 private:
  EntityManagerType entityManager_;
  CompositeStorageType compositeStorage_;
};
}  // namespace ecsfy