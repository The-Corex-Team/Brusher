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

    // Returns true while a tool is mid-interaction (stroke in progress, drag,
    // selection being made, etc.). CanvasWidget uses this to gate the
    // low-memory idle-resource release so we never swap out the active
    // editing state under the user's hand.
    virtual bool isBusy() const { return false; }
};
