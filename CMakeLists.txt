cmake_minimum_required(VERSION 3.16)

project(Brusher VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

include(GNUInstallDirs)

find_package(Qt6 REQUIRED COMPONENTS Widgets OpenGLWidgets Svg)

set(SOURCES
    src/main.cpp

    src/ui/MainWindow.cpp
    src/ui/MainWindow.h
    src/ui/MainWindow.ui

    src/canvas/CanvasWidget.cpp
    src/canvas/CanvasWidget.h

    src/history/DocumentSnapshot.cpp
    src/history/DocumentSnapshot.h
    src/history/HistoryManager.cpp
    src/history/HistoryManager.h

    src/filters/ImageFilters.cpp
    src/filters/ImageFilters.h

    src/io/ProjectSerializer.cpp
    src/io/ProjectSerializer.h

    src/tools/PenTool.cpp
    src/tools/PenTool.h
    src/tools/EraserTool.cpp
    src/tools/EraserTool.h
    src/tools/FillTool.cpp
    src/tools/FillTool.h
    src/tools/EyedropperTool.cpp
    src/tools/EyedropperTool.h
    src/tools/LineTool.cpp
    src/tools/LineTool.h
    src/tools/SelectionTool.cpp
    src/tools/SelectionTool.h
    src/tools/RectSelectTool.cpp
    src/tools/RectSelectTool.h
    src/tools/EllipseSelectTool.cpp
    src/tools/EllipseSelectTool.h
    src/tools/LassoSelectTool.cpp
    src/tools/LassoSelectTool.h
    src/tools/TextTool.cpp
    src/tools/TextTool.h
    src/tools/MoveTool.cpp
    src/tools/MoveTool.h
    src/tools/ZoomTool.cpp
    src/tools/ZoomTool.h

    src/ui/panels/ToolPanel.cpp
    src/ui/panels/ToolPanel.h
    src/ui/panels/ToolOptionsBar.cpp
    src/ui/panels/ToolOptionsBar.h
    src/ui/panels/ColorPanel.cpp
    src/ui/panels/ColorPanel.h
    src/ui/panels/LayerPanel.cpp
    src/ui/panels/LayerPanel.h

    src/ui/dialogs/NewProjectDialog.cpp
    src/ui/dialogs/NewProjectDialog.h
    src/ui/dialogs/CanvasSizeDialog.cpp
    src/ui/dialogs/CanvasSizeDialog.h

    src/ui/BrusherTheme.cpp
    src/ui/BrusherTheme.h

    src/ui/BrushSettingsPopup.cpp
    src/ui/BrushSettingsPopup.h

    resources.qrc
)

add_executable(brusher MACOSX_BUNDLE WIN32 ${SOURCES})

target_include_directories(brusher PRIVATE src)

target_link_libraries(brusher PRIVATE
    Qt6::Widgets
    Qt6::OpenGLWidgets
    Qt6::Svg
)

# macOS bundle metadata — drives `macdeployqt` and is what shows up in
# Finder / `mdls` output. `MACOSX_BUNDLE` on add_executable already
# triggers CMake to build a `.app`; the properties below fill in the
# Info.plist.
set_target_properties(brusher PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_BUNDLE_NAME "Brusher"
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.brusher.app"
    MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}"
    MACOSX_BUNDLE_COPYRIGHT "© Brusher"
    MACOSX_BUNDLE_INFO_STRING "Brusher ${PROJECT_VERSION}"
)

install(TARGETS brusher
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    BUNDLE DESTINATION .
)

install(FILES packaging/brusher.desktop
    DESTINATION ${CMAKE_INSTALL_DATADIR}/applications
    RENAME brusher.desktop
)

# The repo's icon lives at packaging/icons/Brusher.svg; install the SVG
# into the scalable apps directory so deb / AppImage / Windows shortcut
# all pick it up. (Earlier revisions referenced a non-existent
# assets/brusher.png — replaced.)
install(FILES packaging/icons/Brusher.svg
    DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps
    RENAME brusher.svg
)

# ---------------------------------------------------------------------------
# CPack — produces a .deb on Linux and an NSIS installer on Windows.
# The generator is chosen at run time via `cpack -G <gen>`, so we leave
# CPACK_GENERATOR unset here; include(CPack) at the bottom of the file
# is what tells CMake to read these variables.
# ---------------------------------------------------------------------------
set(CPACK_PACKAGE_NAME "Brusher")
set(CPACK_PACKAGE_VENDOR "Brusher")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Modern Painting Application")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/walter-322/Brusher")
set(CPACK_PACKAGE_CONTACT "Brusher")

# Debian / Ubuntu (.deb)
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Brusher")
set(CPACK_DEBIAN_PACKAGE_SECTION "graphics")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
# SHLIBDEPS auto-resolves the Qt6 .so dependencies at cpack-time, so we
# don't need to enumerate them by hand.
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_DEBIAN_PACKAGE_DEPENDS "")
set(CPACK_DEBIAN_POSTINST_SCRIPT "${CMAKE_SOURCE_DIR}/packaging/postinst")
set(CPACK_DEBIAN_FILE_NAME "DEB-DEFAULT")

# Windows NSIS installer
set(CPACK_NSIS_DISPLAY_NAME "Brusher")
set(CPACK_NSIS_PACKAGE_NAME "Brusher")
# CPack's NSIS template auto-detects brusher.svg from the install tree
# at CPack time, so we don't need CPACK_NSIS_INSTALLED_ICON_FILE.
set(CPACK_NSIS_CREATE_ICONS_EXTRA
    "CreateShortCut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\Brusher.lnk' '$INSTDIR\\\\bin\\\\brusher.exe'")
set(CPACK_NSIS_DELETE_ICONS_EXTRA
    "Delete '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\Brusher.lnk'")
set(CPACK_NSIS_URL_INFO_ABOUT "${CPACK_PACKAGE_HOMEPAGE_URL}")

include(CPack)
