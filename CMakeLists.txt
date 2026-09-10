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
    src/main.cpp

    src/ui/MainWindow.cpp
    src/ui/MainWindow.h
    src/ui/MainWindow.ui

    src/canvas/CanvasWidget.cpp
    src/canvas/CanvasWidget.h

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

    src/ui/panels/ToolPanel.cpp
    src/ui/panels/ToolPanel.h
    src/ui/panels/ColorPanel.cpp
    src/ui/panels/ColorPanel.h
    src/ui/panels/LayerPanel.cpp
    src/ui/panels/LayerPanel.h

    src/ui/dialogs/NewProjectDialog.cpp
    src/ui/dialogs/NewProjectDialog.h

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
set(CPACK_PACKAGE_CONTACT "walter.muller.1708@gmail.com")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Brusher Digital Painting Software")
set(CPACK_PACKAGE_NAME "brusher")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")

if(UNIX AND NOT APPLE)
    set(CPACK_GENERATOR "DEB")
    set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
endif()

include(CPack)
