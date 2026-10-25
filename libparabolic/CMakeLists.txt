if(WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

add_library("lib${SHORT_NAME}"
    "include/application_state.h"
    "include/browser.h"
    "include/configuration.h"
    "include/environment.h"
    "include/file.h"
    "include/hash.h"
    "include/history.h"
    "include/media.h"
    "include/post_processor.h"
    "include/product_info.h"
    "include/string_manip.h"
    "include/subtitle.h"
    "include/translator.h"
    "include/updater.h"
    "src/application_state.cpp"
    "src/configuration.cpp"
    "src/environment.cpp"
    "src/file.cpp"
    "src/hash.cpp"
    "src/history.cpp"
    "src/media.cpp"
    "src/post_processor.cpp"
    "src/product_info.cpp"
    "src/string_manip.cpp"
    "src/subtitle.cpp"
    "src/translator.cpp"
    "src/updater.cpp")
target_include_directories("lib${SHORT_NAME}" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
set_target_properties("lib${SHORT_NAME}" PROPERTIES VERSION "${PROJECT_VERSION}" SOVERSION "${PROJECT_VERSION}")
if(MSVC)
    target_compile_options("lib${SHORT_NAME}" PRIVATE /W4)
else()
    target_compile_options("lib${SHORT_NAME}" PRIVATE -Wall -Wextra -Wpedantic)
endif()

find_package(cpr REQUIRED)
find_package(Intl REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(platform_folders REQUIRED)
find_package(semver CONFIG REQUIRED)
target_link_libraries("lib${SHORT_NAME}" PUBLIC cpr::cpr Intl::Intl nlohmann_json::nlohmann_json OpenSSL::SSL OpenSSL::Crypto sago::platform_folders semver::semver)
if(APPLE)
    target_link_libraries("lib${SHORT_NAME}" PUBLIC "-framework CoreFoundation")
endif()