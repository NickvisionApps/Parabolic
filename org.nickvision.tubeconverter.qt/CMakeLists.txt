qt_standard_project_setup()

qt_add_executable("${PROJECT_NAME}.qt" WIN32 MACOSX_BUNDLE
    "include/app.h"
    "include/helpers/translation_notifier.h"
    "include/views/downloads_page.h"
    "include/views/history_page.h"
    "include/views/home_page.h"
    "include/views/keyring_page.h"
    "include/views/main_window.h"
    "include/views/postprocessing_argument_dialog.h"
    "include/views/settings_dialog.h"
    "include/views/settings_discovery_page.h"
    "include/views/settings_downloads_page.h"
    "include/views/settings_extractor_page.h"
    "include/views/settings_general_page.h"
    "include/views/settings_postprocessing_page.h"
    "src/app.cpp"
    "src/helpers/translation_notifier.cpp"
    "src/main.cpp"
    "src/views/downloads_page.cpp"
    "src/views/downloads_page.ui"
    "src/views/history_page.cpp"
    "src/views/history_page.ui"
    "src/views/home_page.cpp"
    "src/views/home_page.ui"
    "src/views/keyring_page.cpp"
    "src/views/keyring_page.ui"
    "src/views/main_window.cpp"
    "src/views/main_window.ui"
    "src/views/postprocessing_argument_dialog.cpp"
    "src/views/settings_dialog.cpp"
    "src/views/settings_discovery_page.cpp"
    "src/views/settings_downloads_page.cpp"
    "src/views/settings_extractor_page.cpp"
    "src/views/settings_general_page.cpp"
    "src/views/settings_postprocessing_page.cpp")
qt_add_resources("${PROJECT_NAME}.qt" "resources"
    PREFIX "/"
    BASE "${CMAKE_SOURCE_DIR}/resources"
    FILES "${CMAKE_SOURCE_DIR}/resources/org.nickvision.tubeconverter.svg")
target_include_directories("${PROJECT_NAME}.qt" PRIVATE "include")
set_target_properties("${PROJECT_NAME}.qt" PROPERTIES OUTPUT_NAME "${DISPLAY_NAME}")
if (WIN32)
    target_compile_options("${PROJECT_NAME}.qt" PRIVATE /W4)
else ()
    target_compile_options("${PROJECT_NAME}.qt" PRIVATE -Wall -Wextra -Wpedantic)
endif ()

if (APPLE)
    set_target_properties("${PROJECT_NAME}.qt" PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER "${PROJECT_NAME}"
        MACOSX_BUNDLE_BUNDLE_NAME "${DISPLAY_NAME}"
        MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}"
        MACOSX_BUNDLE_COPYRIGHT "Copyright (c) 2021-2026 Nickvision"
        MACOSX_BUNDLE_ICON_FILE "${PROJECT_NAME}.icns")
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
