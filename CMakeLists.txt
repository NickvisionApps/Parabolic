cmake_minimum_required(VERSION 3.5)

project(NickvisionTubeConverter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(HELPERS helpers/filetypehelpers.h helpers/filetypehelpers.cpp)
set(MODELS models/configuration.h models/configuration.cpp models/update/version.h models/update/version.cpp models/update/updateconfig.h models/update/updateconfig.cpp models/update/updater.h models/update/updater.cpp models/filetype.h models/download.h models/download.cpp models/downloadmanager.h models/downloadmanager.cpp models/datadownloadscolumns.h models/datadownloadscolumns.cpp)
set(CONTROLS controls/headerbar.h controls/headerbar.cpp controls/infobar.h controls/infobar.cpp controls/progressdialog.h controls/progressdialog.cpp)
set(VIEWS views/mainwindow.h views/mainwindow.cpp views/settingsdialog.h views/settingsdialog.cpp)

add_executable(NickvisionTubeConverter main.cpp ${HELPERS} ${MODELS} ${CONTROLS} ${VIEWS})

target_compile_options(NickvisionTubeConverter PRIVATE -fpie)

find_package(PkgConfig)
pkg_check_modules(GTKMM_VARS REQUIRED IMPORTED_TARGET gtkmm-4.0)
pkg_check_modules(JSONCPP REQUIRED IMPORTED_TARGET jsoncpp)
pkg_check_modules(CURLPP REQUIRED curlpp)

target_link_libraries(NickvisionTubeConverter PRIVATE PkgConfig::GTKMM_VARS)
target_link_libraries(NickvisionTubeConverter PRIVATE PkgConfig::JSONCPP)
target_link_libraries(NickvisionTubeConverter PRIVATE ${CURLPP_LDFLAGS})
