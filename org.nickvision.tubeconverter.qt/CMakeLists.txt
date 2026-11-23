qt_standard_project_setup()

qt_add_executable("${PROJECT_NAME}.qt" WIN32 MACOSX_BUNDLE
    "include/views/main_window.h"
    "src/main.cpp"
    "src/views/main_window.cpp")
target_include_directories("${PROJECT_NAME}.qt" PRIVATE "include")
set_target_properties("${PROJECT_NAME}.qt" PROPERTIES OUTPUT_NAME "${SHORT_NAME}")
target_link_libraries("${PROJECT_NAME}.qt" PRIVATE "lib${SHORT_NAME}" qlementine qlementine-icons)
if (WIN32)
    target_compile_options("${PROJECT_NAME}.qt" PRIVATE /W4)
else ()
    target_compile_options("${PROJECT_NAME}.qt" PRIVATE -Wall -Wextra -Wpedantic)
endif ()
add_dependencies("${PROJECT_NAME}.qt" "${SHORT_NAME}_translations")
