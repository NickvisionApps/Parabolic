if(WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    if(BUILD_AS_PORTABLE)
        add_definitions(-DPORTABLE_BUILD)
    endif()
endif()

add_library(libparabolic
    "include/controllers/adddownloaddialogcontroller.h"
    "include/controllers/credentialdialogcontroller.h"
    "include/controllers/keyringdialogcontroller.h"
    "include/controllers/mainwindowcontroller.h"
    "include/controllers/preferencesviewcontroller.h"
    "include/events/downloadaddedeventargs.h"
    "include/events/downloadcompletedeventargs.h"
    "include/events/downloadcredentialneededeventargs.h"
    "include/events/downloadprogresschangedeventargs.h"
    "include/models/audiocodec.h"
    "include/models/batchfile.h"
    "include/models/batchfileentry.h"
    "include/models/browser.h"
    "include/models/configuration.h"
    "include/models/credentialcheckstatus.h"
    "include/models/download.h"
    "include/models/downloaderoptions.h"
    "include/models/downloadhistory.h"
    "include/models/downloadmanager.h"
    "include/models/downloadoptions.h"
    "include/models/downloadrecoveryqueue.h"
    "include/models/downloadstatus.h"
    "include/models/executable.h"
    "include/models/format.h"
    "include/models/formatvalue.h"
    "include/models/historicdownload.h"
    "include/models/historylength.h"
    "include/models/m3u.h"
    "include/models/media.h"
    "include/models/mediafiletype.h"
    "include/models/mediatype.h"
    "include/models/pathtype.h"
    "include/models/postprocessor.h"
    "include/models/postprocessorargument.h"
    "include/models/postprocessorargumentcheckstatus.h"
    "include/models/previousdownloadoptions.h"
    "include/models/startupinformation.h"
    "include/models/subtitleformat.h"
    "include/models/subtitlelanguage.h"
    "include/models/theme.h"
    "include/models/timeframe.h"
    "include/models/videocodec.h"
    "include/models/urlinfo.h"
    "include/models/videoresolution.h"
    "include/models/ytdlpmanager.h"
    "src/controllers/adddownloaddialogcontroller.cpp"
    "src/controllers/credentialdialogcontroller.cpp"
    "src/controllers/keyringdialogcontroller.cpp"
    "src/controllers/mainwindowcontroller.cpp"
    "src/controllers/preferencesviewcontroller.cpp"
    "src/events/downloadaddedeventargs.cpp"
    "src/events/downloadcompletedeventargs.cpp"
    "src/events/downloadcredentialneededeventargs.cpp"
    "src/events/downloadprogresschangedeventargs.cpp"
    "src/models/batchfile.cpp"
    "src/models/batchfileentry.cpp"
    "src/models/configuration.cpp"
    "src/models/download.cpp"
    "src/models/downloaderoptions.cpp"
    "src/models/downloadhistory.cpp"
    "src/models/downloadmanager.cpp"
    "src/models/downloadoptions.cpp"
    "src/models/downloadrecoveryqueue.cpp"
    "src/models/format.cpp"
    "src/models/historicdownload.cpp"
    "src/models/m3u.cpp"
    "src/models/media.cpp"
    "src/models/mediafiletype.cpp"
    "src/models/postprocessorargument.cpp"
    "src/models/previousdownloadoptions.cpp"
    "src/models/startupinformation.cpp"
    "src/models/subtitlelanguage.cpp"
    "src/models/timeframe.cpp"
    "src/models/urlinfo.cpp"
    "src/models/videoresolution.cpp"
    "src/models/ytdlpmanager.cpp")
target_include_directories(libparabolic PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
set_target_properties(libparabolic PROPERTIES VERSION "${PROJECT_VERSION}" SOVERSION "${PROJECT_VERSION}")
if(MSVC)
    target_compile_options(libparabolic PRIVATE /W4)
else()
    target_compile_options(libparabolic PRIVATE -Wall -Wextra -Wpedantic)
endif()

find_package(libnick CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS date_time)
target_link_libraries(libparabolic PUBLIC libnick::libnick Boost::boost Boost::date_time)
