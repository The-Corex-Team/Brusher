#pragma once

#include <QWidget>

class CanvasWidget;
class QSlider;
class QSpinBox;
class QLabel;

// Photoshop-style brush settings popup. Shown on right-click over the
// canvas while a drawing tool is active. Lets the user nudge size and
// opacity (mirroring the controls in ToolOptionsBar) and pick a color
// via QColorDialog, all without leaving the canvas.
//
// Constructed once by CanvasWidget and shown via showAt(); the popup
// is a Qt::Popup so it auto-closes on focus-out and Escape.
class BrushSettingsPopup : public QWidget {
    Q_OBJECT
public:
    explicit BrushSettingsPopup(CanvasWidget *canvas, QWidget *parent = nullptr);

    // Position the popup so its top-left is at the global cursor position,
    // then show it. anchor is unused for layout but kept for future
    // (e.g., clamping inside a parent rect).
    void showAt(const QPoint &globalPos);

private slots:
    void onSizeChanged(int size);
    void onOpacityChanged(int opacity);
    void onPickColor();
    void onCanvasBrushSizeChanged(int size);
    void onCanvasBrushOpacityChanged(int opacity);
    void onCanvasBrushColorChanged(const QColor &color);

private:
    void updateSwatch(const QColor &color);

    bool eventFilter(QObject *watched, QEvent *event) override;

    CanvasWidget *m_canvas;
    QSlider *m_sizeSlider;
    QSpinBox *m_sizeSpin;
    QSlider *m_opacitySlider;
    QSpinBox *m_opacitySpin;
    QLabel *m_swatch;
};