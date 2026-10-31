function(generate_app_icon TARGET ICON_PNG)
    find_program(SIPS_EXECUTABLE NAMES sips REQUIRED)
    find_program(ICONUTIL_EXECUTABLE NAMES iconutil REQUIRED)
    get_filename_component(ICON_NAME "${ICON_PNG}" NAME)
    string(REGEX REPLACE "\\.png$" "" ICON_NAME "${ICON_NAME}")
    set(ICONSET_DIR "${CMAKE_CURRENT_BINARY_DIR}/${ICON_NAME}.iconset")
    set(ICNS_FILE "${CMAKE_CURRENT_BINARY_DIR}/${ICON_NAME}.icns")
    add_custom_command(
        OUTPUT "${ICNS_FILE}"
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${ICONSET_DIR}"
        COMMAND "${SIPS_EXECUTABLE}" -z 16 16 "${ICON_PNG}" --out "${ICONSET_DIR}/icon_16x16.png"
        COMMAND "${SIPS_EXECUTABLE}" -z 32 32 "${ICON_PNG}" --out "${ICONSET_DIR}/icon_16x16@2x.png"
        COMMAND "${SIPS_EXECUTABLE}" -z 32 32 "${ICON_PNG}" --out "${ICONSET_DIR}/icon_32x32.png"
        COMMAND "${SIPS_EXECUTABLE}" -z 64 64 "${ICON_PNG}" --out "${ICONSET_DIR}/icon_32x32@2x.png"
        COMMAND "${SIPS_EXECUTABLE}" -z 128 128 "${ICON_PNG}" --out "${ICONSET_DIR}/icon_128x128.png"
        COMMAND "${SIPS_EXECUTABLE}" -z 256 256 "${ICON_PNG}" --out "${ICONSET_DIR}/icon_128x128@2x.png"
        COMMAND "${SIPS_EXECUTABLE}" -z 256 256 "${ICON_PNG}" --out "${ICONSET_DIR}/icon_256x256.png"
        COMMAND "${SIPS_EXECUTABLE}" -z 512 512 "${ICON_PNG}" --out "${ICONSET_DIR}/icon_256x256@2x.png"
        COMMAND "${SIPS_EXECUTABLE}" -z 512 512 "${ICON_PNG}" --out "${ICONSET_DIR}/icon_512x512.png"
        COMMAND "${SIPS_EXECUTABLE}" -z 1024 1024 "${ICON_PNG}" --out "${ICONSET_DIR}/icon_512x512@2x.png"
        COMMAND "${ICONUTIL_EXECUTABLE}" -c icns "${ICONSET_DIR}" -o "${ICNS_FILE}"
        COMMAND "${CMAKE_COMMAND}" -E rm -rf "${ICONSET_DIR}"
        DEPENDS "${ICON_PNG}"
        VERBATIM)
    target_sources("${TARGET}" PRIVATE "${ICNS_FILE}")
    set_source_files_properties("${ICNS_FILE}" PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
endfunction()
