#pragma once

#include "Tool.h"
#include <QColor>
#include <QRect>

class FillTool : public Tool {
public:
    void mousePressEvent(QMouseEvent *event, CanvasWidget *canvas, QImage *image) override;
    void mouseMoveEvent(QMouseEvent *event, CanvasWidget *canvas, QImage *image) override;
    void mouseReleaseEvent(QMouseEvent *event, CanvasWidget *canvas, QImage *image) override;

private:
    QRect floodFill(QImage *image, const QPoint &startPoint, const QColor &fillColor, int tolerance);
    bool colorMatch(QRgb c1, QRgb c2, int tolerance);
};
