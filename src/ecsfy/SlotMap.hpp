#include <utility>
#include <vector>

namespace ecsfy {

template <typename ValueType, typename SlotType>
struct ReinterpretGetter {
  SlotType operator()(const ValueType &value) const {
    return *reinterpret_cast<const SlotType *>(&value);
  }
};

template <typename ValueType, typename SlotType>
struct ReinterpretSetter {
  void operator()(ValueType &value, SlotType slot) const {
    *reinterpret_cast<SlotType *>(&value) = slot;
  }
};

template <typename _ValueType, typename _SlotType,
          typename _SlotGetter = ReinterpretGetter<_ValueType, _SlotType>,
          typename _SlotSetter = ReinterpretSetter<_ValueType, _SlotType>>
class SlotMap {
  static_assert(sizeof(_ValueType) >= sizeof(_SlotType));

 public:
  using ValueType = _ValueType;
  using SlotType = _SlotType;
  using SlotGetter = _SlotGetter;
  using SlotSetter = _SlotSetter;

 public:
  ValueType &Get(SlotType slot) {
    assert(slot < slots.size());
    return slots[slot];
  }

  const ValueType &Get(SlotType slot) const {
    assert(slot < slots.size());
    return slots[slot];
  }

  const ValueType &operator[](SlotType slot) const { return Get(slot); }
  ValueType &operator[](SlotType slot) { return Get(slot); }

  template <typename ...Args>
  std::pair<bool, SlotType> Emplace(Args &&...args) {
    if (HasStale())
      return {false, Reuse(std::forward<Args>(args)...)};
    return {true, Allocate(std::forward<Args>(args)...)};
  }

  void RemoveAt(SlotType slot) { Stale(slot); }

  std::size_t GetSlotCount() const { return slots.size(); }

 private:
  template <typename ...Args>
  SlotType Allocate(Args &&...args) {
    slots.emplace_back(std::forward<Args>(args)...);
    return slots.size() - 1;
  }

  template <typename ...Args>
  SlotType Reuse(Args &&...args) {
    assert(HasStale());
    auto stale = GetNextStaleSlot(head);
    auto next = GetNextStaleSlot(stale);
    SetNextStaleSlot(head, next);

    if (stale == tail) tail = head;
    
    // construct.
    if constexpr (sizeof...(Args) == 0) {
      if constexpr (!std::is_trivially_constructible<ValueType, Args...>::value)
        new (&slots[stale]) ValueType();
    }
    else {
      static_assert(std::is_constructible<ValueType, Args...>::value);
      new (&slots[stale]) ValueType(std::forward<Args>(args)...);
    }
    
    return stale;
  }

  bool HasStale() const { return head != tail; }

  void Stale(SlotType slot) {
    assert(slot < slots.size());
    if constexpr (!std::is_trivially_destructible<ValueType>::value)
      slots[slot].~ValueType();
    SetNextStaleSlot(tail, slot);
    tail = slot;
  }

  SlotType GetNextStaleSlot(const SlotType &slot) {
    return SlotGetter()(slots[slot]);
  }

  void SetNextStaleSlot(SlotType slot, SlotType next) {
    SlotSetter()(slots[slot], next);
  }

 private:
  std::vector<ValueType> slots = {ValueType{}};
  SlotType head = 0;
  SlotType tail = 0;
};
}  // namespace ecsfy