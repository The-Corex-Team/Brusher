#pragma once

#include <QToolBar>
#include <QStackedWidget>

class CanvasWidget;
class QSlider;
class QSpinBox;
class QComboBox;
class QCheckBox;
class QLabel;

class ToolOptionsBar : public QToolBar {
    Q_OBJECT
public:
    explicit ToolOptionsBar(CanvasWidget *canvas, QWidget *parent = nullptr);

public slots:
    void setToolType(int type);

private slots:
    void onBrushSizeChanged(int size);
    void onBrushOpacityChanged(int opacity);
    void onToleranceChanged(int tolerance);

private:
    void setupPages();
    QWidget* createBrushOptions();
    QWidget* createEraserOptions();
    QWidget* createFillOptions();
    QWidget* createLineOptions();
    QWidget* createEyedropperOptions();
    QWidget* createSelectOptions();
    QWidget* createTextOptions();
    QWidget* createEmptyOptions(const QString &message);

    CanvasWidget *m_canvas;
    QStackedWidget *m_stack;
    
    // Shared controls
    QSlider *m_brushSizeSlider;
    QSpinBox *m_brushSizeSpinBox;
    QSlider *m_opacitySlider;
    QSpinBox *m_opacitySpinBox;
    QSpinBox *m_toleranceSpinBox;
};
