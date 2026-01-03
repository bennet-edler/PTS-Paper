cmake_minimum_required(VERSION 3.10)
project(PTS_Impl)


# C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# google test
find_package(GTest REQUIRED)
include(GoogleTest)

# header
add_library(lib 
  types.cc
  types.hpp
)
target_include_directories(lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# main program
add_executable(program main.cpp)
target_link_libraries(program lib)

# test program
add_executable(run_tests tests.cpp)
target_link_libraries(run_tests lib GTest::gtest_main)
gtest_discover_tests(run_tests)


