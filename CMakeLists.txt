cmake_minimum_required(VERSION 3.21)

project(Brusher
    VERSION 1.0.0
    DESCRIPTION "Brusher Digital Painting Software"
    HOMEPAGE_URL "https://github.com/The-Corex-Team/Brusher"
    LANGUAGES CXX
)

# ---------------------------------------------------------------------------
# C++ configuration
# ---------------------------------------------------------------------------

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---------------------------------------------------------------------------
# Qt
# ---------------------------------------------------------------------------

find_package(Qt6 REQUIRED COMPONENTS
    Widgets
    OpenGL
    OpenGLWidgets
    Svg
    Network
)

qt_standard_project_setup()

# ---------------------------------------------------------------------------
# Sources
# ---------------------------------------------------------------------------

set(CMAKE_AUTORCC ON)

set(SOURCES
    # Application
    src/main.cpp

    # Main UI
    src/ui/MainWindow.cpp
    src/ui/MainWindow.ui

    # Canvas
    src/canvas/CanvasWidget.cpp

    # Basic tools
    src/tools/PenTool.cpp
    src/tools/EraserTool.cpp
    src/tools/FillTool.cpp
    src/tools/EyedropperTool.cpp
    src/tools/LineTool.cpp

    # Advanced tools
    src/tools/SelectionTool.cpp
    src/tools/RectSelectTool.cpp
    src/tools/EllipseSelectTool.cpp
    src/tools/LassoSelectTool.cpp
    src/tools/TextTool.cpp
    src/tools/MoveTool.cpp
    src/tools/ZoomTool.cpp

    # UI panels
    src/ui/panels/ToolPanel.cpp
    src/ui/panels/ToolOptionsBar.cpp
    src/ui/panels/ColorPanel.cpp
    src/ui/panels/LayerPanel.cpp
    src/ui/ui/BrusherTheme.cpp

    # UI dialogs / popups
    src/ui/dialogs/NewProjectDialog.cpp
    src/ui/dialogs/CanvasSizeDialog.cpp
    src/ui/BrushSettingsPopup.cpp

    # History
    src/history/DocumentSnapshot.cpp
    src/history/HistoryManager.cpp

    # Image filters
    src/filters/ImageFilters.cpp

    # Project I/O
    src/io/ProjectSerializer.cpp

    # Qt resources
    resources.qrc
)

# ---------------------------------------------------------------------------
# Executable
# ---------------------------------------------------------------------------

qt_add_executable(brusher
    ${SOURCES}
)

target_include_directories(brusher PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(brusher PRIVATE
    Qt6::Widgets
    Qt6::OpenGL
    Qt6::OpenGLWidgets
    Qt6::Svg
    Qt6::Network
)

# ---------------------------------------------------------------------------
# Platform properties
# ---------------------------------------------------------------------------

set_target_properties(brusher PROPERTIES
    OUTPUT_NAME "brusher"
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)

# ---------------------------------------------------------------------------
# Installation
# ---------------------------------------------------------------------------

include(GNUInstallDirs)

install(TARGETS brusher
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# ---------------------------------------------------------------------------
# Qt deployment
#
# Windows deployment is handled by windeployqt in windows.yaml.
# Linux deployment is handled by Qt's CMake deployment API.
# ---------------------------------------------------------------------------

if(UNIX AND NOT APPLE)
    qt_generate_deploy_app_script(
        TARGET brusher
        OUTPUT_SCRIPT deploy_script
        NO_TRANSLATIONS
    )

    install(SCRIPT ${deploy_script})
endif()

# ---------------------------------------------------------------------------
# Debian package
# ---------------------------------------------------------------------------

set(CPACK_PACKAGE_VENDOR "Corex Team")
set(CPACK_PACKAGE_CONTACT "general.support@corexteam.org")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Brusher Digital Painting Software")
set(CPACK_PACKAGE_NAME "brusher")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")

if(UNIX AND NOT APPLE)
    set(CPACK_GENERATOR "DEB")
    set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
endif()

include(CPack)