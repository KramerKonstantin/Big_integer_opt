cmake_minimum_required(VERSION 3.10)
project(big_int_opt)

include_directories(${BIGINT_SOURCE_DIR})

set(CMAKE_CXX_STANDARD 11)

add_executable(big_int_opt
        big_integer.cpp
        big_integer.h
        big_integer_testing.cpp
        gtest.h gtest-all.cpp
        gtest-main.cpp
        my_vector.cpp
        my_vector.h)

target_link_libraries(big_int_opt -lpthread)