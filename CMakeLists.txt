cmake_minimum_required(VERSION 3.10)
project(PTS_Impl)


# C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# google test
find_package(GTest REQUIRED)
include(GoogleTest)

# header
add_library(pts_lib 
  src/types.cc
  src/types.hpp
  src/gap_manager.hpp
  src/gap_manager.cc
  src/schedule.hpp
  src/schedule.cc
  src/tower_schedule.hpp
  src/tower_schedule.cc
  src/mcs.hpp
  src/mcs.cc
)
target_include_directories(pts_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# main program
add_executable(program src/main.cpp)
target_link_libraries(program pts_lib)

# test program
add_executable(run_tests tests/tests.cpp)
target_link_libraries(run_tests 
  pts_lib 
  GTest::gmock
  GTest::gtest_main
)
gtest_discover_tests(run_tests)


