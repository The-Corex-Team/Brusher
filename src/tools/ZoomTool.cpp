#include "ZoomTool.h"
#include "../canvas/CanvasWidget.h"

void ZoomTool::mousePressEvent(QMouseEvent *event, CanvasWidget *canvas, QImage *image)
{
    Q_UNUSED(image);
    if (event->button() == Qt::LeftButton) {
        if (event->modifiers() & Qt::AltModifier) {
            canvas->zoomOutAt(event->position());
        } else {
            canvas->zoomInAt(event->position());
        }
    }
}

void ZoomTool::mouseMoveEvent(QMouseEvent *event, CanvasWidget *canvas, QImage *image)
{
    Q_UNUSED(event);
    Q_UNUSED(canvas);
    Q_UNUSED(image);
}

void ZoomTool::mouseReleaseEvent(QMouseEvent *event, CanvasWidget *canvas, QImage *image)
{
    Q_UNUSED(event);
    Q_UNUSED(canvas);
    Q_UNUSED(image);
}
