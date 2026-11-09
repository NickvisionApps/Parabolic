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
    "include/app_delegate.h"
    "include/main_menu_controller.h"
    "include/main_window_controller.h"
    "include/settings_aria2_view_controller.h"
    "include/settings_dialog_controller.h"
    "include/settings_discovery_view_controller.h"
    "include/settings_downloads_view_controller.h"
    "include/settings_extractor_view_controller.h"
    "include/settings_general_view_controller.h"
    "include/settings_history_view_controller.h"
    "include/settings_postprocessing_argument_editor_view_controller.h"
    "include/settings_postprocessing_arguments_view_controller.h"
    "include/settings_postprocessing_view_controller.h"
    "include/settings_tab_view_controller.h"
    "src/app_delegate.mm"
    "src/main.mm"
    "src/main_menu_controller.mm"
    "src/main_window_controller.mm"
    "src/settings_aria2_view_controller.mm"
    "src/settings_dialog_controller.mm"
    "src/settings_discovery_view_controller.mm"
    "src/settings_downloads_view_controller.mm"
    "src/settings_extractor_view_controller.mm"
    "src/settings_general_view_controller.mm"
    "src/settings_history_view_controller.mm"
    "src/settings_postprocessing_argument_editor_view_controller.mm"
    "src/settings_postprocessing_arguments_view_controller.mm"
    "src/settings_postprocessing_view_controller.mm"
    "src/settings_tab_view_controller.mm")
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
