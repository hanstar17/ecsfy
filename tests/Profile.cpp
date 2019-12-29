#include "ecsfy/SlotMap.hpp"
#include "catch2/catch.hpp"
#include "ScopedProfiler.hpp"
#include <chrono>

template <unsigned CN, unsigned IN>
struct ArraysOfComponents {
  float value[CN][IN];
  float get(unsigned itemIndex, unsigned componentIndex) const {
    return value[componentIndex][itemIndex];
  }

  void set(unsigned itemIndex, unsigned componentIndex, float v) {
    value[componentIndex][itemIndex] = v;
  }

  constexpr unsigned size() const { return IN; }
  constexpr unsigned itemSize() const { return CN; }
};

template <unsigned CN, unsigned IN>
struct ArrayOfStructs {
  float value[IN][CN];
  float get(unsigned itemIndex, unsigned componentIndex) const {
    return value[itemIndex][componentIndex];
  }

  void set(unsigned itemIndex, unsigned componentIndex, float v) {
    value[itemIndex][componentIndex] = v;
  }
  
  constexpr unsigned size() const { return IN; }
  constexpr unsigned itemSize() const { return CN; }
};

template <typename C>
void AccessOne(C &container) {
  for (int i = 0; i < container.size(); ++i) {
    container.get(i, 0);
    container.set(i, 0, 1);
  }
}

template <typename C>
void AccessAll(C &container) {
  for (int i = 0; i < container.size(); ++i) {
    for (int j = 0; j < container.itemSize(); ++j) {
      container.get(i, j);
      container.set(i, j, 1);
    }
  }
}

template <typename C>
void AccessQuaterSparse(C &container) {
  for (int i = 0; i < container.size(); ++i) {
    for (int j = 0; j < container.itemSize() / 8; ++j) {
      container.get(i, j);
      container.get(i, container.itemSize() - j - 1);
      container.set(i, j, 0);
      container.set(i, container.itemSize() - j - 1, 0);
    }
  }
}

template <typename C>
void AccessQuaterCompact(C &container) {
  for (int i = 0; i < container.size(); ++i) {
    for (int j = 0; j < container.itemSize() / 4; ++j) {
      container.get(i, j);
      container.set(i, j, 0);
    }
  }
}

static constexpr int kComponentCount = 30;
static constexpr int kItemCount = 10000000;

using A = ArraysOfComponents<kComponentCount, kItemCount>;
using B = ArrayOfStructs<kComponentCount, kItemCount>;

A a;
B b;
TEST_CASE( "access patterns and loops", "[profile]" ) {
  SECTION("access one") {
    {
        ScopedProfiler s("access one a");
        AccessOne(a);
    }

    {
        ScopedProfiler s("access one b");
        AccessOne(b);
    }
  }

  SECTION("access all") {
    {
        ScopedProfiler s("access all b");
        AccessAll(a);
    }

    {
        ScopedProfiler s("access all b");
        AccessAll(b);
    }
  }

  SECTION("access quater compact") {
    {
        ScopedProfiler s("access quater a");
        AccessQuaterCompact(a);
    }

    {
        ScopedProfiler s("access quater b");
        AccessQuaterCompact(b);
    }
  }

  SECTION("access quater sparse") {
    {
        ScopedProfiler s("access quater sparse a");
        AccessQuaterSparse(a);
    }

    {
        ScopedProfiler s("access quater sparse b");
        AccessQuaterSparse(b);
    }
  }
}
