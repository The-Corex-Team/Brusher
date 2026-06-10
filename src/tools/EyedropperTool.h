#pragma once

#include "Tool.h"

class EyedropperTool : public Tool {
public:
    void mousePressEvent(QMouseEvent *event, CanvasWidget *canvas, QImage *image) override;
    void mouseMoveEvent(QMouseEvent *event, CanvasWidget *canvas, QImage *image) override;
    void mouseReleaseEvent(QMouseEvent *event, CanvasWidget *canvas, QImage *image) override;

private:
    void pickColor(QMouseEvent *event, CanvasWidget *canvas);
};
