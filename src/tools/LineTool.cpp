#include "LineTool.h"
#include "../canvas/CanvasWidget.h"
#include <QPainter>

void LineTool::mousePressEvent(QMouseEvent *event, CanvasWidget *canvas, QImage *image)
{
    if (event->button() == Qt::LeftButton) {
        m_startPoint = event->position().toPoint();
        m_currentPoint = m_startPoint;
        m_drawing = true;
        canvas->update();
    }
}

void LineTool::mouseMoveEvent(QMouseEvent *event, CanvasWidget *canvas, QImage *image)
{
    if ((event->buttons() & Qt::LeftButton) && m_drawing) {
        m_currentPoint = event->position().toPoint();
        canvas->update();
    }
}

void LineTool::mouseReleaseEvent(QMouseEvent *event, CanvasWidget *canvas, QImage *image)
{
    if (event->button() == Qt::LeftButton && m_drawing) {
        m_drawing = false;
        
        QPainter painter(image);
        painter.setPen(QPen(canvas->brushColor(), canvas->brushSize(), Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter.drawLine(m_startPoint, event->position().toPoint());
        
        canvas->update();
    }
}

void LineTool::drawPreview(QPainter *painter, CanvasWidget *canvas)
{
    if (m_drawing) {
        painter->setPen(QPen(canvas->brushColor(), canvas->brushSize(), Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter->drawLine(m_startPoint, m_currentPoint);
    }
}
