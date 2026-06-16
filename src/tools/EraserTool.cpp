#include "EraserTool.h"
#include "../canvas/CanvasWidget.h"
#include <QPainter>

void EraserTool::mousePressEvent(QMouseEvent *event, CanvasWidget *canvas, QImage *image)
{
    if (event->button() == Qt::LeftButton) {
        m_lastPoint = event->position().toPoint();
    }
}

void EraserTool::mouseMoveEvent(QMouseEvent *event, CanvasWidget *canvas, QImage *image)
{
    if (event->buttons() & Qt::LeftButton) {
        drawLineTo(event->position().toPoint(), canvas, image);
    }
}

void EraserTool::mouseReleaseEvent(QMouseEvent *event, CanvasWidget *canvas, QImage *image)
{
    if (event->button() == Qt::LeftButton) {
        drawLineTo(event->position().toPoint(), canvas, image);
    }
}

void EraserTool::drawLineTo(const QPoint &endPoint, CanvasWidget *canvas, QImage *image)
{
    QPainter painter(image);
    // Use CompositionMode_Clear to "erase" pixels by making them transparent.
    // If the image doesn't have an alpha channel, it paints with the background color (usually white or black).
    // Our QImage is RGB32 right now, which doesn't support alpha, so we should convert it to ARGB32 later.
    // For now, if it's RGB32, CompositionMode_Clear might fill with black. Let's explicitly set the format to ARGB32 in CanvasWidget.
    painter.setCompositionMode(QPainter::CompositionMode_Clear);
    painter.setPen(QPen(Qt::transparent, canvas->brushSize(), Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.drawLine(m_lastPoint, endPoint);
    
    int rad = (canvas->brushSize() / 2) + 2;
    canvas->updateCanvasRect(QRect(m_lastPoint, endPoint).normalized().adjusted(-rad, -rad, +rad, +rad));
    m_lastPoint = endPoint;
}
