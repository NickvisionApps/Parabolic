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

target_link_libraries("${PROJECT_NAME}.qt" PRIVATE "lib${SHORT_NAME}" qlementine qlementine-icons)

add_dependencies("${PROJECT_NAME}.qt" "${SHORT_NAME}_translations")
