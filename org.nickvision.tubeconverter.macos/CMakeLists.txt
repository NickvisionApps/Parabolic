set(MACOSX_BUNDLE_EXECUTABLE_NAME "${DISPLAY_NAME}")
set(MACOSX_BUNDLE_BUNDLE_NAME "${DISPLAY_NAME}")
set(MACOSX_BUNDLE_GUI_IDENTIFIER "${PROJECT_NAME}")
set(MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}")
set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}")
set(MACOSX_BUNDLE_ICON_FILE "${PROJECT_NAME}.icns")
set(MACOSX_BUNDLE_COPYRIGHT "Copyright (c) 2021-2026 Nickvision")
set(CMAKE_OSX_DEPLOYMENT_TARGET "26.0" CACHE STRING "Minimum macOS deployment version" FORCE)
configure_file("${CMAKE_SOURCE_DIR}/resources/macos/Info.plist.in" "${CMAKE_CURRENT_BINARY_DIR}/Info.plist" @ONLY)

add_executable("${PROJECT_NAME}.macos"
    "include/delegate.h"
    "src/main.mm"
    "src/delegate.mm")
set_target_properties("${PROJECT_NAME}.macos" PROPERTIES
    OUTPUT_NAME "${DISPLAY_NAME}"
    XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC YES
    XCODE_ATTRIBUTE_CLANG_ENABLE_MODULES YES
    XCODE_ATTRIBUTE_CODE_SIGN_STYLE "Manual"
    XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "-"
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_BINARY_DIR}/Info.plist")
target_compile_options("${PROJECT_NAME}.macos" PRIVATE -Wall -Wextra -Wpedantic -Wno-elaborated-enum-base -fobjc-arc)
target_include_directories("${PROJECT_NAME}.macos" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

find_library(FOUNDATION_FRAMEWORK Foundation REQUIRED)
find_library(COCOA_FRAMEWORK Cocoa REQUIRED)
target_link_libraries("${PROJECT_NAME}.macos" PRIVATE "lib${SHORT_NAME}" ${FOUNDATION_FRAMEWORK} ${COCOA_FRAMEWORK})

include(MacosHelpers)
generate_app_icon("${PROJECT_NAME}.macos" "${CMAKE_SOURCE_DIR}/resources/${PROJECT_NAME}.png")
