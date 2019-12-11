#include <bitset>
#include <tuple>
#include "TypeTraits.hpp"
namespace ecsfy {
template <typename ...Components>
struct ComponentRegistry {
  static_assert(IsUniqueSequence<Components...>::value);

  static constexpr int Size = sizeof...(Components);
  static constexpr int ComponentSizes[Size] = {sizeof(Components)...};
  using TupleType = std::tuple<Components...>;
  using CompositionType = std::bitset<Size>;

  template <typename Component>
  static constexpr int GetIndex() {
    return IndexOf<Component, Components...>();
  }

  template <typename ...ComponentPack>
  static constexpr CompositionType GetComposition() {
    if constexpr (sizeof...(ComponentPack) == 0)
      return CompositionType();
    else
      return Compose<ComponentPack...>();
  }
  
  template <int Id>
  static constexpr int GetSize() {
    static_assert(Id < Size);
    return ComponentSizes[Id];
  }

  static int GetSize(const int id) { return ComponentSizes[id]; }

  TupleType values;

 private:
  template<typename Component, typename ...Rest>
  static constexpr CompositionType Compose() {
    CompositionType comp;
    comp.set(GetIndex<Component>(), true);
    if constexpr (sizeof...(Rest) == 0)
      return comp;
    else
      return comp | Compose<Rest...>();
  }
};

template <typename ...Systems>
struct SystemRegistry {
  static_assert(IsUniqueSequence<Systems...>::value);

  static constexpr int Size = sizeof...(Systems);
  using TupleType = std::tuple<Systems...>;

  TupleType values;
};

}  // namespace ecsfy