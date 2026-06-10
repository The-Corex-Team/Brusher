#pragma once

#include "Tool.h"
#include <QColor>

class FillTool : public Tool {
public:
    void mousePressEvent(QMouseEvent *event, CanvasWidget *canvas, QImage *image) override;
    void mouseMoveEvent(QMouseEvent *event, CanvasWidget *canvas, QImage *image) override;
    void mouseReleaseEvent(QMouseEvent *event, CanvasWidget *canvas, QImage *image) override;

private:
    void floodFill(QImage *image, const QPoint &startPoint, const QColor &fillColor);
    bool colorMatch(QRgb c1, QRgb c2);
};
