cmake_minimum_required(VERSION 3.16)
project(y-lib-json)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
include(FetchContent)
set(JSON_BuildTests OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    nlohmann_json
    URL https://github.com/nlohmann/json/archive/refs/tags/v3.11.3.zip
)
FetchContent_MakeAvailable(nlohmann_json)
add_library(y-json SHARED module.cpp)
target_include_directories(y-json PRIVATE ../y---lang/include ../y---lang/src ${nlohmann_json_SOURCE_DIR}/include)
