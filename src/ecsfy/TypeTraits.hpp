#pragma once
#include <cassert>
#include <cstdint>
#include <iostream>
#include <limits>
#include <type_traits>
#include <utility>

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

namespace {
template <typename Value, int Index, typename First, typename... Rest>
constexpr int IndexOfRec() {
  static_assert(Index >= 0);
  if constexpr (std::is_same<Value, First>::value)
    return Index;
  else if constexpr (sizeof...(Rest) == 0)
    return -1;
  else
    return IndexOfRec<Value, Index + 1, Rest...>();
}
}  // namespace

template <typename Value, typename... List>
constexpr int IndexOf() {
  if constexpr (sizeof...(List) == 0)
    return -1;
  else
    return IndexOfRec<Value, 0, List...>();
}

template <int... _Values>
struct CompactTuple {
  static constexpr int Size = Sum(CountBit(_Values - 1)...);
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
    return AccSum<I - 1, 0, CountBit(_Values - 1)...>();
  }

  template<int I>
  static constexpr UnderlyingType GetLength() {
    return GetAt<I, CountBit(_Values - 1)...>();
  }

  template<int I>
  static constexpr UnderlyingType GetMax() {
    constexpr int lshift = UnderlyingSize - GetLocation<I>() - GetLength<I>();
    constexpr int rshift = UnderlyingSize - GetLength<I>();
    return static_cast<UnderlyingType>(std::numeric_limits<UnderlyingType>::max() << lshift) >> rshift;
  }

  template<int I>
  UnderlyingType Get() const {
    constexpr int rshift = GetLocation<I>();
    constexpr UnderlyingType mask = GetMax<I>();
    return (value >> rshift) & mask;
  }

  template<int I>
  void Set(UnderlyingType elemValue) {
    assert(CountBit(elemValue) <= GetLength<I>());
    constexpr int lshift = UnderlyingSize - GetLength<I>();
    constexpr int rshift = UnderlyingSize - GetLocation<I>() - GetLength<I>();
    constexpr UnderlyingType mask = static_cast<UnderlyingType>(std::numeric_limits<UnderlyingType>::max() << lshift) >> rshift;
    value = (~mask & value) | (static_cast<UnderlyingType>(elemValue << lshift) >> rshift);
  }

  UnderlyingType value;
};

template <uint64_t N>
struct Index {
  using Type = typename std::conditional<N <= 256l, uint8_t,
               typename std::conditional<N <= 65536l, uint16_t,
               typename std::conditional<N <= 65536l * 65536l, uint32_t, uint64_t
               >::type
               >::type
               >::type;
};

template <typename Item>
constexpr bool IsIn() {
  return false;
}

template <typename Item, typename First, typename... Rest>
constexpr bool IsIn() {
  return std::is_same<Item, First>::value || IsIn<Item, Rest...>();
}

template <typename... Ts>
struct IsUniqueSequence : public std::true_type {};

template <typename First, typename Second, typename... Rest>
struct IsUniqueSequence<First, Second, Rest...> {
  static constexpr bool value = !std::is_same<First, Second>::value &&
                            !IsIn<First, Rest...>() &&
                            IsUniqueSequence<Second, Rest...>::value;
};
}
