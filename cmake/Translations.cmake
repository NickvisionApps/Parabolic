function(generate_translations)
    cmake_parse_arguments(ARG "" "TARGET;SHORT_NAME;ROOT_DIRECTORY;OUTPUT_DIRECTORY" "" ${ARGN})
    if(NOT ARG_SHORT_NAME)
        message(FATAL_ERROR "generate_translations: SHORT_NAME is required")
    endif()
    if(NOT ARG_ROOT_DIRECTORY)
        set(ARG_ROOT_DIRECTORY "${CMAKE_SOURCE_DIR}")
    endif()
    if(NOT ARG_OUTPUT_DIRECTORY)
        set(ARG_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/locale")
    endif()
    find_program(XGETTEXT_EXECUTABLE NAMES xgettext REQUIRED)
    find_program(MSGMERGE_EXECUTABLE NAMES msgmerge REQUIRED)
    find_program(MSGFMT_EXECUTABLE NAMES msgfmt REQUIRED)
    set(PO_DIRECTORY "${ARG_ROOT_DIRECTORY}/resources/po")
    set(POT_FILE "${PO_DIRECTORY}/${ARG_SHORT_NAME}.pot")
    set(LINGUAS_FILE "${PO_DIRECTORY}/LINGUAS")
    if(NOT EXISTS "${LINGUAS_FILE}")
        message(FATAL_ERROR "generate_translations: ${LINGUAS_FILE} not found")
    endif()
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${LINGUAS_FILE}")
    file(GLOB_RECURSE ABSOLUTE_SOURCES CONFIGURE_DEPENDS
        "${ARG_ROOT_DIRECTORY}/lib${ARG_SHORT_NAME}/*.h"
        "${ARG_ROOT_DIRECTORY}/lib${ARG_SHORT_NAME}/*.cpp"
        "${ARG_ROOT_DIRECTORY}/${PROJECT_NAME}.*/*.h"
        "${ARG_ROOT_DIRECTORY}/${PROJECT_NAME}.*/*.cpp")
    list(SORT ABSOLUTE_SOURCES)
    set(RELATIVE_SOURCES "")
    foreach(ABSOLUTE_SOURCE IN LISTS ABSOLUTE_SOURCES)
        file(RELATIVE_PATH RELATIVE_SOURCE "${ARG_ROOT_DIRECTORY}" "${ABSOLUTE_SOURCE}")
        list(APPEND RELATIVE_SOURCES "${RELATIVE_SOURCE}")
    endforeach()
    file(STRINGS "${LINGUAS_FILE}" LINGUAS_LINES)
    set(TRANSLATION_COMMANDS
        COMMAND "${XGETTEXT_EXECUTABLE}"
            --from-code=utf-8
            --language=C++
            --force-po
            --output=${POT_FILE}
            --keyword=_
            --keyword=_n:1,2
            --keyword=_p:1c,2
            --keyword=_pn:1c,2,3
            --keyword=w_
            --keyword=w_n:1,2
            --keyword=w_p:1c,2
            --keyword=w_pn:1c,2,3
            --width=80
            ${RELATIVE_SOURCES})
    set(PO_FILES "")
    foreach(LINGUAS_LINE IN LISTS LINGUAS_LINES)
        string(STRIP "${LINGUAS_LINE}" LANGUAGE)
        if(LANGUAGE STREQUAL "" OR LANGUAGE MATCHES "^#")
            continue()
        endif()
        set(PO_FILE "${PO_DIRECTORY}/${LANGUAGE}.po")
        list(APPEND PO_FILES "${PO_FILE}")
        list(APPEND TRANSLATION_COMMANDS
            COMMAND "${CMAKE_COMMAND}" -E make_directory "${ARG_OUTPUT_DIRECTORY}/${LANGUAGE}/LC_MESSAGES"
            COMMAND "${MSGMERGE_EXECUTABLE}" --backup=off --update "${PO_FILE}" "${POT_FILE}"
            COMMAND "${MSGFMT_EXECUTABLE}" "${PO_FILE}" --output-file "${ARG_OUTPUT_DIRECTORY}/${LANGUAGE}/LC_MESSAGES/${ARG_SHORT_NAME}.mo")
    endforeach()
    set(STAMP_FILE "${CMAKE_CURRENT_BINARY_DIR}/${ARG_SHORT_NAME}_translations.stamp")
    add_custom_command(
        OUTPUT "${STAMP_FILE}"
        COMMENT "Generating translations"
        WORKING_DIRECTORY "${ARG_ROOT_DIRECTORY}"
        ${TRANSLATION_COMMANDS}
        COMMAND "${CMAKE_COMMAND}" -E touch "${STAMP_FILE}"
        DEPENDS ${ABSOLUTE_SOURCES} "${LINGUAS_FILE}" ${PO_FILES}
        VERBATIM)
    add_custom_target("${ARG_SHORT_NAME}_translations" ALL DEPENDS "${STAMP_FILE}")
    if(ARG_TARGET)
        add_dependencies("${ARG_TARGET}" "${ARG_SHORT_NAME}_translations")
    endif()
endfunction()
