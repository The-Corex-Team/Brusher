#include "LassoSelectTool.h"
#include "../canvas/CanvasWidget.h"
#include <QPainter>
#include <QPainterPath>

void LassoSelectTool::mousePressEvent(QMouseEvent *event, CanvasWidget *canvas, QImage *image)
{
    Q_UNUSED(image);
    if (event->button() == Qt::LeftButton) {
        m_points.clear();
        m_points.append(event->position().toPoint());
        m_selecting = true;
        canvas->update();
    }
}

void LassoSelectTool::mouseMoveEvent(QMouseEvent *event, CanvasWidget *canvas, QImage *image)
{
    Q_UNUSED(image);
    if ((event->buttons() & Qt::LeftButton) && m_selecting) {
        m_points.append(event->position().toPoint());
        canvas->update();
    }
}

void LassoSelectTool::mouseReleaseEvent(QMouseEvent *event, CanvasWidget *canvas, QImage *image)
{
    Q_UNUSED(image);
    if (event->button() == Qt::LeftButton && m_selecting) {
        m_selecting = false;
        if (m_points.size() < 3) {
            canvas->update();
            return;
        }

        QPainterPath path;
        path.moveTo(m_points.first());
        for (int i = 1; i < m_points.size(); ++i) {
            path.lineTo(m_points[i]);
        }
        path.closeSubpath();

        const QRect bounds = path.boundingRect().toAlignedRect().intersected(
            QRect(0, 0, canvas->canvasSize().width(), canvas->canvasSize().height()));

        if (!bounds.isEmpty()) {
            QImage mask(bounds.size(), QImage::Format_Alpha8);
            mask.fill(0);

            QPainter maskPainter(&mask);
            maskPainter.setRenderHint(QPainter::Antialiasing);
            maskPainter.translate(-bounds.topLeft());
            maskPainter.fillPath(path, Qt::white);

            canvas->setSelectionMask(mask, bounds);
        }

        canvas->update();
    }
}

void LassoSelectTool::drawPreview(QPainter *painter, CanvasWidget *canvas)
{
    Q_UNUSED(canvas);
    if (m_selecting && m_points.size() > 1) {
        QPen pen(QColor(0, 120, 215), 0, Qt::DashLine);
        painter->setPen(pen);
        painter->setBrush(Qt::NoBrush);

        QPainterPath path;
        path.moveTo(m_points.first());
        for (int i = 1; i < m_points.size(); ++i) {
            path.lineTo(m_points[i]);
        }
        painter->drawPath(path);
    }
}
