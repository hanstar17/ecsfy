add_library(testlib Entity.cpp)

add_executable(tests Main.cpp)
target_link_libraries(tests PRIVATE
  testlib)

target_include_directories(Tests PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  src
  ${CMAKE_SOURCE_DIR}/external/catch2/include)

enable_testing()
