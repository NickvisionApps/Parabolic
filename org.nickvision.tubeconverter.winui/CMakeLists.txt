include(${CMAKE_SOURCE_DIR}/cmake/helpers.cmake)

if(WIN32)
    set(OUTPUT_NAME "${PROJECT_NAME}.winui")
    set(GNOME_NOTIFICATIONS false)
    set(STARTUP_NOTIFY false)

    cmake_policy(SET CMP0091 NEW)
    set(CMAKE_MSVC_RUNTIME_LIBRARYc"MultiThreaded$<$<CONFIG:Debug>:Debug>")
    add_link_options("/DEFAULTLIB:ucrt$<$<CONFIG:Debug>:d>.lib" "/NODEFAULTLIB:libucrt$<$<CONFIG:Debug>:d>.lib")
    set(CMAKE_VS_GLOBALS
        "AppxPackage=false"
        "RuntimeIdentifier=win10-x64"
        "RuntimeIdentifiers=win10-x64"
        "BaseNuGetRuntimeIdentifier=win10"
        "AssetTargetFallback=$(AssetTargetFallback);native")

    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/Directory.Build.props" "${CMAKE_CURRENT_BINARY_DIR}/Directory.Build.props" COPYONLY)
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/Directory.Build.targets" "${CMAKE_CURRENT_BINARY_DIR}/Directory.Build.targets" COPYONLY)

    add_executable(${OUTPUT_NAME} WIN32
        "../resources/windows/${PROJECT_NAME}.manifest"
        "../resources/windows/${PROJECT_NAME}.rc"
        "pch.cpp" "pch.h"
        "Controls/AboutDialog.xaml.cpp" "Controls/AboutDialog.xaml.h" "Controls/AboutDialog.xaml" "Controls/AboutDialog.idl"
        "Controls/SettingsRow.xaml.cpp" "Controls/SettingsRow.xaml.h" "Controls/SettingsRow.xaml" "Controls/SettingsRow.idl"
        "Controls/StatusPage.xaml.cpp" "Controls/StatusPage.xaml.h" "Controls/StatusPage.xaml" "Controls/StatusPage.idl"
        "Controls/TitleBar.xaml.cpp" "Controls/TitleBar.xaml.h" "Controls/TitleBar.xaml" "Controls/TitleBar.idl"
        "Controls/ViewStack.xaml.cpp" "Controls/ViewStack.xaml.h" "Controls/ViewStack.xaml" "Controls/ViewStack.idl"
        "Helpers/WinUIHelpers.h"
        "Views/AddDownloadDialog.xaml.cpp" "Views/AddDownloadDialog.xaml.h" "Views/AddDownloadDialog.xaml" "Views/AddDownloadDialog.idl"
        "Views/MainWindow.xaml.cpp" "Views/MainWindow.xaml.h" "Views/MainWindow.xaml" "Views/MainWindow.idl"
        "Views/SettingsPage.xaml.cpp" "Views/SettingsPage.xaml.h" "Views/SettingsPage.xaml" "Views/SettingsPage.idl"
        "App.xaml.cpp" "App.xaml.h" "App.xaml" "App.idl")
    set_property(SOURCE App.xaml PROPERTY VS_XAML_TYPE "ApplicationDefinition")
    set_property(TARGET ${OUTPUT_NAME} PROPERTY VS_PACKAGE_REFERENCES
        "Microsoft.Windows.CppWinRT_2.0.250303.1"
        "Microsoft.WindowsAppSDK_1.7.250606001"
        "Microsoft.Windows.SDK.BuildTools_10.0.26100.4188"
        "Microsoft.Windows.ImplementationLibrary_1.0.250325.1")
    target_include_directories(${OUTPUT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
    set_target_properties(${OUTPUT_NAME} PROPERTIES
        VS_GLOBAL_RootNamespace Nickvision.TubeConverter.WinUI
        VS_GLOBAL_AppContainerApplication false
        VS_GLOBAL_AppxPackage false
        VS_GLOBAL_CppWinRTOptimized true
        VS_GLOBAL_CppWinRTRootNamespaceAutoMerge true
        VS_GLOBAL_CppWinRTUsePrefixes false
        VS_GLOBAL_UseWinUI true
        VS_GLOBAL_ApplicationType "Windows Store"
        VS_GLOBAL_ApplicationTypeRevision 10.0
        VS_GLOBAL_WindowsTargetPlatformVersion 10.0
        VS_GLOBAL_WindowsTargetPlatformMinVersion "10.0.19045.0"
        VS_GLOBAL_WindowsPackageType None
        VS_GLOBAL_EnablePreviewMsixTooling true
        VS_GLOBAL_OutputType WinExe
        VS_GLOBAL_WindowsAppSDKSelfContained true)
    target_link_libraries(${OUTPUT_NAME} PRIVATE libparabolic)

    get_target_property(SOURCES ${OUTPUT_NAME} SOURCES)
    foreach(SOURCE ${SOURCES})
        cmake_path(GET SOURCE EXTENSION LAST_ONLY EXTENSION)
        if(NOT "${EXTENSION}" STREQUAL ".idl")
            continue()
        endif()
        set(IDL_SOURCE "${SOURCE}")
        cmake_path(REMOVE_EXTENSION SOURCE LAST_ONLY OUTPUT_VARIABLE BASENAME)
        set(XAML_SOURCE "${BASENAME}.xaml")
        if("${XAML_SOURCE}" IN_LIST SOURCES)
            set_property(SOURCE "${IDL_SOURCE}" PROPERTY VS_SETTINGS "SubType=Code" "DependentUpon=${XAML_SOURCE}")
        else()
            set_property(SOURCE "${IDL_SOURCE}" PROPERTY VS_SETTINGS "SubType=Code")
            set_property(SOURCE "${BASENAME}.h" PROPERTY VS_SETTINGS "DependentUpon=${IDL_SOURCE}")
            set_property(SOURCE "${BASENAME}.cpp" PROPERTY VS_SETTINGS "DependentUpon=${IDL_SOURCE}")
        endif()
    endforeach()

    add_custom_command(TARGET ${OUTPUT_NAME} PRE_BUILD COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${CMAKE_SOURCE_DIR}/resources/${PROJECT_NAME}.ico" "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/resources/icon.ico")

    generate_translations()
endif()
