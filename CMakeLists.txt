cmake_minimum_required(VERSION 3.16)
project(ToggleNote VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported)
if(ipo_supported)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
endif()

if(WIN32 OR APPLE)
    find_package(Qt5 5.15 REQUIRED COMPONENTS Widgets Network Svg)
else()
    find_package(Qt6 6.5 REQUIRED COMPONENTS Widgets Network Svg)
    find_package(X11 REQUIRED)
endif()

set(SOURCES
    src/app/main.cpp
    src/app/Application.cpp
    src/app/CommandLineArgs.cpp
    src/app/NoteManager.cpp
    src/app/OsSignalHandler.cpp
    src/app/CurrentProcessManager.cpp
    src/app/ThemeUtils.cpp
    src/app/Paths.cpp
    src/core/Note.cpp
    src/core/Settings.cpp
    src/core/ActionId.cpp
    src/core/ActionDefinition.cpp
    src/services/NoteService.cpp
    src/services/SettingsService.cpp
    src/services/KeybindingService.cpp
    src/services/GlobalHotkeyService.cpp
    $<$<PLATFORM_ID:Windows>:src/services/GlobalHotkeyService_win.cpp>
    $<$<PLATFORM_ID:Darwin>:src/services/GlobalHotkeyService_mac.cpp>
    $<$<NOT:$<PLATFORM_ID:Windows,Darwin>>:src/services/GlobalHotkeyService_x11.cpp>
    $<$<NOT:$<PLATFORM_ID:Windows,Darwin,Linux>>:src/ui/note/ClickThroughManager.cpp>
    $<$<PLATFORM_ID:Windows>:src/ui/note/ClickThroughManager_win.cpp>
    $<$<PLATFORM_ID:Darwin>:src/ui/note/ClickThroughManager_mac.mm>
    $<$<NOT:$<PLATFORM_ID:Windows,Darwin>>:src/ui/note/ClickThroughManager_x11.cpp>
    src/infra/NoteFileRepository.cpp
    src/infra/SettingsRepository.cpp
    src/presentation/NoteViewModel.cpp
    src/ui/note/NoteWindow.cpp
    src/ui/note/AutohideController.cpp
    src/ui/note/NoteController.cpp
    src/ui/note/NoteEditor.cpp
    src/ui/note/NoteSettingsController.cpp
    src/ui/note/NoteSettingsDialog.cpp
    src/ui/note/SearchBar.cpp
    src/ui/settings/SettingsDialog.cpp
    src/ui/settings/CustomColorDialog.cpp
    src/ui/settings/ColorGridDialog.cpp
    src/ui/settings/ColorRow.cpp
    src/ui/settings/ResettableRow.cpp
    src/ui/settings/SpinRow.cpp
    src/ui/settings/SliderSpinRow.cpp
    src/ui/settings/FontFamilyRow.cpp
    src/ui/settings/KeybindingsPage.cpp
    src/ui/settings/SettingsRowUtils.cpp
    src/design/Design.cpp
    src/ui/common/FramelessWindow.cpp
    src/ui/common/BaseDialog.cpp
    src/ui/common/TitleBar.cpp
    src/ui/common/Icons.cpp
    src/ui/common/TrayIcon.cpp
    src/ui/note/CornerButton.cpp
    src/ui/note/LockButton.cpp
    src/ui/note/PinButton.cpp
    src/ui/note/GhostIndicator.cpp
    src/ui/common/AlwaysOnTopManager.cpp
    src/ui/note/WindowGeometryHelper.cpp
    src/ui/note/ToolbarController.cpp
    src/ui/toolbar/ToolbarWidget.cpp
    src/ui/toolbar/ContextMenu.cpp
    assets/resources.qrc
)

if(WIN32)
    list(APPEND SOURCES src/app/resources.rc)
endif()

set(HEADERS
    src/app/Application.h
    src/app/CommandLineArgs.h
    src/app/NoteManager.h
    src/app/OsSignalHandler.h
    src/app/CurrentProcessManager.h
    src/app/ThemeUtils.h
    src/core/Note.h
    src/core/Settings.h
    src/ports/INoteRepository.h
    src/ports/INoteService.h
    src/ports/ISettingsRepository.h
    src/ports/ISettingsProvider.h
    src/ports/IKeybindingProvider.h
    src/ports/IKeybindingConfiguration.h
    src/ports/INoteController.h
    src/services/NoteService.h
    src/services/SettingsService.h
    src/services/KeybindingService.h
    src/services/GlobalHotkeyService.h
    src/core/Icon.h
    src/core/ActionDefinition.h
    src/infra/NoteFileRepository.h
    src/infra/SettingsRepository.h
    src/presentation/NoteViewModel.h
    src/ui/note/AutohideController.h
    src/ui/note/NoteWindow.h
    src/ui/note/NoteController.h
    src/ui/note/NoteEditor.h
    src/ui/note/NoteSettingsController.h
    src/ui/note/NoteSettingsDialog.h
    src/ui/note/SearchBar.h
    src/ui/note/CornerButton.h
    src/ui/note/LockButton.h
    src/ui/note/PinButton.h
    src/ui/note/GhostIndicator.h
    src/ui/common/AlwaysOnTopManager.h
    src/ui/note/WindowGeometryHelper.h
    src/ui/note/ClickThroughManager.h
    src/ui/settings/SettingsDialog.h
    src/ui/settings/CustomColorDialog.h
    src/ui/settings/ColorGridDialog.h
    src/ui/settings/ColorRow.h
    src/ui/settings/ResettableRow.h
    src/ui/settings/SpinRow.h
    src/ui/settings/SliderSpinRow.h
    src/ui/settings/FontFamilyRow.h
    src/ui/settings/KeybindingsPage.h
    src/ui/common/FramelessWindow.h
    src/ui/common/Icons.h
    src/ui/common/TrayIcon.h
    src/ui/toolbar/ToolbarButton.h
    src/ui/note/ToolbarController.h
    src/ui/toolbar/ToolbarWidget.h
    src/ui/toolbar/ContextMenu.h
    src/design/Design.h
)

add_executable(ToggleNote ${SOURCES} ${HEADERS})
set_target_properties(ToggleNote PROPERTIES OUTPUT_NAME "togglenote")

if(WIN32 OR APPLE)
    target_link_libraries(ToggleNote PRIVATE Qt5::Widgets Qt5::Network Qt5::Svg)
else()
    target_link_libraries(ToggleNote PRIVATE Qt6::Widgets Qt6::Network Qt6::Svg)
endif()

target_include_directories(ToggleNote PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app
    ${CMAKE_CURRENT_SOURCE_DIR}/src/infra
    ${CMAKE_CURRENT_SOURCE_DIR}/src/services
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ports
    ${CMAKE_CURRENT_SOURCE_DIR}/src/presentation
    ${CMAKE_CURRENT_SOURCE_DIR}/src/design
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/note
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/settings
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/toolbar
)

target_compile_definitions(ToggleNote PRIVATE
    $<$<CONFIG:Debug>:TN_DEBUG>
)

# ── Platform-specific ──
if(WIN32)
    set_target_properties(ToggleNote PROPERTIES WIN32_EXECUTABLE TRUE)
    if(MINGW)
        target_compile_options(ToggleNote PRIVATE -mcrtdll=ucrtbase)
        target_link_options(ToggleNote PRIVATE -mcrtdll=ucrtbase -static-libgcc)
    endif()
elseif(APPLE)
    target_link_libraries(ToggleNote PRIVATE "-framework Carbon")
    set_target_properties(ToggleNote PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_BUNDLE_NAME "ToggleNote"
        MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}"
        MACOSX_BUNDLE_COPYRIGHT "MIT License"
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/packaging/Info.plist"
        MACOSX_BUNDLE_ICON_FILE "togglenote.icns"
    )
    set_source_files_properties(
        ${CMAKE_CURRENT_SOURCE_DIR}/packaging/togglenote.icns PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources"
    )
    install(FILES packaging/togglenote.icns DESTINATION "ToggleNote.app/Contents/Resources")
else()
    target_link_libraries(ToggleNote PRIVATE X11::X11 Xext)
    target_link_options(ToggleNote PRIVATE "-Wl,-rpath,/usr/lib" "-Wl,--disable-new-dtags")
endif()

# ── Windows: deploy Qt DLLs ──
if(WIN32)
    get_target_property(QT5_QMAKE_EXECUTABLE Qt5::qmake IMPORTED_LOCATION)
    get_filename_component(QT5_BIN_DIR "${QT5_QMAKE_EXECUTABLE}" DIRECTORY)
    get_filename_component(QT5_INSTALL_PREFIX "${QT5_BIN_DIR}" DIRECTORY)

    set(QT5_DEPLOY_DLLS
        Qt5Core Qt5Gui Qt5Widgets Qt5Network Qt5Svg
        libgcc_s_dw2-1 libstdc++-6 libwinpthread-1
    )
    set(QT5_DEPLOY_PLUGINS
        platforms styles imageformats iconengines
    )

    file(WRITE "${CMAKE_BINARY_DIR}/deploy.cmake"
        "set(DLLS ${QT5_DEPLOY_DLLS})\n"
        "set(PLUGINS ${QT5_DEPLOY_PLUGINS})\n"
        "set(SRC_BIN \"${QT5_BIN_DIR}\")\n"
        "set(SRC_PLUGINS \"${QT5_INSTALL_PREFIX}/plugins\")\n"
        "set(DST \"${CMAKE_BINARY_DIR}\")\n"
        "foreach(dll \${DLLS})\n"
        "  if(EXISTS \"\${SRC_BIN}/\${dll}.dll\")\n"
        "    configure_file(\"\${SRC_BIN}/\${dll}.dll\" \"\${DST}/\${dll}.dll\" COPYONLY)\n"
        "  endif()\n"
        "endforeach()\n"
        "foreach(p \${PLUGINS})\n"
        "  file(COPY \"\${SRC_PLUGINS}/\${p}/\" DESTINATION \"\${DST}/\${p}\" FILES_MATCHING PATTERN \"*.dll\")\n"
        "endforeach()\n"
    )

    add_custom_target(deploy-windows
        COMMAND ${CMAKE_COMMAND} -P "${CMAKE_BINARY_DIR}/deploy.cmake"
        COMMENT "Deploying Qt DLLs and plugins to ${CMAKE_BINARY_DIR}"
    )

    file(WRITE "${CMAKE_BINARY_DIR}/install-qt.cmake"
        "set(DLLS ${QT5_DEPLOY_DLLS})\n"
        "set(PLUGINS ${QT5_DEPLOY_PLUGINS})\n"
        "set(SRC_BIN \"${QT5_BIN_DIR}\")\n"
        "set(SRC_PLUGINS \"${QT5_INSTALL_PREFIX}/plugins\")\n"
        "set(DST \"\${CMAKE_INSTALL_PREFIX}/bin\")\n"
        "foreach(dll \${DLLS})\n"
        "  if(EXISTS \"\${SRC_BIN}/\${dll}.dll\")\n"
        "    configure_file(\"\${SRC_BIN}/\${dll}.dll\" \"\${DST}/\${dll}.dll\" COPYONLY)\n"
        "  endif()\n"
        "endforeach()\n"
        "foreach(p \${PLUGINS})\n"
        "  file(COPY \"\${SRC_PLUGINS}/\${p}/\" DESTINATION \"\${DST}/\${p}\" FILES_MATCHING PATTERN \"*.dll\")\n"
        "endforeach()\n"
    )
endif()

# ── Install ──
if(APPLE)
    install(TARGETS ToggleNote BUNDLE DESTINATION .)
    install(CODE [[
        execute_process(COMMAND macdeployqt "${CMAKE_INSTALL_PREFIX}/ToggleNote.app"
                        RESULT_VARIABLE macdeployqt_res
                        ERROR_VARIABLE macdeployqt_err)
        if(NOT macdeployqt_res EQUAL 0)
            message(WARNING "macdeployqt failed: ${macdeployqt_err}")
        endif()
        execute_process(COMMAND codesign --force --deep --sign - "${CMAKE_INSTALL_PREFIX}/ToggleNote.app"
                        RESULT_VARIABLE codesign_res)
        if(NOT codesign_res EQUAL 0)
            message(WARNING "codesign failed — macOS may reject the app")
        endif()
    ]])
else()
    install(TARGETS ToggleNote RUNTIME DESTINATION bin)
endif()
if(WIN32)
    install(SCRIPT "${CMAKE_BINARY_DIR}/install-qt.cmake")
endif()
if(UNIX)
    install(FILES packaging/togglenote.desktop DESTINATION share/applications)
    install(FILES packaging/com.togglenote.ToggleNote.metainfo.xml DESTINATION share/metainfo)
    install(FILES packaging/togglenote.svg DESTINATION share/icons/hicolor/scalable/apps)
    install(FILES packaging/icons/togglenote_16.png DESTINATION share/icons/hicolor/16x16/apps RENAME togglenote.png)
    install(FILES packaging/icons/togglenote_22.png DESTINATION share/icons/hicolor/22x22/apps RENAME togglenote.png)
    install(FILES packaging/icons/togglenote_24.png DESTINATION share/icons/hicolor/24x24/apps RENAME togglenote.png)
    install(FILES packaging/icons/togglenote_32.png DESTINATION share/icons/hicolor/32x32/apps RENAME togglenote.png)
    install(FILES packaging/icons/togglenote_48.png DESTINATION share/icons/hicolor/48x48/apps RENAME togglenote.png)
    install(FILES packaging/icons/togglenote_64.png DESTINATION share/icons/hicolor/64x64/apps RENAME togglenote.png)
    install(FILES packaging/icons/togglenote_128.png DESTINATION share/icons/hicolor/128x128/apps RENAME togglenote.png)
    install(FILES packaging/icons/togglenote_256.png DESTINATION share/icons/hicolor/256x256/apps RENAME togglenote.png)
    install(FILES LICENSE DESTINATION share/licenses/togglenote)
endif()

# ── Uninstall ──
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/packaging/cmake_uninstall.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
    IMMEDIATE @ONLY
)
add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
)

# ── CPack ──
set(CPACK_PACKAGE_NAME "ToggleNote")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Lightweight sticky notes that float above everything")
set(CPACK_PACKAGE_VENDOR "ToggleNote")
set(CPACK_PACKAGE_CONTACT "melogsistemas@gmail.com")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")

if(WIN32)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
    else()
        set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES")
    endif()
    set(CPACK_NSIS_DISPLAY_NAME "ToggleNote")
    set(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/packaging/togglenote.ico")
    set(CPACK_NSIS_MUI_UNIICON "${CMAKE_SOURCE_DIR}/packaging/togglenote.ico")
    set(CPACK_NSIS_CREATE_ICONS_EXTRA
        "CreateShortCut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\ToggleNote.lnk' '$INSTDIR\\\\bin\\\\togglenote.exe'")
    set(CPACK_NSIS_DELETE_ICONS_EXTRA
        "Delete '$SMPROGRAMS\\\\$START_MENU\\\\ToggleNote.lnk'")
elseif(UNIX)
    set(CPACK_DEBIAN_PACKAGE_NAME "togglenote")
    set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
    set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
    set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
    set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "${CPACK_PACKAGE_DESCRIPTION_SUMMARY}")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS
        "libqt6widgets6 (>= 6.5), libqt6network6 (>= 6.5), libqt6svg6 (>= 6.5), libx11-6")

    set(CPACK_RPM_PACKAGE_NAME "togglenote")
    set(CPACK_RPM_PACKAGE_LICENSE "MIT")
    set(CPACK_RPM_PACKAGE_GROUP "Utilities")
    set(CPACK_RPM_PACKAGE_URL "https://github.com/melogsistemas/togglenote")
    set(CPACK_RPM_PACKAGE_DESCRIPTION "${CPACK_PACKAGE_DESCRIPTION_SUMMARY}")
    set(CPACK_RPM_PACKAGE_REQUIRES "qt6-qtbase >= 6.5, qt6-qtsvg >= 6.5, libx11 >= 1.7")
    set(CPACK_RPM_FILE_NAME "togglenote-${CPACK_PACKAGE_VERSION}-x86_64.rpm")
elseif(APPLE)
    set(CPACK_GENERATOR "DragNDrop")
    set(CPACK_DMG_VOLUME_NAME "ToggleNote")
    set(CPACK_DMG_BACKGROUND_IMAGE "${CMAKE_SOURCE_DIR}/packaging/dmg-background.png")
    set(CPACK_DMG_DS_STORE "${CMAKE_SOURCE_DIR}/packaging/dmg-dsstore")
    set(CPACK_PACKAGE_FILE_NAME "togglenote-${CPACK_PACKAGE_VERSION}-macos")
endif()

include(CPack)

# ── clangd compile_commands.json symlink ──
execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink
    ${CMAKE_BINARY_DIR}/compile_commands.json
    ${CMAKE_SOURCE_DIR}/compile_commands.json
    OUTPUT_QUIET ERROR_QUIET)
