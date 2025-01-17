cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

project(GeodeURLHandler VERSION 1.0.0)

CPMAddPackage("gh:geode-sdk/ipc@1.0.1")

set(GeodeURLHandler_ICON ${CMAKE_CURRENT_SOURCE_DIR}/Icon.icns)
set_source_files_properties(${GeodeURLHandler_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

add_executable(${PROJECT_NAME} MACOSX_BUNDLE src/main.mm ${GeodeURLHandler_ICON})
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME "Geode URL Handler"
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist"
)

target_link_libraries(${PROJECT_NAME} PRIVATE geode-ipc "-framework Cocoa")

install(CODE "
    include(BundleUtilities)
    fixup_bundle(\"${CMAKE_CURRENT_BINARY_DIR}/Geode URL Handler.app\" \"\" \"\")
    file(ARCHIVE_CREATE
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/GeodeURLHandler.zip
        FORMAT zip
        PATHS \"${CMAKE_CURRENT_BINARY_DIR}/Geode URL Handler.app\"
    )
    file(REMOVE_RECURSE \"${CMAKE_CURRENT_BINARY_DIR}/Geode URL Handler.app/\")
    execute_process(OUTPUT_FILE ${CMAKE_BINARY_DIR}/GeodeURLHandler.h COMMAND xxd -n GeodeURLHandler -i \"${CMAKE_CURRENT_BINARY_DIR}/GeodeURLHandler.zip\")
")

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} --install ${CMAKE_BINARY_DIR} --prefix ${CMAKE_BINARY_DIR}/install
    BYPRODUCTS ${CMAKE_BINARY_DIR}/GeodeURLHandler.h
)