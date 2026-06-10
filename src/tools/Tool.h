#pragma once

#include <QMouseEvent>
#include <QImage>

class CanvasWidget;

class Tool {
public:
    virtual ~Tool() = default;
    
    virtual void mousePressEvent(QMouseEvent *event, CanvasWidget *canvas, QImage *image) = 0;
    virtual void mouseMoveEvent(QMouseEvent *event, CanvasWidget *canvas, QImage *image) = 0;
    virtual void mouseReleaseEvent(QMouseEvent *event, CanvasWidget *canvas, QImage *image) = 0;
    
    virtual void drawPreview(QPainter *painter, CanvasWidget *canvas) {}
};
