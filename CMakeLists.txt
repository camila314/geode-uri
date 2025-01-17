cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

project(geode-url VERSION 1.0.0)

include(CPM.cmake)

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

add_subdirectory(handler-win)

add_library(${PROJECT_NAME} SHARED src/main.cpp)
add_dependencies(${PROJECT_NAME} GeodeURLHandler)


if (WIN32)
    target_link_libraries(${PROJECT_NAME} advapi32)
endif()

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR})

setup_geode_mod(${PROJECT_NAME})