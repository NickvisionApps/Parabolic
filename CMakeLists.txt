#Project Settings
cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O1")
set(CMAKE_INSTALL_PREFIX "/usr")
project(NickvisionTubeConverter LANGUAGES CXX)

#Source Files
set(HELPERS helpers/curlhelpers.h helpers/curlhelpers.cpp)
set(MODELS models/configuration.h models/configuration.cpp)
set(UI ui/application.h ui/application.cpp ui/widget.h ui/widget.cpp)
set(XML ui/controls/progressdialog.xml ui/controls/progresstracker.xml ui/controls/comboboxdialog.xml ui/views/menuhelp.xml ui/views/mainwindow.xml ui/views/preferencesdialog.xml ui/views/shortcutsdialog.xml)
set(CONTROLS ui/controls/progressdialog.h ui/controls/progressdialog.cpp ui/controls/progresstracker.h ui/controls/progresstracker.cpp ui/controls/comboboxdialog.h ui/controls/comboboxdialog.cpp)
set(VIEWS ui/views/mainwindow.h ui/views/mainwindow.cpp ui/views/preferencesdialog.h ui/views/preferencesdialog.cpp ui/views/shortcutsdialog.h ui/views/shortcutsdialog.cpp)

#Create Exe
add_executable(org.nickvision.tubeconverter main.cpp ${HELPERS} ${MODELS} ${UI} ${XML} ${CONTROLS} ${VIEWS})

#Find Libraries
find_package(PkgConfig)
find_package(Threads REQUIRED)
pkg_check_modules(GTK REQUIRED IMPORTED_TARGET gtk4)
pkg_check_modules(ADWAITA REQUIRED IMPORTED_TARGET libadwaita-1)
pkg_check_modules(JSONCPP REQUIRED IMPORTED_TARGET jsoncpp)
pkg_check_modules(CURLPP REQUIRED curlpp)

#Link Libraries
target_link_libraries(org.nickvision.tubeconverter PRIVATE Threads::Threads)
target_link_libraries(org.nickvision.tubeconverter PRIVATE PkgConfig::GTK)
target_link_libraries(org.nickvision.tubeconverter PRIVATE PkgConfig::ADWAITA)
target_link_libraries(org.nickvision.tubeconverter PRIVATE PkgConfig::JSONCPP)
target_link_libraries(org.nickvision.tubeconverter PRIVATE ${CURLPP_LDFLAGS})

#Generate GResources
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/GCR_CMake/macros)
include(GlibCompileResourcesSupport)
compile_gresources(RESOURCE_FILE XML_OUT TYPE BUNDLE PREFIX /org/nickvision/tubeconverter RESOURCES ${XML})
add_custom_target(resource ALL DEPENDS ${RESOURCE_FILE})

#Installl
install(FILES ${CMAKE_BINARY_DIR}/resources.gresource DESTINATION share/org.nickvision.tubeconverter)
install(FILES ${CMAKE_BINARY_DIR}/resources.gresource.xml DESTINATION share/org.nickvision.tubeconverter)
install(FILES ${CMAKE_SOURCE_DIR}/org.nickvision.tubeconverter.desktop DESTINATION share/applications/)
install(FILES ${CMAKE_BINARY_DIR}/org.nickvision.tubeconverter PERMISSIONS WORLD_EXECUTE WORLD_WRITE WORLD_READ DESTINATION bin)