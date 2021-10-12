cmake_minimum_required(VERSION 3.13)
project(preprocessor_cpp)

set(CMAKE_CXX_STANDARD 17)

add_executable(preprocessor_cpp main.cpp token.h token.cpp utils.h)