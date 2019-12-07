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

  template <typename ForwardValueType>
  SlotType Insert(ForwardValueType &&value) {
    return HasStale() ? Reuse(std::forward<ForwardValueType>(value))
                      : Allocate(std::forward<ForwardValueType>(value));
  }

  void Erase(SlotType slot) { Stale(slot); }

 private:
  SlotType Allocate(const ValueType &value) {
    slots.push_back(value);
    return slots.size() - 1;
  }

  SlotType Allocate(ValueType &&value) {
    slots.push_back(std::move(value));
    return slots.size() - 1;
  }

  SlotType Reuse(const ValueType &value) {
    assert(HasStale());
    auto stale = GetNextStaleSlot(head);
    auto next = GetNextStaleSlot(stale);
    SetNextStaleSlot(head, next);

    if (stale == tail) tail = head;
    new (&slots[stale]) ValueType(value);
    return stale;
  }

  SlotType Reuse(ValueType &&value) {
    assert(HasStale());
    auto stale = GetNextStaleSlot(head);
    auto next = GetNextStaleSlot(stale);
    SetNextStaleSlot(head, next);

    if (stale == tail) tail = head;
    new (&slots[stale]) ValueType(std::move(value));
    return stale;
  }

  bool HasStale() const { return head != tail; }

  void Stale(SlotType slot) {
    assert(slot < slots.size());
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