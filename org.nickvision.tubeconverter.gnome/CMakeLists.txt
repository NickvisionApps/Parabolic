include(${CMAKE_SOURCE_DIR}/cmake/helpers.cmake)

if(NOT WIN32)
    set(OUTPUT_NAME "${PROJECT_NAME}.gnome")
    set(GNOME_NOTIFICATIONS true)
    set(STARTUP_NOTIFY true)
    set(DBUS_ACTIVATABLE true)

    add_executable(${OUTPUT_NAME}
        "blueprints/add_download_dialog.blp"
        "blueprints/credential_dialog.blp"
        "blueprints/download_row.blp"
        "blueprints/keyring_page.blp"
        "blueprints/main_window.blp"
        "blueprints/preferences_dialog.blp"
        "blueprints/shortcuts_dialog.blp"
        "include/controls/downloadrow.h"
        "include/helpers/builder.h"
        "include/helpers/controlbase.h"
        "include/helpers/controlptr.h"
        "include/helpers/dialogbase.h"
        "include/helpers/dialogptr.h"
        "include/helpers/gtkhelpers.h"
        "include/views/adddownloaddialog.h"
        "include/views/credentialdialog.h"
        "include/views/keyringpage.h"
        "include/views/mainwindow.h"
        "include/views/preferencesdialog.h"
        "include/application.h"
        "src/controls/downloadrow.cpp"
        "src/helpers/builder.cpp"
        "src/helpers/dialogbase.cpp"
        "src/helpers/gtkhelpers.cpp"
        "src/views/adddownloaddialog.cpp"
        "src/views/credentialdialog.cpp"
        "src/views/keyringpage.cpp"
        "src/views/mainwindow.cpp"
        "src/views/preferencesdialog.cpp"
        "src/application.cpp"
        "src/main.cpp")
    target_include_directories(${OUTPUT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(gtk REQUIRED IMPORTED_TARGET gtk4)
    pkg_check_modules(adwaita REQUIRED IMPORTED_TARGET libadwaita-1)
    pkg_check_modules(libxmlpp REQUIRED IMPORTED_TARGET libxml++-5.0)
    target_link_libraries(${OUTPUT_NAME} PRIVATE libparabolic PkgConfig::gtk PkgConfig::adwaita PkgConfig::libxmlpp)

    find_program(GLIB_COMPULE_RESOURCES_EXECUTABLE glib-compile-resources)
    if(GLIB_COMPULE_RESOURCES_EXECUTABLE)
        add_custom_command(TARGET ${OUTPUT_NAME} PRE_BUILD COMMAND "${GLIB_COMPULE_RESOURCES_EXECUTABLE}" --sourcedir "${CMAKE_CURRENT_SOURCE_DIR}/resources" "${CMAKE_CURRENT_SOURCE_DIR}/resources/${PROJECT_NAME}.gresource.xml" --target="${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.gresource")
    else()
        message(WARNING "glib-compile-resources not found, resources will not be compiled")
    endif()

    find_program(BLUEPRINT_COMPILER_EXECUTABLE blueprint-compiler)
    if(BLUEPRINT_COMPILER_EXECUTABLE)
        add_custom_command(TARGET ${OUTPUT_NAME} PRE_BUILD COMMAND "${BLUEPRINT_COMPILER_EXECUTABLE}" batch-compile "${CMAKE_CURRENT_BINARY_DIR}/ui" "${CMAKE_CURRENT_SOURCE_DIR}/blueprints" "${CMAKE_CURRENT_SOURCE_DIR}/blueprints/*.blp")
    else()
        message(FATAL_ERROR "blueprint-compiler not found, UI will not be compiled")
    endif()

    generate_translations()
    generate_unix_install()
endif()
