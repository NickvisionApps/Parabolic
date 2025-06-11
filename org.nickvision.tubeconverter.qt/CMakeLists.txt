include(${CMAKE_SOURCE_DIR}/cmake/helpers.cmake)

set(OUTPUT_NAME "${PROJECT_NAME}.qt")
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(GNOME_NOTIFICATIONS false)
set(STARTUP_NOTIFY false)
set(DBUS_ACTIVATABLE false)
add_definitions(-DQT_NO_KEYWORDS)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Svg)
find_package(qlementine CONFIG REQUIRED)
find_package(qlementine-icons CONFIG REQUIRED)
qt_standard_project_setup()

qt_add_executable(${OUTPUT_NAME}
    "include/controls/aboutdialog.h"
    "include/controls/downloadrow.h"
    "include/controls/historypane.h"
    "include/controls/historyrow.h"
    "include/controls/infobar.h"
    "include/controls/logpane.h"
    "include/controls/statuspage.h"
    "include/helpers/qthelpers.h"
    "include/views/adddownloaddialog.h"
    "include/views/credentialdialog.h"
    "include/views/keyringdialog.h"
    "include/views/mainwindow.h"
    "include/views/settingsdialog.h"
    "include/application.h"
    "src/controls/aboutdialog.cpp"
    "src/controls/downloadrow.cpp"
    "src/controls/historypane.cpp"
    "src/controls/historyrow.cpp"
    "src/controls/infobar.cpp"
    "src/controls/logpane.cpp"
    "src/controls/statuspage.cpp"
    "src/helpers/qthelpers.cpp"
    "src/views/adddownloaddialog.cpp"
    "src/views/credentialdialog.cpp"
    "src/views/keyringdialog.cpp"
    "src/views/mainwindow.cpp"
    "src/views/settingsdialog.cpp"
    "src/application.cpp"
    "src/main.cpp"
    "resources/${OUTPUT_NAME}.qrc"
    "../resources/windows/${PROJECT_NAME}.rc")
target_include_directories(${OUTPUT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(${OUTPUT_NAME} PRIVATE libparabolic Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Svg qlementine::qlementine qlementine-icons::qlementine-icons)
if(WIN32)
    set_target_properties(${OUTPUT_NAME} PROPERTIES WIN32_EXECUTABLE ON)
endif()

if(WIN32)
    add_custom_command(TARGET ${OUTPUT_NAME} POST_BUILD COMMAND Qt6::windeployqt ARGS $<TARGET_FILE:${OUTPUT_NAME}> --no-quick-import --no-translations --no-compiler-runtime)
    add_custom_command(TARGET ${OUTPUT_NAME} POST_BUILD COMMAND "mt.exe" -manifest "${CMAKE_SOURCE_DIR}/resources/windows/${PROJECT_NAME}.manifest" -inputresource:\"$<TARGET_FILE:${OUTPUT_NAME}>\"\;\#1 -outputresource:\"$<TARGET_FILE:${OUTPUT_NAME}>\"\;\#1)
endif()

generate_translations()
generate_linux_install()
