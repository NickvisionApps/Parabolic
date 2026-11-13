find_program(GRADLE_EXECUTABLE NAMES gradle REQUIRED)

set(UI_RESOURCES "${CMAKE_CURRENT_BINARY_DIR}/app-resources/common")

add_custom_target("${PROJECT_NAME}.ui.format" ALL
    COMMAND "${GRADLE_EXECUTABLE}" -p "${CMAKE_CURRENT_SOURCE_DIR}" spotlessApply --console=plain "-PuiBuildDir=${CMAKE_CURRENT_BINARY_DIR}"
    COMMENT "Running spotless"
    USES_TERMINAL
    VERBATIM)

add_custom_target("${PROJECT_NAME}.ui" ALL
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${UI_RESOURCES}"
    COMMAND "${CMAKE_COMMAND}" -E copy "$<TARGET_FILE:lib${SHORT_NAME}>" "${UI_RESOURCES}/${CMAKE_SHARED_LIBRARY_PREFIX}${SHORT_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}"
    COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_BINARY_DIR}/locale" "${UI_RESOURCES}/locale"
    COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_SOURCE_DIR}/resources/yt-dlp-plugins" "${UI_RESOURCES}/plugins"
    COMMAND "${GRADLE_EXECUTABLE}" -p "${CMAKE_CURRENT_SOURCE_DIR}" createDistributable --console=plain --stacktrace "-PuiBuildDir=${CMAKE_CURRENT_BINARY_DIR}"
    COMMENT "Building ${DISPLAY_NAME} UI with Gradle"
    USES_TERMINAL
    VERBATIM)
add_dependencies("${PROJECT_NAME}.ui" "lib${SHORT_NAME}" "${SHORT_NAME}_translations" "${PROJECT_NAME}.ui.format")
