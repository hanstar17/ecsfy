#include <chrono>

struct ScopedProfiler {
  ScopedProfiler(const char *tag = nullptr);
  ~ScopedProfiler();

 private:
  std::chrono::steady_clock::time_point start;
  const char *tag;
};