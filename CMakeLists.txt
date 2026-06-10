cmake_minimum_required(VERSION 3.16)

project(Brusher)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets OpenGLWidgets Svg)

include_directories(src)

add_executable(Brusher
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

target_link_libraries(Brusher PRIVATE Qt6::Widgets Qt6::OpenGLWidgets Qt6::Svg)

install(TARGETS Brusher
    RUNTIME DESTINATION bin
    BUNDLE DESTINATION .
)
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_PACKAGE_VENDOR "Corex Team")
set(CPACK_PACKAGE_CONTACT "walter.muller.1708@gmail.com")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Brusher Digital Painting Software")
set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_NAME "brusher")
set(CPACK_PACKAGE_VERSION "0.1.0")
include(CPack)