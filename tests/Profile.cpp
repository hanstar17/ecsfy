#include "ecsfy/SlotMap.hpp"
#include "catch2/catch.hpp"
#include "ScopedProfiler.hpp"
#include <chrono>
#include <thread>
#include <array>
#include <vector>

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

template <typename _Container, std::size_t N, typename _Func, typename... _Args>
void TestSequential(const char *tag, std::array<_Container, N> &containers, _Func func, _Args&&... args) {
  ScopedProfiler s(tag);
  for (auto &c : containers) {
    func(c, std::forward(args)...);
  }
}

template <typename _Container, std::size_t N, typename _Func, typename... _Args>
void TestParallel(const char *tag, std::array<_Container, N> &containers, _Func func, _Args &&... args) {
  ScopedProfiler s(tag);
  std::vector<std::thread> threads; threads.reserve(N);
  for (auto &c : containers) {
    threads.push_back(std::thread(func, std::ref(c), std::forward(args)...));
  }
  for (auto &t : threads) {
    t.join();
  }
}

static constexpr int kComponentCount = 20;
static constexpr int kItemCount = 1000000;
static constexpr int kBlockCount = 7;

using A = ArraysOfComponents<kComponentCount, kItemCount>;
using B = ArrayOfStructs<kComponentCount, kItemCount>;

std::array<A, kBlockCount> as;
std::array<B, kBlockCount> bs;
TEST_CASE( "access patterns and loops", "[profile]" ) {
  TestSequential("AccessOne A", as, AccessOne<A>);
  TestSequential("AccessOne B", bs, AccessOne<B>);
  TestParallel("AccessOne A (p)", as, AccessOne<A>);
  TestParallel("AccessOne B (p)", bs, AccessOne<B>);

  TestSequential("AccessAll A", as, AccessAll<A>);
  TestSequential("AccessAll B", bs, AccessAll<B>);
  TestParallel("AccessAll A (p)", as, AccessAll<A>);
  TestParallel("AccessAll B (p)", bs, AccessAll<B>);

  TestSequential("AccessQuaterCompact A", as, AccessQuaterCompact<A>);
  TestSequential("AccessQuaterCompact B", bs, AccessQuaterCompact<B>);
  TestParallel("AccessQuaterCompact A (p)", as, AccessQuaterCompact<A>);
  TestParallel("AccessQuaterCompact B (p)", bs, AccessQuaterCompact<B>);

  TestSequential("AccessQuaterSparse A", as, AccessQuaterSparse<A>);
  TestSequential("AccessQuaterSparse B", bs, AccessQuaterSparse<B>);
  TestParallel("AccessQuaterSparse A (p)", as, AccessQuaterSparse<A>);
  TestParallel("AccessQuaterSparse B (p)", bs, AccessQuaterSparse<B>);
}
