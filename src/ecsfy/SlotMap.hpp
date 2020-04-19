#include <utility>
#include <vector>

namespace ecsfy {

template <class IndexType, class ValueType>
struct SlotMap_ReinterpretGetter {
  static_assert(sizeof(ValueType) >= sizeof(IndexType));
  IndexType operator()(const ValueType &value) const {
    return *reinterpret_cast<const IndexType *>(&value);
  }
};

template <class IndexType, class ValueType>
struct SlotMap_ReinterpretSetter {
  static_assert(sizeof(ValueType) >= sizeof(IndexType));
  void operator()(ValueType &value, IndexType slot) const {
    *reinterpret_cast<IndexType *>(&value) = slot;
  }
};

template<class IndexType, class ValueType, class NextGetter, class NextSetter>
struct SlotMap_Slot {
  template <typename ...Args>
  void Construct(Args &&...args) {
    assert(!_occupied);
    _occupied = true;
    if constexpr (sizeof...(Args) == 0) {
      if constexpr (!std::is_trivially_constructible_v<ValueType, Args...>)
        new (&GetValue()) ValueType();
    }
    else {
      static_assert(std::is_constructible_v<ValueType, Args...>);
      new (&GetValue()) ValueType(std::forward<Args>(args)...);
    }
  }
  
  void Destruct() {
    assert(_occupied);
    if constexpr (!std::is_trivially_destructible_v<ValueType>)
      GetValue().~ValueType();
    _occupied = false;
  }

  ValueType& GetValue() {
    assert(_occupied);
    return GetRawValue();
  }

  const ValueType& GetValue() const {
    assert(_occupied);
    return GetRawValue();
  }

  bool IsOccupied() const { return _occupied; }
  void SetOccupied(bool occupied) { _occupied = occupied; }

  IndexType GetNext() const {
    assert(!_occupied);
    return NextGetter()(GetRawValue());
  }
  
  void SetNext(IndexType index) {
    assert(!_occupied);
    NextSetter()(GetRawValue(), index);
  }

 private:
  ValueType& GetRawValue() {
    return *reinterpret_cast<ValueType*>(_value);
  }

  const ValueType& GetRawValue() const {
    return *reinterpret_cast<const ValueType*>(_value);
  }

 private:
  std::byte _value[sizeof(ValueType)];
  bool _occupied = false;
};

// slot map doesn't zero-initialize value type unlike some other containers
//  such as map, unordered_map if their constructor is trivially constructible.
//  when a slot is reused. Not only the calls are unncessary, there are
//  some useful usecases where persistant data in a slot can be meaningful. 
template <class _IndexType, class _ValueType,
          class _SlotGetter = SlotMap_ReinterpretGetter<_IndexType, _ValueType>,
          class _SlotSetter = SlotMap_ReinterpretSetter<_IndexType, _ValueType>>
class SlotMap {
 public:
  using IndexType = _IndexType;
  using ValueType = _ValueType;
  using SlotType = SlotMap_Slot<IndexType, ValueType, _SlotGetter, _SlotSetter>;

 public:
  ~SlotMap() {
    if constexpr (!std::is_trivially_destructible_v<ValueType>) {
      for (auto &slot : slots) {
        if (slot.IsOccupied())
          slot.Destruct();
      }
    }
  }

  ValueType &Get(IndexType slot) {
    assert(slot < slots.size());
    return slots[slot].GetValue();
  }

  const ValueType &Get(IndexType slot) const {
    assert(slot < slots.size());
    return slots[slot].GetValue();
  }

  ValueType &operator[](IndexType slot) { return Get(slot); }
  const ValueType &operator[](IndexType slot) const { return Get(slot); }

  template <typename ...Args>
  IndexType Emplace(Args &&...args) {
    if (HasStale())
      return Reuse(std::forward<Args>(args)...);
    return Allocate(std::forward<Args>(args)...);
  }

  template <typename ...Args>
  std::pair<bool, IndexType> EmplaceDetail(Args &&...args) {
    if (HasStale())
      return {false, Reuse(std::forward<Args>(args)...)};
    return {true, Allocate(std::forward<Args>(args)...)};
  }

  void RemoveAt(IndexType slot) { Stale(slot); }

  std::size_t Size() const { return size; }

  std::size_t SlotSize() const { return slots.size(); }

 private:
  template <typename ...Args>
  IndexType Allocate(Args &&...args) {
    ++size;

    slots.emplace_back();
    slots.back().Construct(std::forward<Args>(args)...);
    return slots.size() - 1;
  }

  template <typename ...Args>
  IndexType Reuse(Args &&...args) {
    assert(HasStale());
    auto stale = head;
    auto next = slots[stale].GetNext();
    head = next;

    slots[stale].Construct(std::forward<Args>(args)...);
    ++size;

    return stale;
  }

  bool HasStale() const { return size < slots.size(); }

  void Stale(IndexType slot) {
    assert(slot < slots.size());
    assert(size > 0);
    slots[slot].Destruct();

    if (HasStale()) {
      slots[tail].SetNext(slot);
      tail = slot;
    }
    else {
      head = tail = slot;
    }
    --size;
  }

 private:
  std::vector<SlotType> slots;
  IndexType head;
  IndexType tail;
  std::size_t size = 0;
};
}  // namespace ecsfy