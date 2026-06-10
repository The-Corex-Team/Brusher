#include "EyedropperTool.h"
#include "../canvas/CanvasWidget.h"
#include <QColor>

void EyedropperTool::mousePressEvent(QMouseEvent *event, CanvasWidget *canvas, QImage *image)
{
    if (event->button() == Qt::LeftButton) {
        pickColor(event, canvas);
    }
}

void EyedropperTool::mouseMoveEvent(QMouseEvent *event, CanvasWidget *canvas, QImage *image)
{
    if (event->buttons() & Qt::LeftButton) {
        pickColor(event, canvas);
    }
}

void EyedropperTool::mouseReleaseEvent(QMouseEvent *event, CanvasWidget *canvas, QImage *image)
{
    // Do nothing on release
}

void EyedropperTool::pickColor(QMouseEvent *event, CanvasWidget *canvas)
{
    QPoint pos = event->position().toPoint();
    QImage flat = canvas->getFlattenedImage();
    
    if (pos.x() >= 0 && pos.x() < flat.width() && pos.y() >= 0 && pos.y() < flat.height()) {
        QColor color = flat.pixelColor(pos);
        // Ignore fully transparent pixels or mix them with white checkerboard
        // For simplicity, just set it
        canvas->setBrushColor(color);
    }
}
