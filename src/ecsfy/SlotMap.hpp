#include <utility>
#include <vector>

namespace ecsfy {

template <typename ValueType, typename SlotType>
struct ReinterpretGetter {
  static_assert(sizeof(ValueType) >= sizeof(SlotType));
  SlotType operator()(const ValueType &value) const {
    return *reinterpret_cast<const SlotType *>(&value);
  }
};

template <typename ValueType, typename SlotType>
struct ReinterpretSetter {
  static_assert(sizeof(ValueType) >= sizeof(SlotType));
  void operator()(ValueType &value, SlotType slot) const {
    *reinterpret_cast<SlotType *>(&value) = slot;
  }
};

// slot map doesn't zero-initialize value type unlike some other containers
//  such as map, unordered_map if their constructor is trivially constructible.
//  when a slot is reused. Not only the calls are unncessary, there are
//  some useful usecases where persistant data in a slot can be meaningful. 
template <typename _ValueType, typename _SlotType,
          typename _SlotGetter = ReinterpretGetter<_ValueType, _SlotType>,
          typename _SlotSetter = ReinterpretSetter<_ValueType, _SlotType>>
class SlotMap {
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
  SlotType Emplace(Args &&...args) {
    if (HasStale())
      return Reuse(std::forward<Args>(args)...);
    return Allocate(std::forward<Args>(args)...);
  }

  template <typename ...Args>
  std::pair<bool, SlotType> EmplaceDetail(Args &&...args) {
    if (HasStale())
      return {false, Reuse(std::forward<Args>(args)...)};
    return {true, Allocate(std::forward<Args>(args)...)};
  }

  void RemoveAt(SlotType slot) { Stale(slot); }

  std::size_t Size() const { return size; }

  std::size_t SlotSize() const { return slots.size(); }

 private:
  template <typename ...Args>
  SlotType Allocate(Args &&...args) {
    ++size;
    slots.emplace_back(std::forward<Args>(args)...);
    return slots.size() - 1;
  }

  template <typename ...Args>
  SlotType Reuse(Args &&...args) {
    assert(HasStale());
    auto stale = head;
    auto next = GetNextStaleSlot(stale);
    head = next;

    if (stale == tail)
      hasStale = false;
    
    if constexpr (sizeof...(Args) == 0) {
      if constexpr (!std::is_trivially_constructible<ValueType, Args...>::value)
        new (&slots[stale]) ValueType();
    }
    else {
      static_assert(std::is_constructible<ValueType, Args...>::value);
      new (&slots[stale]) ValueType(std::forward<Args>(args)...);
    }
    ++size;
    return stale;
  }

  bool HasStale() const { return hasStale; }

  void Stale(SlotType slot) {
    assert(slot < slots.size());
    assert(size > 0);
    if constexpr (!std::is_trivially_destructible<ValueType>::value)
      slots[slot].~ValueType();

    if (hasStale) {
      SetNextStaleSlot(tail, slot);
      tail = slot;
    }
    else {
      head = tail = slot;
      hasStale = true;
    }
    --size;
  }

  SlotType GetNextStaleSlot(SlotType slot) {
    return SlotGetter()(slots[slot]);
  }

  void SetNextStaleSlot(SlotType slot, SlotType next) {
    SlotSetter()(slots[slot], next);
  }

 private:
  std::vector<ValueType> slots;
  SlotType head;
  SlotType tail;
  std::size_t size = 0;
  bool hasStale = false;
};
}  // namespace ecsfy