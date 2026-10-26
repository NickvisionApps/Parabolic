if (WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif ()

add_library("lib${SHORT_NAME}"
    "include/application.h"
    "include/audio.h"
    "include/batch.h"
    "include/configuration.h"
    "include/cookies.h"
    "include/environment.h"
    "include/file.h"
    "include/hash.h"
    "include/history.h"
    "include/keyring.h"
    "include/m3u.h"
    "include/postprocessing.h"
    "include/string_manip.h"
    "include/subtitle.h"
    "include/thumbnail.h"
    "include/translation.h"
    "include/updates.h"
    "include/video.h"
    "include/ytdlp.h"
    "src/application.cpp"
    "src/audio.cpp"
    "src/batch.cpp"
    "src/configuration.cpp"
    "src/environment.cpp"
    "src/file.cpp"
    "src/hash.cpp"
    "src/history.cpp"
    "src/keyring.cpp"
    "src/m3u.cpp"
    "src/postprocessing.cpp"
    "src/string_manip.cpp"
    "src/subtitle.cpp"
    "src/thumbnail.cpp"
    "src/translation.cpp"
    "src/updates.cpp"
    "src/video.cpp"
    "src/ytdlp.cpp")
target_include_directories("lib${SHORT_NAME}" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
set_target_properties("lib${SHORT_NAME}" PROPERTIES VERSION "${PROJECT_VERSION}" SOVERSION "${PROJECT_VERSION}")
if (WIN32)
    target_compile_options("lib${SHORT_NAME}" PRIVATE /W4)
elseif (APPLE)
    target_compile_options("lib${SHORT_NAME}" PRIVATE -Wall -Wextra -Wpedantic -Wno-elaborated-enum-base)
else ()
    target_compile_options("lib${SHORT_NAME}" PRIVATE -Wall -Wextra -Wpedantic)
endif ()

find_package(cpr REQUIRED)
find_package(Intl REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(platform_folders REQUIRED)
find_package(semver CONFIG REQUIRED)
find_package(SQLiteCpp REQUIRED)
target_link_libraries("lib${SHORT_NAME}" PUBLIC cpr::cpr Intl::Intl nlohmann_json::nlohmann_json OpenSSL::SSL OpenSSL::Crypto sago::platform_folders semver::semver SQLiteCpp)
if (WIN32)
    target_link_libraries("lib${SHORT_NAME}" PUBLIC Advapi32)
elseif (APPLE)
    find_library(CF_LIBRARY CoreFoundation REQUIRED)
    find_library(SECURITY_LIBRARY Security REQUIRED)
    target_link_libraries("lib${SHORT_NAME}" PUBLIC "${CF_LIBRARY}" "${SECURITY_LIBRARY}")
else ()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(libsecret REQUIRED IMPORTED_TARGET libsecret-1)
    target_link_libraries("lib${SHORT_NAME}" PUBLIC PkgConfig::libsecret)
endif ()

include(EmbedResource)
embed_resource("lib${SHORT_NAME}" "${CMAKE_SOURCE_DIR}/resources/default_thumbnail.jpg")