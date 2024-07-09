if(WIN32)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

include(CTest)
add_library(libparabolic
    "src/controllers/adddownloaddialogcontroller.cpp"
    "src/controllers/mainwindowcontroller.cpp"
    "src/controllers/preferencesviewcontroller.cpp"
    "src/helpers/pythonhelpers.cpp"
    "src/models/configuration.cpp"
    "src/models/download.cpp"
    "src/models/downloaderoptions.cpp"
    "src/models/downloadhistory.cpp"
    "src/models/downloadoptions.cpp"
    "src/models/downloadprogresschangedeventargs.cpp"
    "src/models/historicdownload.cpp"
    "src/models/media.cpp"
    "src/models/mediafiletype.cpp"
    "src/models/timeframe.cpp"
    "src/models/urlinfo.cpp"
    "src/models/videoresolution.cpp")
target_include_directories(libparabolic PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
set_target_properties(libparabolic PROPERTIES VERSION "${PROJECT_VERSION}" SOVERSION "${PROJECT_VERSION}")
if(MSVC)
    target_compile_options(libparabolic PRIVATE /W4)
else()
    target_compile_options(libparabolic PRIVATE -Wall -Wextra -Wpedantic)
endif()

find_package(libnick CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS date_time)
find_package(Python COMPONENTS Interpreter Development REQUIRED)
find_package(pybind11 CONFIG REQUIRED)
target_link_libraries(libparabolic PUBLIC libnick::libnick Boost::boost Boost::date_time pybind11::embed)

add_custom_target(shared_commands ALL DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/__shared.h")
add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/__shared.h" "${CMAKE_CURRENT_BINARY_DIR}/shared.h" COMMAND "${CMAKE_COMMAND}" -E touch "${CMAKE_CURRENT_BINARY_DIR}/shared.h")
if(HAS_GETTEXT)
    add_custom_command(TARGET shared_commands PRE_BUILD COMMAND "${GETTEXT_XGETTEXT_EXECUTABLE}" --from-code=utf-8 --add-comments --c++ --force-po --output="${POT_FILE}" --keyword=_ --keyword=_n:1,2 --keyword=_p:1c,2 --keyword=_pn:1c,2,3 --keyword=C_:1c,2 --width=80 ${TRANSLATE_FILES} WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
    foreach(LINGUA IN ITEMS ${LINGUAS})
        add_custom_command(TARGET shared_commands PRE_BUILD COMMAND "${GETTEXT_MSGMERGE_EXECUTABLE}" --backup=off -U "${CMAKE_SOURCE_DIR}/resources/po/${LINGUA}.po" "${POT_FILE}")
    endforeach()
endif()

if(HAS_GETTEXT AND ITSTOOL_EXECUTABLE AND YELP_BUILD_EXECUTABLE)
    add_custom_command(TARGET shared_commands PRE_BUILD COMMAND "${ITSTOOL_EXECUTABLE}" -o "docs/po/${SHORT_NAME}.pot" "docs/yelp/C/*.page" WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
    foreach(LINGUA IN ITEMS ${DOC_LINGUAS})
        # Update Po
        add_custom_command(TARGET shared_commands PRE_BUILD COMMAND "${GETTEXT_MSGMERGE_EXECUTABLE}" --backup=off -U "${CMAKE_SOURCE_DIR}/docs/po/${LINGUA}.po" "${DOC_POT_FILE}")
        add_custom_command(TARGET shared_commands PRE_BUILD COMMAND "${GETTEXT_MSGFMT_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/docs/po/${LINGUA}.po" --output-file="${CMAKE_SOURCE_DIR}/docs/po/${LINGUA}.mo")
        # Generate Yelp Pages
        add_custom_command(TARGET shared_commands PRE_BUILD COMMAND "${CMAKE_COMMAND}" -E make_directory "${CMAKE_SOURCE_DIR}/docs/yelp/${LINGUA}")
        add_custom_command(TARGET shared_commands PRE_BUILD COMMAND "${ITSTOOL_EXECUTABLE}" -m "${CMAKE_SOURCE_DIR}/docs/po/${LINGUA}.mo" -o "${CMAKE_SOURCE_DIR}/docs/yelp/${LINGUA}/" "${CMAKE_SOURCE_DIR}/docs/yelp/C/*.page")
        add_custom_command(TARGET shared_commands PRE_BUILD COMMAND "${CMAKE_COMMAND}" -E rm "${CMAKE_SOURCE_DIR}/docs/po/${LINGUA}.mo")
        add_custom_command(TARGET shared_commands PRE_BUILD COMMAND "${CMAKE_COMMAND}" -E make_directory "${CMAKE_SOURCE_DIR}/docs/yelp/${LINGUA}/figures")
        add_custom_command(TARGET shared_commands PRE_BUILD COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_SOURCE_DIR}/docs/yelp/C/figures" "${CMAKE_SOURCE_DIR}/docs/yelp/${LINGUA}/figures")
        # Generate HTML Pages
        add_custom_command(TARGET shared_commands PRE_BUILD COMMAND "${CMAKE_COMMAND}" -E make_directory "${CMAKE_SOURCE_DIR}/docs/html/${LINGUA}")
        add_custom_command(TARGET shared_commands PRE_BUILD COMMAND "${CMAKE_COMMAND}" -E make_directory "${CMAKE_SOURCE_DIR}/docs/html/${LINGUA}/figures")
        add_custom_command(TARGET shared_commands PRE_BUILD COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_SOURCE_DIR}/docs/yelp/${LINGUA}/figures" "${CMAKE_SOURCE_DIR}/docs/html/${LINGUA}/figures")
        add_custom_command(TARGET shared_commands PRE_BUILD COMMAND "${YELP_BUILD_EXECUTABLE}" html -o "${CMAKE_SOURCE_DIR}/docs/html/${LINGUA}/" "${CMAKE_SOURCE_DIR}/docs/yelp/${LINGUA}/")
    endforeach()
    # Generate English HTML
    add_custom_command(TARGET shared_commands PRE_BUILD COMMAND "${CMAKE_COMMAND}" -E make_directory "${CMAKE_SOURCE_DIR}/docs/html/C")
    add_custom_command(TARGET shared_commands PRE_BUILD COMMAND "${CMAKE_COMMAND}" -E make_directory "${CMAKE_SOURCE_DIR}/docs/html/C/figures")
    add_custom_command(TARGET shared_commands PRE_BUILD COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_SOURCE_DIR}/docs/yelp/C/figures" "${CMAKE_SOURCE_DIR}/docs/html/C/figures")
    add_custom_command(TARGET shared_commands PRE_BUILD COMMAND "${YELP_BUILD_EXECUTABLE}" html -o "${CMAKE_SOURCE_DIR}/docs/html/C/" "${CMAKE_SOURCE_DIR}/docs/yelp/C/")
endif()

if (BUILD_TESTING)
    add_executable(libparabolic_test
        "tests/downloadtests.cpp"
        "tests/historytests.cpp"
        "tests/main.cpp"
        "tests/urlinfotests.cpp")
    find_package(GTest CONFIG REQUIRED)
    target_link_libraries(libparabolic_test PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main libparabolic)
    if(USING_VCPKG)
        add_custom_command(TARGET libparabolic_test POST_BUILD 
            COMMAND ${Python_EXECUTABLE} -m venv "$<TARGET_FILE_DIR:libparabolic_test>/.."
            COMMAND $<TARGET_FILE_DIR:libparabolic_test>/../Scripts/activate.bat
            COMMAND python -m pip install --force-reinstall "yt-dlp==2024.07.09" > pip_install.log)
    endif()
endif()