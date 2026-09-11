cmake_minimum_required(VERSION 3.21)

project(Brusher
    VERSION 1.0.0
    DESCRIPTION "Brusher Digital Painting Software"
    HOMEPAGE_URL "https://github.com/The-Corex-Team/Brusher"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS
    Widgets
    OpenGL
    OpenGLWidgets
    Svg
    Network
)

qt_standard_project_setup()

set(SOURCES
    # -----------------------------------------------------------------------
    # Application
    # -----------------------------------------------------------------------

    src/main.cpp

    # -----------------------------------------------------------------------
    # Main UI
    # -----------------------------------------------------------------------

    src/ui/MainWindow.cpp
    src/ui/MainWindow.h
    src/ui/MainWindow.ui

    # -----------------------------------------------------------------------
    # Canvas
    # -----------------------------------------------------------------------

    src/canvas/CanvasWidget.cpp
    src/canvas/CanvasWidget.h

    # -----------------------------------------------------------------------
    # Models
    # -----------------------------------------------------------------------

    src/models/Layer.cpp
    src/models/Layer.h

    # -----------------------------------------------------------------------
    # Tools - basic
    # -----------------------------------------------------------------------

    src/tools/Tool.h

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

    # -----------------------------------------------------------------------
    # Tools - advanced
    # -----------------------------------------------------------------------

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

    # -----------------------------------------------------------------------
    # UI panels
    # -----------------------------------------------------------------------

    src/ui/panels/ToolPanel.cpp
    src/ui/panels/ToolPanel.h

    src/ui/panels/ToolOptionsBar.cpp
    src/ui/panels/ToolOptionsBar.h

    src/ui/panels/ColorPanel.cpp
    src/ui/panels/ColorPanel.h

    src/ui/panels/LayerPanel.cpp
    src/ui/panels/LayerPanel.h

    # -----------------------------------------------------------------------
    # UI dialogs / popups
    # -----------------------------------------------------------------------

    src/ui/dialogs/NewProjectDialog.cpp
    src/ui/dialogs/NewProjectDialog.h

    src/ui/dialogs/CanvasSizeDialog.cpp
    src/ui/dialogs/CanvasSizeDialog.h

    src/ui/BrushSettingsPopup.cpp
    src/ui/BrushSettingsPopup.h

    # -----------------------------------------------------------------------
    # History
    # -----------------------------------------------------------------------

    src/history/DocumentSnapshot.cpp
    src/history/DocumentSnapshot.h

    src/history/HistoryManager.cpp
    src/history/HistoryManager.h

    # -----------------------------------------------------------------------
    # Image filters
    # -----------------------------------------------------------------------

    src/filters/ImageFilters.cpp
    src/filters/ImageFilters.h

    # -----------------------------------------------------------------------
    # Project I/O
    # -----------------------------------------------------------------------

    src/io/ProjectSerializer.cpp
    src/io/ProjectSerializer.h

    # -----------------------------------------------------------------------
    # Qt resources
    # -----------------------------------------------------------------------

    resources.qrc
)

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

set_target_properties(brusher PROPERTIES
    OUTPUT_NAME "brusher"
)

# ---------------------------------------------------------------------------
# Installation
# ---------------------------------------------------------------------------

include(GNUInstallDirs)

install(TARGETS brusher
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    BUNDLE DESTINATION .
)

# ---------------------------------------------------------------------------
# Qt deployment
# ---------------------------------------------------------------------------

if(WIN32)
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