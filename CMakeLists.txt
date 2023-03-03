cmake_minimum_required(VERSION 3.16)

project(contract)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_CXX_EXTENSIONS off)

add_library(contract INTERFACE)
target_include_directories(contract INTERFACE include/contract)

add_subdirectory(test)