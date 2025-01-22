cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

project(GeodeURIHandler VERSION 1.0.0)

CPMAddPackage("gh:geode-sdk/ipc@1.0.1")

set(GeodeURIHandler_ICON ${CMAKE_CURRENT_SOURCE_DIR}/Icon.icns)
set_source_files_properties(${GeodeURIHandler_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

add_executable(${PROJECT_NAME} MACOSX_BUNDLE src/main.mm ${GeodeURIHandler_ICON})
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME "Geode URI Handler"
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist"
)

target_link_libraries(${PROJECT_NAME} PRIVATE geode-ipc "-framework Cocoa")

install(CODE "
    file(ARCHIVE_CREATE
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/GeodeURIHandler.zip
        FORMAT zip
        PATHS \"${CMAKE_CURRENT_BINARY_DIR}/Geode URI Handler.app\"
    )
    file(RENAME \"${CMAKE_CURRENT_BINARY_DIR}/Geode URI Handler.app/\" \"${CMAKE_CURRENT_BINARY_DIR}/folder\")
    execute_process(OUTPUT_FILE ${CMAKE_BINARY_DIR}/GeodeURIHandler.h COMMAND xxd -n GeodeURIHandler -i \"${CMAKE_CURRENT_BINARY_DIR}/GeodeURIHandler.zip\")
")

add_custom_command(DEPENDS ${PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMAND zip -r GeodeURIHandler.zip 'Geode URI Handler.app'
    COMMAND xxd -n GeodeURIHandler -i ${CMAKE_CURRENT_BINARY_DIR}/GeodeURIHandler.zip > ${CMAKE_BINARY_DIR}/GeodeURIHandler.h
    OUTPUT "${CMAKE_BINARY_DIR}/GeodeURIHandler.h"
)

add_custom_target(URIHeader DEPENDS "${CMAKE_BINARY_DIR}/GeodeURIHandler.h")
