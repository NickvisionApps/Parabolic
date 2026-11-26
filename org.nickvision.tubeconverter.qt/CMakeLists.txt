qt_standard_project_setup()

qt_add_executable("${PROJECT_NAME}.qt" WIN32 MACOSX_BUNDLE
    "include/app.h"
    "include/helpers/translation_notifier.h"
    "include/views/home_page.h"
    "include/views/main_window.h"
    "include/views/settings_dialog.h"
    "src/app.cpp"
    "src/helpers/translation_notifier.cpp"
    "src/main.cpp"
    "src/views/home_page.cpp"
    "src/views/main_window.cpp"
    "src/views/settings_dialog.cpp")
qt_add_resources("${PROJECT_NAME}.qt" "resources"
    PREFIX "/"
    BASE "${CMAKE_SOURCE_DIR}/resources"
    FILES "${CMAKE_SOURCE_DIR}/resources/org.nickvision.tubeconverter.svg")
target_include_directories("${PROJECT_NAME}.qt" PRIVATE "include")
set_target_properties("${PROJECT_NAME}.qt" PROPERTIES OUTPUT_NAME "${SHORT_NAME}")
if (WIN32)
    target_compile_options("${PROJECT_NAME}.qt" PRIVATE /W4)
else ()
    target_compile_options("${PROJECT_NAME}.qt" PRIVATE -Wall -Wextra -Wpedantic)
endif ()

if (APPLE)
    set(MACOSX_BUNDLE_EXECUTABLE_NAME "${SHORT_NAME}")
    set(MACOSX_BUNDLE_BUNDLE_NAME "${DISPLAY_NAME}")
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "${PROJECT_NAME}")
    set(MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}")
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}")
    set(MACOSX_BUNDLE_ICON_FILE "${PROJECT_NAME}.icns")
    set(MACOSX_BUNDLE_COPYRIGHT "Copyright (c) 2021-2026 Nickvision")
    set(CMAKE_OSX_DEPLOYMENT_TARGET "26.0" CACHE STRING "Minimum macOS deployment version" FORCE)
    configure_file("${CMAKE_SOURCE_DIR}/resources/macos/Info.plist.in" "${CMAKE_CURRENT_BINARY_DIR}/Info.plist" @ONLY)
    set_target_properties("${PROJECT_NAME}.qt" PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_BINARY_DIR}/Info.plist")
    include(MacosHelpers)
    generate_app_icon("${PROJECT_NAME}.qt" "${CMAKE_SOURCE_DIR}/resources/${PROJECT_NAME}.png")
endif ()

find_package(ICU REQUIRED COMPONENTS uc)
target_link_libraries("${PROJECT_NAME}.qt" PRIVATE "lib${SHORT_NAME}" ICU::uc qlementine qlementine-icons)

add_dependencies("${PROJECT_NAME}.qt" "${SHORT_NAME}_translations")
add_custom_command(TARGET "${PROJECT_NAME}.qt" POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_BINARY_DIR}/locale" "$<TARGET_FILE_DIR:${PROJECT_NAME}.qt>/locale"
    COMMENT "Copying translations"
    VERBATIM)
