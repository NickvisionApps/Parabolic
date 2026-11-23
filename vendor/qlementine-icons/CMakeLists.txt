project("qlementine-icons" LANGUAGES CXX VERSION 1.16.1.0 DESCRIPTION "Modern icon theme for Qt applications.")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_CLANG_TIDY "")
set(CMAKE_SKIP_INSTALL_RULES ON)
set(BUILD_SHARED_LIBS OFF)

add_subdirectory(sources EXCLUDE_FROM_ALL)
