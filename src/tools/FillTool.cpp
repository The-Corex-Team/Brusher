#include "FillTool.h"
#include "../canvas/CanvasWidget.h"
#include <queue>

void FillTool::mousePressEvent(QMouseEvent *event, CanvasWidget *canvas, QImage *image)
{
    if (event->button() == Qt::LeftButton) {
        QPoint pos = event->position().toPoint();
        if (pos.x() >= 0 && pos.x() < image->width() && pos.y() >= 0 && pos.y() < image->height()) {
            floodFill(image, pos, canvas->brushColor());
            canvas->update();
        }
    }
}

void FillTool::mouseMoveEvent(QMouseEvent *event, CanvasWidget *canvas, QImage *image)
{
    // Do nothing on move
}

void FillTool::mouseReleaseEvent(QMouseEvent *event, CanvasWidget *canvas, QImage *image)
{
    // Do nothing on release
}

bool FillTool::colorMatch(QRgb c1, QRgb c2)
{
    // Exact match for simplicity, can add tolerance later
    return c1 == c2;
}

void FillTool::floodFill(QImage *image, const QPoint &startPoint, const QColor &fillColor)
{
    QRgb targetColor = image->pixel(startPoint);
    QRgb replacementColor = fillColor.rgba();
    
    if (targetColor == replacementColor) {
        return;
    }
    
    int w = image->width();
    int h = image->height();
    
    std::queue<QPoint> q;
    q.push(startPoint);
    
    // Quick BFS scanline fill
    while (!q.empty()) {
        QPoint p = q.front();
        q.pop();
        
        int x = p.x();
        int y = p.y();
        
        if (image->pixel(x, y) != targetColor) continue;
        
        // Scan left
        int x1 = x;
        while (x1 >= 0 && image->pixel(x1, y) == targetColor) {
            image->setPixel(x1, y, replacementColor);
            if (y > 0 && image->pixel(x1, y - 1) == targetColor) q.push(QPoint(x1, y - 1));
            if (y < h - 1 && image->pixel(x1, y + 1) == targetColor) q.push(QPoint(x1, y + 1));
            x1--;
        }
        
        // Scan right
        x1 = x + 1;
        while (x1 < w && image->pixel(x1, y) == targetColor) {
            image->setPixel(x1, y, replacementColor);
            if (y > 0 && image->pixel(x1, y - 1) == targetColor) q.push(QPoint(x1, y - 1));
            if (y < h - 1 && image->pixel(x1, y + 1) == targetColor) q.push(QPoint(x1, y + 1));
            x1++;
        }
    }
}
