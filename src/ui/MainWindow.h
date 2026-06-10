#pragma once

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class CanvasWidget;
class ToolPanel;
class ColorPanel;
class LayerPanel;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionNew_triggered();
    void on_actionSave_triggered();
    void on_actionClear_triggered();
    void setPenTool();
    void setEraserTool();
    void setFillTool();
    void setEyedropperTool();
    void setLineTool();

private:
    Ui::MainWindow *ui;
    CanvasWidget *m_canvasWidget;
    ToolPanel *m_toolPanel;
    ColorPanel *m_colorPanel;
    LayerPanel *m_layerPanel;
};
