cmake_minimum_required(VERSION 3.16)

project(Brusher VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Qt
find_package(Qt6 REQUIRED COMPONENTS Widgets OpenGLWidgets Svg)

# Sources
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

    resources.qrc
)

add_executable(Brusher WIN32 ${SOURCES})

target_include_directories(Brusher PRIVATE src)

target_link_libraries(Brusher PRIVATE
    Qt6::Widgets
    Qt6::OpenGLWidgets
    Qt6::Svg
)
