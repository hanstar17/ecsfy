#pragma once
#include <cassert>
#include <cstdint>
#include <utility>
#include <type_traits>
#include <limits>

#include <iostream>

namespace ecsfy {
constexpr int CountBit(int n) {
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

template <int _Index, int _Sum, int _First, int... _Rest>
constexpr int AccSum() {
  constexpr int valueCount = sizeof...(_Rest) + 1;
  static_assert(_Index < valueCount);
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
  static_assert(_Index < sizeof...(_Rest) + 1);
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

  template<unsigned I>
  static constexpr UnderlyingType GetMax() {
    constexpr int lshift = UnderlyingSize - GetLocation<I>() - GetLength<I>();
    constexpr int rshift = UnderlyingSize - GetLength<I>();
    return static_cast<UnderlyingType>(std::numeric_limits<UnderlyingType>::max() << lshift) >> rshift;
  }

  template<unsigned I>
  UnderlyingType Get() const {
    constexpr int rshift = GetLocation<I>();
    constexpr UnderlyingType mask = GetMax<I>();
    return (value >> rshift) & mask;
  }

  template<unsigned I>
  void Set(UnderlyingType elemValue) {
    assert(CountBit(elemValue) <= GetLength<I>());
    constexpr int lshift = UnderlyingSize - GetLength<I>();
    constexpr int rshift = UnderlyingSize - GetLocation<I>() - GetLength<I>();
    constexpr UnderlyingType mask = static_cast<UnderlyingType>((std::numeric_limits<UnderlyingType>::max() << lshift) >> rshift);
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
