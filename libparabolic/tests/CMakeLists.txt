set(THREADS_PREFER_PTHREAD_FLAG ON)

add_executable("lib${SHORT_NAME}_tests"
    "configuration.cpp"
    "environment.cpp"
    "hash.cpp"
    "string_manip.cpp"
    "subtitle.cpp"
    "translator.cpp"
    "updater.cpp")
find_package(GTest CONFIG REQUIRED)
find_package(Threads REQUIRED)
target_link_libraries("lib${SHORT_NAME}_tests" PRIVATE "lib${SHORT_NAME}" GTest::gtest GTest::gtest_main Threads::Threads)

if(WIN32)
    add_custom_command(TARGET "lib${SHORT_NAME}_tests" POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_RUNTIME_DLLS:lib${SHORT_NAME}_tests>
        $<TARGET_FILE_DIR:lib${SHORT_NAME}_tests>
        COMMAND_EXPAND_LISTS)
endif()

include(GoogleTest)
gtest_discover_tests("lib${SHORT_NAME}_tests")