# Include GenerateExportHeader module
include(GenerateExportHeader)

# Declare files.
set(SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/animation/WidgetAnimationManager.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/animation/WidgetAnimator.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/resources/ResourceInitialization.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/style/eventFilters/ComboboxItemViewFilter.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/style/eventFilters/CommandLinkButtonEventFilter.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/style/eventFilters/DestructionEventFilter.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/style/eventFilters/LineEditButtonEventFilter.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/style/eventFilters/LineEditMenuEventFilter.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/style/eventFilters/MenuEventFilter.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/style/eventFilters/MouseWheelBlockerEventFilter.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/style/eventFilters/TabBarEventFilter.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/style/eventFilters/TextEditEventFilter.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/style/eventFilters/WidgetWithFocusFrameEventFilter.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/style/Delegates.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/style/EventFilters.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/style/EventFilters.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/style/PolishedWidgetRegistry.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/style/PolishedWidgetRegistry.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/style/PolishUtils.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/style/PolishUtils.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/style/QlementineStyle.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/style/Theme.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/style/ThemeManager.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/tools/ThemeEditor.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/BadgeUtils.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/ColorUtils.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/FontUtils.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/GeometryUtils.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/IconUtils.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/ImageUtils.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/LayoutUtils.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/MenuUtils.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/PrimitiveUtils.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/RadiusesF.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/StateUtils.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/StyleUtils.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/WidgetUtils.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/window/WindowBackend.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/window/WindowBackend.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/AboutDialog.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/AbstractItemListWidget.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/Action.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/ActionButton.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/AdwaitaTitleBarRenderer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/ColorButton.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/ColorEditor.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/CommandLinkButton.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/Expander.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/FlowLayout.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/GroupBox.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/IconWidget.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/Label.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/LineEdit.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/LoadingRing.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/LoadingSpinner.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/Menu.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/NavigationBar.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/NavigationSidebar.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/NavigationView.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/NotificationBadge.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/PageDots.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/PillButton.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/PlainTextEdit.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/Popover.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/PopoverButton.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/QlementineTitleBarRenderer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/RankingControl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/RoundedFocusFrame.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/SegmentedControl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/StatusBadgeWidget.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/Switch.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/TitleBar.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/TitleBarRenderer.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/UnifiedWindow.cpp
)

list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/SystemAccentColor.hpp)

# Platform-specific window backend used by UnifiedWindow.
if(APPLE)
  list(APPEND SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/window/MacWindowBackend.mm
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/SystemAccentColor.mm
  )
elseif(WIN32)
  list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/window/WinWindowBackend.cpp)
else()
  list(APPEND SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/window/LinuxButtonLayout.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/window/LinuxButtonLayout.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/window/LinuxWindowBackend.cpp
  )
endif()
if(NOT APPLE)
  list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/SystemAccentColor.cpp)
endif()

set(HEADERS
  ${CMAKE_CURRENT_SOURCE_DIR}/include/oclero/qlementine/animation/WidgetAnimation.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/oclero/qlementine/animation/WidgetAnimationManager.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/oclero/qlementine/animation/WidgetAnimator.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/oclero/qlementine/Common.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/oclero/qlementine/resources/ResourceInitialization.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/oclero/qlementine/style/Delegates.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/oclero/qlementine/style/QlementineStyle.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/oclero/qlementine/style/QlementineStyleOption.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/oclero/qlementine/style/Theme.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/oclero/qlementine/style/ThemeManager.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/oclero/qlementine/tools/ThemeEditor.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/oclero/qlementine/utils/BadgeUtils.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/oclero/qlementine/utils/BlurUtils.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/oclero/qlementine/utils/ColorUtils.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/oclero/qlementine/utils/FontUtils.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/oclero/qlementine/utils/GeometryUtils.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/oclero/qlementine/utils/IconUtils.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/oclero/qlementine/utils/ImageUtils.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/oclero/qlementine/utils/LayoutUtils.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/oclero/qlementine/utils/MenuUtils.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/oclero/qlementine/utils/PrimitiveUtils.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/oclero/qlementine/utils/RadiusesF.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/oclero/qlementine/utils/StateUtils.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/oclero/qlementine/utils/StyleUtils.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/oclero/qlementine/utils/WidgetUtils.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/oclero/qlementine/widgets/AboutDialog.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/oclero/qlementine/widgets/AbstractItemListWidget.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/oclero/qlementine/widgets/Action.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/oclero/qlementine/widgets/ActionButton.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/oclero/qlementine/widgets/ColorButton.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/oclero/qlementine/widgets/ColorEditor.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/oclero/qlementine/widgets/CommandLinkButton.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/oclero/qlementine/widgets/Expander.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/oclero/qlementine/widgets/FlowLayout.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/oclero/qlementine/widgets/GroupBox.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/oclero/qlementine/widgets/IconWidget.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/oclero/qlementine/widgets/Label.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/oclero/qlementine/widgets/LineEdit.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/oclero/qlementine/widgets/LoadingRing.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/oclero/qlementine/widgets/LoadingSpinner.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/oclero/qlementine/widgets/Menu.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/oclero/qlementine/widgets/NavigationBar.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/oclero/qlementine/widgets/NavigationSidebar.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/oclero/qlementine/widgets/NavigationView.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/oclero/qlementine/widgets/NotificationBadge.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/oclero/qlementine/widgets/PageDots.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/oclero/qlementine/widgets/PillButton.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/oclero/qlementine/widgets/PlainTextEdit.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/oclero/qlementine/widgets/Popover.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/oclero/qlementine/widgets/PopoverButton.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/oclero/qlementine/widgets/RankingControl.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/oclero/qlementine/widgets/RoundedFocusFrame.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/oclero/qlementine/widgets/SegmentedControl.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/oclero/qlementine/widgets/StatusBadgeWidget.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/oclero/qlementine/widgets/Switch.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/oclero/qlementine/widgets/TitleBar.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/oclero/qlementine/widgets/UnifiedWindow.hpp
)

set(RESOURCES
  resources/qlementine.qrc
  resources/qlementine_font_inter.qrc
  resources/qlementine_font_roboto.qrc
)

option(BUILD_SHARED_LIBS "Build shared libraries instead of static" OFF)
option(QLEMENTINE_BUILD_STATIC "Build static library alongside shared (when BUILD_SHARED_LIBS=ON)" OFF)

set(_build_shared ${BUILD_SHARED_LIBS})
set(_build_static ON)  # Always build static unless only shared is requested

if(BUILD_SHARED_LIBS AND NOT QLEMENTINE_BUILD_STATIC)
  set(_build_static OFF)  # Only shared
endif()

# Initialize target variables
set(_targets_to_build)
set(_main_target_name ${PROJECT_NAME})
set(_export_header_target)

# Create shared library target
if(_build_shared)
  set(_shared_target "${PROJECT_NAME}_shared")
  if(NOT _build_static)
    # If only shared, use the main project name
    set(_shared_target ${PROJECT_NAME})
  endif()

  qt_add_library(${_shared_target} SHARED
    ${HEADERS}
    ${SOURCES}
    ${RESOURCES}
  )

  # Set shared library specific properties
  set_target_properties(${_shared_target}
    PROPERTIES
      OUTPUT_NAME ${PROJECT_NAME}
      DEFINE_SYMBOL QLEMENTINE_EXPORTS
      PROJECT_LABEL "${PROJECT_NAME}_Shared"
      FOLDER lib
      SOVERSION ${PROJECT_VERSION_MAJOR}
      VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
      DEBUG_POSTFIX _debug
      CMAKE_AUTORCC ON
      CMAKE_AUTOMOC ON
      CMAKE_AUTOUIC ON
      # Use C++ visibility attributes for symbol export
      CXX_VISIBILITY_PRESET default
      VISIBILITY_INLINES_HIDDEN ON
  )

  list(APPEND _targets_to_build ${_shared_target})
  set(_export_header_target ${_shared_target})

  # Create alias for shared library
  if(_build_static)
    add_library(${PROJECT_NAME}::shared ALIAS ${_shared_target})
  else()
    # If only shared, this is the main target
    add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${_shared_target})
    set(_main_target_name ${_shared_target})
  endif()
endif()

# Create static library target
if(_build_static)
  set(_static_target "${PROJECT_NAME}_static")
  if(NOT _build_shared)
    # If only static, use the main project name
    set(_static_target ${PROJECT_NAME})
  endif()

  qt_add_library(${_static_target} STATIC
    ${HEADERS}
    ${SOURCES}
    ${RESOURCES}
  )

  # Set static library specific properties
  set_target_properties(${_static_target}
    PROPERTIES
      OUTPUT_NAME ${PROJECT_NAME}
      ARCHIVE_OUTPUT_NAME $<IF:$<BOOL:${_build_shared}>,${PROJECT_NAME}_static,${PROJECT_NAME}>
      PROJECT_LABEL "${PROJECT_NAME}_Static"
      FOLDER lib
      DEBUG_POSTFIX _debug
      CMAKE_AUTORCC ON
      CMAKE_AUTOMOC ON
      CMAKE_AUTOUIC ON
  )

  target_compile_definitions(${_static_target}
    PUBLIC
      QLEMENTINE_STATIC_DEFINE
  )

  list(APPEND _targets_to_build ${_static_target})

  if(NOT _export_header_target)
    set(_export_header_target ${_static_target})
  endif()

  # Create alias for static library
  if(_build_shared)
    add_library(${PROJECT_NAME}::static ALIAS ${_static_target})
  else()
    # If only static, this is the main target
    add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${_static_target})
    set(_main_target_name ${_static_target})
  endif()
endif()

# Create the main alias that apps will link against
# This always points to the "preferred" target (shared if available, otherwise static)
if(_build_shared)
  if(NOT TARGET ${PROJECT_NAME})
    # Create an alias from the plain name to the shared target
    add_library(${PROJECT_NAME} ALIAS ${_shared_target})
  endif()
else()
  # For static-only builds, the plain name already exists as the target
  if(NOT TARGET ${PROJECT_NAME})
    add_library(${PROJECT_NAME} ALIAS ${_static_target})
  endif()
endif()

generate_export_header(${_export_header_target}
  BASE_NAME QLEMENTINE
  EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/include/oclero/qlementine/qlementine_export.h
  EXPORT_MACRO_NAME QLEMENTINE_EXPORT
  NO_EXPORT_MACRO_NAME QLEMENTINE_NO_EXPORT
  STATIC_DEFINE QLEMENTINE_STATIC_DEFINE
)

# Configure all targets (shared and/or static)
foreach(_target IN LISTS _targets_to_build)
  if(TARGET ${_target})
    message(STATUS "Configuring target: ${_target}")

    target_include_directories(${_target}
      PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
      PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    )

    target_link_libraries(${_target}
      PUBLIC
        Qt::Core
        Qt::Widgets
        Qt::Svg
    )

    if(APPLE)
      target_link_libraries(${_target} PRIVATE "-framework AppKit")
      target_compile_options(${_target} PRIVATE $<$<COMPILE_LANGUAGE:OBJCXX>:-fobjc-arc>)
    elseif(WIN32)
      target_link_libraries(${_target} PRIVATE dwmapi user32 shell32 advapi32)
    else()
      if(TARGET Qt6::DBus)
        target_link_libraries(${_target} PRIVATE Qt::DBus)
        target_compile_definitions(${_target} PRIVATE QLEMENTINE_WITH_DBUS)
      endif()

      find_package(PkgConfig QUIET)
      if(PkgConfig_FOUND)
        pkg_check_modules(XCB QUIET IMPORTED_TARGET xcb)
        if(XCB_FOUND)
          target_link_libraries(${_target} PRIVATE PkgConfig::XCB)
          target_compile_definitions(${_target} PRIVATE QLEMENTINE_WITH_XCB)
        endif()
      endif()
    endif()

    target_compile_options(${_target}
      PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/MP /WX /W4>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Werror>
    )
  endif()
endforeach()

# Create source groups.
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}
  FILES
    ${HEADERS}
    ${SOURCES}
)

# Select correct startup project in Visual Studio.
if(WIN32)
  set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
endif()

# CMake package configuration
include(CMakePackageConfigHelpers)
configure_package_config_file("${CMAKE_CURRENT_SOURCE_DIR}/../cmake/config.cmake.in"
  "${CMAKE_BINARY_DIR}/cmake/${PROJECT_NAME}Config.cmake"
  INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")

write_basic_package_version_file("${CMAKE_BINARY_DIR}/cmake/${PROJECT_NAME}ConfigVersion.cmake"
  VERSION "${PROJECT_VERSION}"
  COMPATIBILITY AnyNewerVersion)

# Generate pkgconfig file on supported platforms
if(UNIX OR MINGW)
  configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/qlementine.pc.in"
    "${CMAKE_CURRENT_BINARY_DIR}/qlementine.pc"
    @ONLY
  )

  install(FILES "${CMAKE_CURRENT_BINARY_DIR}/qlementine.pc"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig"
    COMPONENT Development
  )
endif()

# Install targets
install(TARGETS ${_targets_to_build}
  EXPORT "${PROJECT_NAME}Targets"
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
  INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/"
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

# Install the generated export header
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/include/oclero/qlementine/qlementine_export.h"
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/oclero/qlementine/")

install(EXPORT "${PROJECT_NAME}Targets"
  FILE "${PROJECT_NAME}Targets.cmake"
  NAMESPACE ${PROJECT_NAME}::
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")

install(FILES
  "${CMAKE_BINARY_DIR}/cmake/${PROJECT_NAME}Config.cmake"
  "${CMAKE_BINARY_DIR}/cmake/${PROJECT_NAME}ConfigVersion.cmake"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")

export(EXPORT "${PROJECT_NAME}Targets"
  FILE "${CMAKE_BINARY_DIR}/cmake/${PROJECT_NAME}Targets.cmake"
  NAMESPACE ${PROJECT_NAME}::)

# Qt Style Plugin
qt_add_plugin(qlementinestyleplugin
  SHARED
  CLASS_NAME QlementineStylePlugin
  OUTPUT_NAME qlementinestyleplugin
  ${CMAKE_CURRENT_SOURCE_DIR}/src/style/QlementineStylePlugin.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/style/QlementineStylePlugin.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/style/qlementine_style_plugin.json
  ${RESOURCES}
)

target_include_directories(qlementinestyleplugin
  PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

target_link_libraries(qlementinestyleplugin
  PRIVATE
    ${PROJECT_NAME}
    Qt::Core
    Qt::Widgets
    Qt::Svg
)

set_target_properties(qlementinestyleplugin
  PROPERTIES
    FOLDER lib
    ARCHIVE_OUTPUT_NAME qlementinestyleplugin
    CMAKE_AUTORCC ON
    CMAKE_AUTOMOC ON
    CMAKE_AUTOUIC ON
)

install(TARGETS qlementinestyleplugin
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}/qt6/plugins/styles"
  RUNTIME DESTINATION "${CMAKE_INSTALL_LIBDIR}/qt6/plugins/styles"
)
