#pragma once

#include "Tool.h"
#include <QPoint>

class LineTool : public Tool {
public:
    void mousePressEvent(QMouseEvent *event, CanvasWidget *canvas, QImage *image) override;
    void mouseMoveEvent(QMouseEvent *event, CanvasWidget *canvas, QImage *image) override;
    void mouseReleaseEvent(QMouseEvent *event, CanvasWidget *canvas, QImage *image) override;
    
    void drawPreview(QPainter *painter, CanvasWidget *canvas) override;
    bool isBusy() const override { return m_drawing; }

private:
    QPoint m_startPoint;
    QPoint m_currentPoint;
    bool m_drawing = false;
};
