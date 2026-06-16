#include "FillTool.h"
#include "../canvas/CanvasWidget.h"
#include <queue>
#include <cmath>

void FillTool::mousePressEvent(QMouseEvent *event, CanvasWidget *canvas, QImage *image)
{
    if (event->button() == Qt::LeftButton) {
        QPoint pos = event->position().toPoint();
        if (pos.x() >= 0 && pos.x() < image->width() && pos.y() >= 0 && pos.y() < image->height()) {
            floodFill(image, pos, canvas->brushColor(), canvas->fillTolerance());
            canvas->update();
        }
    }
}

void FillTool::mouseMoveEvent(QMouseEvent *event, CanvasWidget *canvas, QImage *image)
{
    Q_UNUSED(event);
    Q_UNUSED(canvas);
    Q_UNUSED(image);
}

void FillTool::mouseReleaseEvent(QMouseEvent *event, CanvasWidget *canvas, QImage *image)
{
    Q_UNUSED(event);
    Q_UNUSED(canvas);
    Q_UNUSED(image);
}

bool FillTool::colorMatch(QRgb c1, QRgb c2, int tolerance)
{
    if (tolerance <= 0) {
        return c1 == c2;
    }

    return std::abs(qRed(c1) - qRed(c2)) <= tolerance
        && std::abs(qGreen(c1) - qGreen(c2)) <= tolerance
        && std::abs(qBlue(c1) - qBlue(c2)) <= tolerance
        && std::abs(qAlpha(c1) - qAlpha(c2)) <= tolerance;
}

void FillTool::floodFill(QImage *image, const QPoint &startPoint, const QColor &fillColor, int tolerance)
{
    QRgb targetColor = image->pixel(startPoint);
    QRgb replacementColor = fillColor.rgba();

    if (targetColor == replacementColor) {
        return;
    }

    const int w = image->width();
    const int h = image->height();

    std::queue<QPoint> q;
    q.push(startPoint);

    while (!q.empty()) {
        QPoint p = q.front();
        q.pop();

        int x = p.x();
        int y = p.y();

        if (!colorMatch(image->pixel(x, y), targetColor, tolerance)) {
            continue;
        }

        int x1 = x;
        while (x1 >= 0 && colorMatch(image->pixel(x1, y), targetColor, tolerance)) {
            image->setPixel(x1, y, replacementColor);
            if (y > 0 && colorMatch(image->pixel(x1, y - 1), targetColor, tolerance)) {
                q.push(QPoint(x1, y - 1));
            }
            if (y < h - 1 && colorMatch(image->pixel(x1, y + 1), targetColor, tolerance)) {
                q.push(QPoint(x1, y + 1));
            }
            x1--;
        }

        x1 = x + 1;
        while (x1 < w && colorMatch(image->pixel(x1, y), targetColor, tolerance)) {
            image->setPixel(x1, y, replacementColor);
            if (y > 0 && colorMatch(image->pixel(x1, y - 1), targetColor, tolerance)) {
                q.push(QPoint(x1, y - 1));
            }
            if (y < h - 1 && colorMatch(image->pixel(x1, y + 1), targetColor, tolerance)) {
                q.push(QPoint(x1, y + 1));
            }
            x1++;
        }
    }
}
