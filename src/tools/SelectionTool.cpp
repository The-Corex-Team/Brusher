#include "SelectionTool.h"
#include "../canvas/CanvasWidget.h"
#include <QPainter>

void SelectionTool::mousePressEvent(QMouseEvent *event, CanvasWidget *canvas, QImage *image)
{
    Q_UNUSED(image);
    if (event->button() == Qt::LeftButton) {
        m_startPoint = event->position().toPoint();
        m_currentPoint = m_startPoint;
        m_selecting = true;
        canvas->update();
    }
}

void SelectionTool::mouseMoveEvent(QMouseEvent *event, CanvasWidget *canvas, QImage *image)
{
    Q_UNUSED(image);
    if ((event->buttons() & Qt::LeftButton) && m_selecting) {
        m_currentPoint = event->position().toPoint();
        canvas->update();
    }
}

void SelectionTool::mouseReleaseEvent(QMouseEvent *event, CanvasWidget *canvas, QImage *image)
{
    Q_UNUSED(image);
    if (event->button() == Qt::LeftButton && m_selecting) {
        m_selecting = false;
        m_currentPoint = event->position().toPoint();
        applySelection(canvas, buildPath(m_startPoint, m_currentPoint));
        canvas->update();
    }
}

void SelectionTool::drawPreview(QPainter *painter, CanvasWidget *canvas)
{
    Q_UNUSED(canvas);
    if (m_selecting) {
        QPen pen(QColor(0, 120, 215), 0, Qt::DashLine);
        painter->setPen(pen);
        painter->setBrush(Qt::NoBrush);
        painter->drawPath(buildPath(m_startPoint, m_currentPoint));
    }
}

void SelectionTool::applySelection(CanvasWidget *canvas, const QPainterPath &path)
{
    if (path.isEmpty()) {
        return;
    }

    const QRect bounds = path.boundingRect().toAlignedRect().intersected(
        QRect(0, 0, canvas->canvasSize().width(), canvas->canvasSize().height()));

    if (bounds.isEmpty()) {
        return;
    }

    QImage mask(bounds.size(), QImage::Format_Alpha8);
    mask.fill(0);

    QPainter maskPainter(&mask);
    maskPainter.setRenderHint(QPainter::Antialiasing);
    maskPainter.translate(-bounds.topLeft());
    maskPainter.fillPath(path, Qt::white);

    canvas->setSelectionMask(mask, bounds);
}
