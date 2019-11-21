#pragma once
#include <cassert>
#include <cstdint>
#include <utility>
#include <type_traits>
#include <limits>

namespace ecsfy {  
constexpr int CountBit(int n) {
  // 14 < 16 -> 4
  // 16 < 32 -> 5
  // 1 < 2 -> 1
  int count = 0;
  while (n) {
    n >>= 1;
    ++count;
  }
  return count;
}

template <typename... Ints>
constexpr int Sum(Ints... ints) {
  return (0 + ... + ints);
}

template <int _Index, int _Sum = 0, int _First, int... _Rest>
constexpr int AccSum() {
  if constexpr (_Index < 0)
    return _Sum;
  else if constexpr (_Index == 0)
    return _Sum + _First;
  else
    return AccSum<_Index - 1, _Sum + _First, _Rest...>();
}

template <int _Index, int _First, int... _Rest>
constexpr int GetAt() {
  static_assert(_Index >= 0);
  if constexpr (_Index == 0)
    return _First;
  else
    return GetAt<_Index - 1, _Rest...>();
}

template <int... _Values>
struct CompactTuple {
  static const int Size = Sum(CountBit(_Values)...);
  static_assert(Size > 0 && Size <= 64);
  using UnderlyingType = typename std::conditional<Size < 8, uint8_t,
                         typename std::conditional<Size < 16, uint16_t,
                         typename std::conditional<Size < 32, uint32_t, uint64_t
                         >::type
                         >::type
                         >::type;
  static const int UnderlyingSize = sizeof(UnderlyingType) * 8;

  template<int I>
  static constexpr UnderlyingType GetLocation() {
    return AccSum<I - 1, 0, CountBit(_Values)...>();
  }

  template<int I>
  static constexpr UnderlyingType GetLength() {
    return GetAt<I, CountBit(_Values)...>();
  }

  template<int I>
  static constexpr UnderlyingType GetMax() {
    constexpr UnderlyingType lshift = UnderlyingSize - GetLocation<I>() - GetLength<I>();
    constexpr UnderlyingType rshift = UnderlyingSize - GetLength<I>();
    return (std::numeric_limits<UnderlyingType>::max() << lshift) >> rshift;
  }

  template<int I>
  UnderlyingType Get() const {
    constexpr UnderlyingType lshift = UnderlyingSize - GetLocation<I>() - GetLength<I>();
    constexpr UnderlyingType rshift = UnderlyingSize - GetLength<I>();
    return (value << lshift) >> rshift;
  }

  template<int I>
  void Set(UnderlyingType elemValue) {
    constexpr UnderlyingType lshift = UnderlyingSize - GetLength<I>();
    constexpr UnderlyingType rshift = UnderlyingSize - GetLocation<I>() - GetLength<I>();
    constexpr UnderlyingType mask = (std::numeric_limits<UnderlyingType>::max() << lshift) >> rshift;
    value = (~mask & value) | ((elemValue << lshift) >> rshift);
  }

  UnderlyingType value = 0;
};

template <uint64_t N>
struct Index {
  using Type = typename std::conditional<N < 256l, uint8_t,
               typename std::conditional<N < 65536l, uint16_t,
               typename std::conditional<N < 65536l * 65536l, uint32_t, uint64_t
               >::type
               >::type
               >::type;
};
}
