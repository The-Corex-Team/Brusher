#include "FillTool.h"
#include "../canvas/CanvasWidget.h"
#include <algorithm>
#include <cmath>
#include <vector>

void FillTool::mousePressEvent(QMouseEvent *event, CanvasWidget *canvas, QImage *image)
{
    if (event->button() == Qt::LeftButton) {
        QPoint pos = event->position().toPoint();
        if (pos.x() >= 0 && pos.x() < image->width() && pos.y() >= 0 && pos.y() < image->height()) {
            const QRect changed = floodFill(image, pos, canvas->brushColor(), canvas->fillTolerance());
            if (!changed.isEmpty()) {
                canvas->updateCanvasRect(changed);
            }
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

QRect FillTool::floodFill(QImage *image, const QPoint &startPoint, const QColor &fillColor, int tolerance)
{
    if (image->format() != QImage::Format_ARGB32) {
        *image = image->convertToFormat(QImage::Format_ARGB32);
    }

    const QRgb targetColor = image->pixel(startPoint);
    const QRgb replacementColor = fillColor.rgba();

    if (targetColor == replacementColor) {
        return QRect();
    }

    const int w = image->width();
    const int h = image->height();
    int minX = startPoint.x();
    int maxX = startPoint.x();
    int minY = startPoint.y();
    int maxY = startPoint.y();

    std::vector<QPoint> stack;
    stack.reserve(std::min(static_cast<size_t>(w) * static_cast<size_t>(h), static_cast<size_t>(65536)));
    stack.push_back(startPoint);

    while (!stack.empty()) {
        const QPoint p = stack.back();
        stack.pop_back();

        const int x = p.x();
        const int y = p.y();
        QRgb *row = reinterpret_cast<QRgb *>(image->scanLine(y));

        if (!colorMatch(row[x], targetColor, tolerance)) {
            continue;
        }

        int x1 = x;
        while (x1 >= 0 && colorMatch(row[x1], targetColor, tolerance)) {
            row[x1] = replacementColor;
            minX = std::min(minX, x1);
            maxX = std::max(maxX, x1);
            minY = std::min(minY, y);
            maxY = std::max(maxY, y);
            if (y > 0 && colorMatch(image->pixel(x1, y - 1), targetColor, tolerance)) {
                stack.emplace_back(x1, y - 1);
            }
            if (y < h - 1 && colorMatch(image->pixel(x1, y + 1), targetColor, tolerance)) {
                stack.emplace_back(x1, y + 1);
            }
            x1--;
        }

        x1 = x + 1;
        while (x1 < w && colorMatch(row[x1], targetColor, tolerance)) {
            row[x1] = replacementColor;
            minX = std::min(minX, x1);
            maxX = std::max(maxX, x1);
            minY = std::min(minY, y);
            maxY = std::max(maxY, y);
            if (y > 0 && colorMatch(image->pixel(x1, y - 1), targetColor, tolerance)) {
                stack.emplace_back(x1, y - 1);
            }
            if (y < h - 1 && colorMatch(image->pixel(x1, y + 1), targetColor, tolerance)) {
                stack.emplace_back(x1, y + 1);
            }
            x1++;
        }
    }

    return QRect(QPoint(minX, minY), QPoint(maxX, maxY)).adjusted(-1, -1, 1, 1);
}
