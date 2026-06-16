#include "MoveTool.h"
#include "../canvas/CanvasWidget.h"
#include <QPainter>

void MoveTool::mousePressEvent(QMouseEvent *event, CanvasWidget *canvas, QImage *image)
{
    if (event->button() != Qt::LeftButton || !canvas->hasSelection()) {
        return;
    }

    canvas->pushHistoryState("Move Selection");

    m_startPoint = event->position().toPoint();
    m_lastPoint = m_startPoint;
    m_moving = true;

    const QRect bounds = canvas->selectionBounds();
    const QImage &mask = canvas->selectionMask();

    m_floatingPixels = QImage(bounds.size(), QImage::Format_ARGB32);
    m_floatingPixels.fill(Qt::transparent);

    for (int y = 0; y < bounds.height(); ++y) {
        for (int x = 0; x < bounds.width(); ++x) {
            if (qAlpha(mask.pixel(x, y)) > 0) {
                const QPoint canvasPos(bounds.x() + x, bounds.y() + y);
                m_floatingPixels.setPixel(x, y, image->pixel(canvasPos));
                image->setPixel(canvasPos, qRgba(0, 0, 0, 0));
            }
        }
    }

    canvas->update();
}

void MoveTool::mouseMoveEvent(QMouseEvent *event, CanvasWidget *canvas, QImage *image)
{
    Q_UNUSED(image);
    if ((event->buttons() & Qt::LeftButton) && m_moving) {
        m_lastPoint = event->position().toPoint();
        canvas->update();
    }
}

void MoveTool::mouseReleaseEvent(QMouseEvent *event, CanvasWidget *canvas, QImage *image)
{
    if (event->button() != Qt::LeftButton || !m_moving) {
        return;
    }

    m_moving = false;
    const QPoint delta = event->position().toPoint() - m_startPoint;
    const QRect bounds = canvas->selectionBounds();
    const QImage &mask = canvas->selectionMask();

    QPainter painter(image);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

    for (int y = 0; y < bounds.height(); ++y) {
        for (int x = 0; x < bounds.width(); ++x) {
            if (qAlpha(mask.pixel(x, y)) > 0) {
                const QPoint dest(bounds.x() + x + delta.x(), bounds.y() + y + delta.y());
                if (dest.x() >= 0 && dest.x() < image->width() &&
                    dest.y() >= 0 && dest.y() < image->height()) {
                    painter.drawImage(dest, m_floatingPixels, QRect(x, y, 1, 1));
                }
            }
        }
    }

    canvas->setSelectionMask(mask, bounds.translated(delta));
    m_floatingPixels = QImage();
    canvas->update();
}

void MoveTool::drawPreview(QPainter *painter, CanvasWidget *canvas)
{
    if (m_moving && !m_floatingPixels.isNull()) {
        const QPoint delta = m_lastPoint - m_startPoint;
        const QRect bounds = canvas->selectionBounds().translated(delta);
        painter->drawImage(bounds.topLeft(), m_floatingPixels);
    }
}
