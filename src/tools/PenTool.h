#pragma once

#include "Tool.h"
#include <QPoint>

class PenTool : public Tool {
public:
    void mousePressEvent(QMouseEvent *event, CanvasWidget *canvas, QImage *image) override;
    void mouseMoveEvent(QMouseEvent *event, CanvasWidget *canvas, QImage *image) override;
    void mouseReleaseEvent(QMouseEvent *event, CanvasWidget *canvas, QImage *image) override;

private:
    void drawLineTo(const QPoint &endPoint, CanvasWidget *canvas, QImage *image);
    
    QPoint m_lastPoint;
};
