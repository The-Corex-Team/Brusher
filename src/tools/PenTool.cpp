#include "PenTool.h"
#include "../canvas/CanvasWidget.h"
#include <QPainter>

void PenTool::mousePressEvent(QMouseEvent *event, CanvasWidget *canvas, QImage *image)
{
    if (event->button() == Qt::LeftButton) {
        m_lastPoint = event->position().toPoint();
    }
}

void PenTool::mouseMoveEvent(QMouseEvent *event, CanvasWidget *canvas, QImage *image)
{
    if (event->buttons() & Qt::LeftButton) {
        drawLineTo(event->position().toPoint(), canvas, image);
    }
}

void PenTool::mouseReleaseEvent(QMouseEvent *event, CanvasWidget *canvas, QImage *image)
{
    if (event->button() == Qt::LeftButton) {
        drawLineTo(event->position().toPoint(), canvas, image);
    }
}

void PenTool::drawLineTo(const QPoint &endPoint, CanvasWidget *canvas, QImage *image)
{
    QPainter painter(image);
    QColor color = canvas->brushColor();
    color.setAlpha(canvas->brushOpacity());
    painter.setPen(QPen(color, canvas->brushSize(), Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.drawLine(m_lastPoint, endPoint);
    
    int rad = (canvas->brushSize() / 2) + 2;
    canvas->update(QRect(m_lastPoint, endPoint).normalized().adjusted(-rad, -rad, +rad, +rad));
    m_lastPoint = endPoint;
}
