#include "ScopedProfiler.hpp"
#include <iostream>

ScopedProfiler::ScopedProfiler(const char *tag) : tag{tag} {
  start = std::chrono::high_resolution_clock::now();
}

ScopedProfiler::~ScopedProfiler() {
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
    std::chrono::high_resolution_clock::now() - start);
  if (tag)
    std::cout << tag << ": " << elapsed.count() << "ms" << std::endl;
  else
    std::cout << "untagged: " << elapsed.count() <<  "ms" << std::endl;
}