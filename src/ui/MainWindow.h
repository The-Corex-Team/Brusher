#pragma once

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class CanvasWidget;
class ToolPanel;
class ColorPanel;
class LayerPanel;
class ToolOptionsBar;
class QLabel;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionNew_triggered();
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void on_actionSaveProject_triggered();
    void on_actionExport_triggered();
    void on_actionExit_triggered();

    void on_actionUndo_triggered();
    void on_actionRedo_triggered();
    void on_actionClear_triggered();

    void on_actionCanvasSize_triggered();
    void on_actionRotateCW_triggered();
    void on_actionRotateCCW_triggered();
    void on_actionFlipHorizontal_triggered();
    void on_actionFlipVertical_triggered();
    void on_actionCrop_triggered();

    void on_actionBlur_triggered();
    void on_actionSharpen_triggered();

    void on_actionNewLayer_triggered();
    void on_actionDuplicateLayer_triggered();
    void on_actionDeleteLayer_triggered();
    void on_actionFlattenImage_triggered();

    void on_actionZoomIn_triggered();
    void on_actionZoomOut_triggered();
    void on_actionFitToWindow_triggered();
    void on_actionActualSize_triggered();

    void setTool(int toolType);
    void updateStatusBarZoom(double zoom);
    void updateUndoRedoActions();
    void swapColors();

private:
    void setupStatusBar();

    Ui::MainWindow *ui;
    CanvasWidget *m_canvasWidget;
    ToolPanel *m_toolPanel;
    ToolOptionsBar *m_toolOptionsBar;
    ColorPanel *m_colorPanel;
    LayerPanel *m_layerPanel;

    QLabel *m_zoomLabel;
    QLabel *m_sizeLabel;
    QLabel *m_toolLabel;

    QColor m_fgColor;
    QColor m_bgColor;
};
