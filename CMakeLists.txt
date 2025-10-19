cmake_minimum_required(VERSION 3.20)
project(canpid C CXX)

# Compiler standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)


# Main library 
add_library(canpid_objs
  src/can_bus.c
  src/pid.c
  src/fsm.c
  src/plant.c
  src/cli.c
)
target_include_directories(canpid_objs PUBLIC ${CMAKE_SOURCE_DIR}/src)

# Main executable
add_executable(canpid src/main.c)
target_link_libraries(canpid PRIVATE canpid_objs)

# Unit tests (using GoogleTest)
include(CTest)
if(BUILD_TESTING)
  include(FetchContent)

  # Fetch GoogleTest automatically (no manual install needed)
  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0   # stable release
  )
  FetchContent_MakeAvailable(googletest)

  # Test executable
  add_executable(canpid_tests
    tests/test_pid.cpp
    tests/test_can.cpp
  )

  target_link_libraries(canpid_tests PRIVATE
    canpid_objs
    GTest::gtest_main
  )

  include(GoogleTest)
  gtest_discover_tests(canpid_tests)
endif()
