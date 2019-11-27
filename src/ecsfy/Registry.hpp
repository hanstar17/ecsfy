#include <tuple>
#include "TypeTraits.hpp"
namespace ecsfy {
template <typename ...Components>
struct ComponentRegistry {
  static_assert(IsUniqueSequence<Components...>::value);

  static constexpr int Size = sizeof...(Components);
  static constexpr int ComponentSizes[Size] = {sizeof(Components)...};
  using TupleType = std::tuple<Components...>;

  template <typename Component>
  static constexpr int GetID() {
    return IndexOf<Component, Components...>();
  }

  template <int Id>
  static constexpr int GetSize() {
    static_assert(Id < Size);
    return ComponentSizes[Id];
  }

  static int GetSize(const int id) { return ComponentSizes[id]; }

  TupleType values;
};

template <typename ...Systems>
struct SystemRegistry {
  static_assert(IsUniqueSequence<Systems...>::value);

  static constexpr int Size = sizeof...(Systems);
  using TupleType = std::tuple<Systems...>;

  TupleType values;
};

}  // namespace ecsfy