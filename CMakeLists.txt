cmake_minimum_required(VERSION 3.10)

# Name and Language
project(PTS_Impl LANGUAGES CXX)

# C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(program main.cpp)
