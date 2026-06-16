cmake_minimum_required(VERSION 3.16)

project(Brusher)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets OpenGLWidgets Svg)

include_directories(src)

add_executable(Brusher WIN32
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
    resources.qrc
)

target_link_libraries(Brusher PRIVATE Qt6::Widgets Qt6::OpenGLWidgets Qt6::Svg)

install(TARGETS Brusher
    RUNTIME DESTINATION bin
    BUNDLE DESTINATION .
)

set(CPACK_PACKAGE_NAME "brusher")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_VENDOR "The Corex Team")
set(CPACK_PACKAGE_DESCRIPTION "A modern painting application")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "walter.muller.1708@gmail.com")
set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

include(CPack)
