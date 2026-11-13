find_program(GRADLE_EXECUTABLE NAMES gradle REQUIRED)

file(GLOB_RECURSE UI_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.kt"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.kts"
    "${CMAKE_CURRENT_SOURCE_DIR}/gradle.properties")

if(APPLE)
    set(UI_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/compose/binaries/main/app/${DISPLAY_NAME}.app")
elseif(WIN32)
    set(UI_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/compose/binaries/main/app/${DISPLAY_NAME}/${DISPLAY_NAME}.exe")
else()
    set(UI_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/compose/binaries/main/app/${DISPLAY_NAME}/bin/${DISPLAY_NAME}")
endif()

add_custom_target("${PROJECT_NAME}.ui.format" ALL
    COMMAND "${GRADLE_EXECUTABLE}" -p "${CMAKE_CURRENT_SOURCE_DIR}" spotlessApply --console=plain "-PuiBuildDir=${CMAKE_CURRENT_BINARY_DIR}"
    COMMENT "Running spotless"
    USES_TERMINAL
    VERBATIM)

add_custom_command(OUTPUT "${UI_OUTPUT}"
    COMMAND "${GRADLE_EXECUTABLE}" -p "${CMAKE_CURRENT_SOURCE_DIR}" createDistributable --console=plain --stacktrace "-PuiBuildDir=${CMAKE_CURRENT_BINARY_DIR}"
    DEPENDS ${UI_SOURCES}
    COMMENT "Building ${DISPLAY_NAME} UI with Gradle"
    USES_TERMINAL
    VERBATIM)
add_custom_target("${PROJECT_NAME}.ui" ALL DEPENDS "${UI_OUTPUT}")
add_dependencies("${PROJECT_NAME}.ui" "${PROJECT_NAME}.ui.format")
