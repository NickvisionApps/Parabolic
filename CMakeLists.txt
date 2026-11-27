cmake_minimum_required(VERSION 4.3)
project(org.nickvision.tubeconverter)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(DEFINED ENV{VCPKG_ROOT})
    file(TO_CMAKE_PATH $ENV{VCPKG_ROOT} VCPKG_ROOT)
    if(EXISTS "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
        set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
        set(USING_VCPKG ON)
    endif()
endif()

if (POLICY CMP0141)
    cmake_policy(SET CMP0141 NEW)
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project("org.nickvision.tubeconverter" LANGUAGES CXX VERSION 2026.11.0 DESCRIPTION "Download web video and audio.")
set(SHORT_NAME "parabolic")
set(DISPLAY_NAME "Parabolic")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(GNUInstallDirs)

option(ENABLE_CLANG_TIDY "Run clang-tidy during compilation" ON)
find_program(CLANG_TIDY_EXECUTABLE NAMES clang-tidy)
if(ENABLE_CLANG_TIDY AND CLANG_TIDY_EXECUTABLE)
    set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXECUTABLE}")
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Svg)
if(NOT WIN32 AND NOT APPLE)
    find_package(Qt6 OPTIONAL_COMPONENTS DBus)
endif()

add_subdirectory("lib${SHORT_NAME}")
add_subdirectory("vendor/qlementine")
add_subdirectory("vendor/qlementine-icons")
add_subdirectory("${PROJECT_NAME}.qt")

include(Translations)
generate_translations(TARGET "lib${SHORT_NAME}" SHORT_NAME "${SHORT_NAME}" SOURCE_DIRECTORIES "lib${SHORT_NAME}" "${PROJECT_NAME}.qt")

find_program(CLANG_FORMAT_EXECUTABLE NAMES clang-format)
if(CLANG_FORMAT_EXECUTABLE)
    file(GLOB_RECURSE FORMAT_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_SOURCE_DIR}/lib${SHORT_NAME}/*.h"
        "${CMAKE_SOURCE_DIR}/lib${SHORT_NAME}/*.cpp"
        "${CMAKE_SOURCE_DIR}/${PROJECT_NAME}.qt/*.h"
        "${CMAKE_SOURCE_DIR}/${PROJECT_NAME}.qt/*.cpp")
    add_custom_target(format ALL COMMAND "${CLANG_FORMAT_EXECUTABLE}" -i ${FORMAT_SOURCES} COMMENT "Running clang-format")
    add_dependencies("lib${SHORT_NAME}" format)
    add_dependencies("${PROJECT_NAME}.qt" format)
endif()
