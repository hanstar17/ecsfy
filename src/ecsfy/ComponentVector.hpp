#include <memory>
#include <unordered_map>
#include <vector>

namespace ecsfy {

class IComponentVector {
 public:
  virtual ~IComponentVector() = default;
};

template <typename Component>
class ComponentVector : public IComponentVector {
  using ComponentType = Component;

 private:
  std::vector<ComponentType> components;
};

template <class _ComponentRegistry>
class CompositeVector {
  using ComponentRegistryType = _ComponentRegistry;
  using ComponentIndexType = typename ComponentRegistryType::IndexType;

 public:
  template <typename Component>
  ComponentVector<Component>& GetVector() {
    return *static_cast<ComponentVector<Component>*>(
        vectors[ComponentRegistryType::GetIndex<Component>().get());
  }

 private:
  std::unordered_map<ComponentIndexType, std::unique_ptr<IComponentVector>>
      vectors;
};

// inline void test() { CompositeVector<size_t> a; }

}  // namespace ecsfy