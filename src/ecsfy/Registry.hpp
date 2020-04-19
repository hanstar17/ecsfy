#include <bitset>
#include <tuple>
#include "TypeTraits.hpp"
namespace ecsfy {
template <typename... Components>
struct ComponentRegistry {
  static_assert(IsUniqueSequence<Components...>::value);

  static constexpr int Size = sizeof...(Components);
  static constexpr int ComponentSizes[Size] = {sizeof(Components)...};

  using IndexType = typename Index<Size>::Type;
  using TupleType = std::tuple<Components...>;
  using CompositionType = std::bitset<Size>;

  template <typename Component>
  static constexpr IndexType GetIndex() {
    return IndexOf<Component, Components...>();
  }

  template <unsigned Index>
  static constexpr int GetSize() {
    static_assert(Index < Size);
    return ComponentSizes[Index];
  }

  static int GetSize(const int index) { return ComponentSizes[index]; }

  template <typename... Components2>
  static constexpr CompositionType GetComposition() {
    if constexpr (sizeof...(Components2) == 0)
      return CompositionType();
    else
      return Compose<Components2...>();
  }

  TupleType values;

 private:
  template <typename Component, typename... Rest>
  static constexpr CompositionType Compose() {
    CompositionType comp;
    comp.set(GetIndex<Component>(), true);
    if constexpr (sizeof...(Rest) == 0)
      return comp;
    else
      return comp | Compose<Rest...>();
  }
};

}  // namespace ecsfy