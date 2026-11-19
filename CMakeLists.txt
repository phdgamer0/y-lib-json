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
# y-- source tree to compile against; yenv passes its installed toolchain
set(YLANG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../y---lang" CACHE PATH "y-- source directory")
add_library(y-json SHARED module.cpp)
# symbols come from the ymm process at load time
if(APPLE)
    target_link_options(y-json PRIVATE -undefined dynamic_lookup)
endif()
target_include_directories(y-json PRIVATE ${YLANG_DIR}/include ${YLANG_DIR}/include/ylang ${YLANG_DIR}/src ${nlohmann_json_SOURCE_DIR}/include)
