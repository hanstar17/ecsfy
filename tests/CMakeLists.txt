add_executable(tests Main.cpp Entity.cpp EntityManager.cpp Registry.cpp TypeTraits.cpp)

target_include_directories(tests PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/external/catch2/include)
