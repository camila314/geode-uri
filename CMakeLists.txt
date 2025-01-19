cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

project(geode-uri VERSION 1.0.0 LANGUAGES CXX)

include(CPM.cmake)

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

add_library(${PROJECT_NAME} SHARED src/platform.cpp src/GeodeURI.cpp ${CMAKE_BINARY_DIR}/GeodeURIHandler.h)
add_dependencies(${PROJECT_NAME} URIHeader)
target_include_directories(${PROJECT_NAME} PUBLIC include/)
if (WIN32)
    add_subdirectory(handler-win)
    target_link_libraries(${PROJECT_NAME} advapi32)
elseif (APPLE)
    enable_language(OBJCXX)
    set_source_files_properties(src/platform.cpp PROPERTIES LANGUAGE OBJCXX)
    add_subdirectory(handler-mac)
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
else()
    message(FATAL_ERROR "Invalid Platform")
endif()

if (PROJECT_IS_TOP_LEVEL)
    target_compile_definitions(${PROJECT_NAME} PRIVATE -DURI_TL=1)
endif()

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR})

setup_geode_mod(${PROJECT_NAME})