#include "ScopedProfiler.hpp"
#include <iostream>

ScopedProfiler::ScopedProfiler(const char *tag) : tag{tag} {
  start = std::chrono::high_resolution_clock::now();
}

ScopedProfiler::~ScopedProfiler() {
  auto elapsed = std::chrono::high_resolution_clock::now() - start;
  if (tag)
    std::cout << tag << ": " << elapsed.count() << std::endl;
  else
    std::cout << "untagged: " << elapsed.count() << std::endl;
}