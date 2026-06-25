#pragma once

#include "Tool.h"
#include <QPoint>
#include <QImage>

class MoveTool : public Tool {
public:
    void mousePressEvent(QMouseEvent *event, CanvasWidget *canvas, QImage *image) override;
    void mouseMoveEvent(QMouseEvent *event, CanvasWidget *canvas, QImage *image) override;
    void mouseReleaseEvent(QMouseEvent *event, CanvasWidget *canvas, QImage *image) override;
    void drawPreview(QPainter *painter, CanvasWidget *canvas) override;
    bool isBusy() const override { return m_moving; }

private:
    QPoint m_startPoint;
    QPoint m_lastPoint;
    bool m_moving = false;
    QImage m_floatingPixels;
};
