cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(GeodeURLHandler VERSION 1.0.0)

CPMAddPackage("gh:geode-sdk/ipc@1.0.1")

add_executable(${PROJECT_NAME} src/main.cpp icon.rc)

set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE TRUE) 

target_link_libraries(${PROJECT_NAME} PRIVATE geode-ipc user32 shlwapi)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND pwsh -executionpolicy bypass -File ${CMAKE_CURRENT_SOURCE_DIR}/generate_header.ps1 $<TARGET_FILE:GeodeURLHandler> ${CMAKE_BINARY_DIR}/GeodeURLHandler.h
    BYPRODUCTS ${CMAKE_BINARY_DIR}/GeodeURLHandler.h
)