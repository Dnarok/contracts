cmake_minimum_required(VERSION 3.16)

project(contract_test LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY  https://github.com/catchorg/Catch2.git
    GIT_TAG         v3.3.2
)
FetchContent_MakeAvailable(Catch2)

add_executable(contract_test test.cpp)
target_link_libraries(contract_test PRIVATE Catch2::Catch2WithMain)
target_include_directories(contract_test PUBLIC ${CMAKE_SOURCE_DIR}/include)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)
catch_discover_tests(contract_test)