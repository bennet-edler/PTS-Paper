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
  types.cc
  types.hpp
  gap_manager.hpp
  gap_manager.cc
  schedule.hpp
  schedule.cc
  tower_schedule.hpp
  tower_schedule.cc
)
target_include_directories(pts_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# main program
add_executable(program main.cpp)
target_link_libraries(program pts_lib)

# test program
add_executable(run_tests tests.cpp)
target_link_libraries(run_tests 
  pts_lib 
  GTest::gmock
  GTest::gtest_main
)
gtest_discover_tests(run_tests)


