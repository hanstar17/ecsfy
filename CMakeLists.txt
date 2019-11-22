cmake_minimum_required(VERSION 3.14.5)
project(Ecsfy)

# compile settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Wall)

# Test
add_subdirectory(tests)

# output settings
install(TARGETS tests DESTINATION
  ${CMAKE_SOURCE_DIR}/bin)