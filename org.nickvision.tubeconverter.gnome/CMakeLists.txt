if(NOT WIN32)
    set(OUTPUT_NAME "${PROJECT_NAME}.gnome")
    set(GNOME_NOTIFICATIONS true)
    set(STARTUP_NOTIFY true)
    set(DBUS_ACTIVATABLE true)

    add_executable(${OUTPUT_NAME}
        "src/controls/downloadrow.cpp"
        "src/helpers/builder.cpp"
        "src/helpers/dialogbase.cpp"
        "src/helpers/gtkhelpers.cpp"
        "src/views/adddownloaddialog.cpp"
        "src/views/credentialdialog.cpp"
        "src/views/keyringpage.cpp"
        "src/views/mainwindow.cpp"
        "src/views/preferencesdialog.cpp"
        "src/application.cpp"
        "src/main.cpp")
    target_include_directories(${OUTPUT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(gtk REQUIRED IMPORTED_TARGET gtk4)
    pkg_check_modules(adwaita REQUIRED IMPORTED_TARGET libadwaita-1)
    pkg_check_modules(libxmlpp REQUIRED IMPORTED_TARGET libxml++-5.0)
    target_link_libraries(${OUTPUT_NAME} PRIVATE libparabolic PkgConfig::gtk PkgConfig::adwaita PkgConfig::libxmlpp)

    add_custom_target(gnome_commands ALL DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/__gnome.h")
    add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/__gnome.h" "${CMAKE_CURRENT_BINARY_DIR}/gnome.h" COMMAND "${CMAKE_COMMAND}" -E touch "${CMAKE_CURRENT_BINARY_DIR}/gnome.h")

    find_program(GLIB_COMPULE_RESOURCES_EXECUTABLE glib-compile-resources)
    if(GLIB_COMPULE_RESOURCES_EXECUTABLE)
        add_custom_command(TARGET gnome_commands PRE_BUILD COMMAND "${GLIB_COMPULE_RESOURCES_EXECUTABLE}" --sourcedir "${CMAKE_CURRENT_SOURCE_DIR}/resources" "${CMAKE_CURRENT_SOURCE_DIR}/resources/${PROJECT_NAME}.gresource.xml" --target="${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.gresource")
    else()
        message(WARNING "glib-compile-resources not found, resources will not be compiled")
    endif()

    find_program(BLUEPRINT_COMPILER_EXECUTABLE blueprint-compiler)
    if(BLUEPRINT_COMPILER_EXECUTABLE)
        add_custom_command(TARGET gnome_commands PRE_BUILD COMMAND "${BLUEPRINT_COMPILER_EXECUTABLE}" batch-compile "${CMAKE_CURRENT_BINARY_DIR}/ui" "${CMAKE_CURRENT_SOURCE_DIR}/blueprints" "${CMAKE_CURRENT_SOURCE_DIR}/blueprints/*.blp")
    else()
        message(FATAL_ERROR "blueprint-compiler not found, UI will not be compiled")
    endif()

    if(HAS_GETTEXT)
        foreach(LINGUA IN ITEMS ${LINGUAS})
            add_custom_command(TARGET gnome_commands PRE_BUILD COMMAND "${CMAKE_COMMAND}" -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/${LINGUA}/LC_MESSAGES")
            add_custom_command(TARGET gnome_commands PRE_BUILD COMMAND "${GETTEXT_MSGFMT_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/resources/po/${LINGUA}.po" --output-file="${CMAKE_CURRENT_BINARY_DIR}/${LINGUA}/LC_MESSAGES/${SHORT_NAME}.mo")
        endforeach()
    endif()

    configure_file("${CMAKE_SOURCE_DIR}/resources/linux/${PROJECT_NAME}.desktop.in" "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.desktop" @ONLY)
    configure_file("${CMAKE_SOURCE_DIR}/resources/linux/${PROJECT_NAME}.in" "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}" @ONLY)
    configure_file("${CMAKE_SOURCE_DIR}/resources/linux/${PROJECT_NAME}.metainfo.xml.in" "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.metainfo.xml" @ONLY)
    configure_file("${CMAKE_SOURCE_DIR}/resources/linux/${PROJECT_NAME}.service.in" "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.service" @ONLY)
    install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/" DESTINATION "${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}")
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT_NAME}" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE DESTINATION "${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}")
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE DESTINATION "${CMAKE_INSTALL_BINDIR}")
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.desktop" DESTINATION "${CMAKE_INSTALL_DATADIR}/applications")
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.metainfo.xml" DESTINATION "${CMAKE_INSTALL_DATADIR}/metainfo")
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.service" DESTINATION "${CMAKE_INSTALL_DATADIR}/dbus-1/services")
    install(FILES "${CMAKE_SOURCE_DIR}/resources/${PROJECT_NAME}.svg" "${CMAKE_SOURCE_DIR}/resources/${PROJECT_NAME}-devel.svg" DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps")
    install(FILES "${CMAKE_SOURCE_DIR}/resources/${PROJECT_NAME}-symbolic.svg" DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/symbolic/apps")

    if(HAS_YELP)
        install(DIRECTORY "${CMAKE_SOURCE_DIR}/docs/yelp/C/" DESTINATION "${CMAKE_INSTALL_DATADIR}/help/C/${SHORT_NAME}")
        foreach(LINGUA IN ITEMS ${DOC_LINGUAS})
            install(DIRECTORY "${CMAKE_SOURCE_DIR}/docs/yelp/${LINGUA}/" DESTINATION "${CMAKE_INSTALL_DATADIR}/help/${LINGUA}/${SHORT_NAME}")
        endforeach()
    endif()

    find_program(GTK_UPDATE_ICON_CACHE_EXECUTABLE gtk-update-icon-cache)
    if(GTK_UPDATE_ICON_CACHE_EXECUTABLE)
        install(CODE "execute_process(COMMAND ${GTK_UPDATE_ICON_CACHE_EXECUTABLE})")
    else()
        message(WARNING "gtk-update-icon-cache not found, icons will not be updated")
    endif()

    find_program(UPDATE_DESKTOP_DATABASE_EXECUTABLE update-desktop-database)
    if(UPDATE_DESKTOP_DATABASE_EXECUTABLE)
        install(CODE "execute_process(COMMAND ${UPDATE_DESKTOP_DATABASE_EXECUTABLE})")
    else()
        message(WARNING "update-desktop-database not found, desktop database will not be updated")
    endif()
endif()
