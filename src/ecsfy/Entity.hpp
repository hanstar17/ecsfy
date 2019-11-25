#include <cstdint>
#include "TypeTraits.hpp"

namespace ecsfy {
template <typename _Traits>
struct Entity {
 private:
  enum {
    kIndex,
    kTypeID,
    kGeneration
  };

 public:
  using CompactType = CompactTuple<_Traits::EntityCount,
                                   _Traits::TypeCount,
                                   _Traits::GenerationCount>;
  using UnderlyingType = typename CompactType::UnderlyingType;
  
  static constexpr UnderlyingType GetMaxGeneration() { return CompactType::template GetMax<kGeneration>(); }

  UnderlyingType GetIndex() const { return value.template Get<kIndex>(); }
  void SetIndex(UnderlyingType v) { value.template Set<kIndex>(v); }

  UnderlyingType GetTypeID() const { return value.template Get<kTypeID>(); }
  void SetTypeID(UnderlyingType v) { value.template Set<kTypeID>(v); }

  UnderlyingType GetGeneration() const { return value.template Get<kGeneration>(); }
  void SetGeneration(UnderlyingType v) { value.template Set<kGeneration>(v); }
  
  CompactType value;
};
}  // namespace ecsfy