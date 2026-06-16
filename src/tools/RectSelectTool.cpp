#include "RectSelectTool.h"

QPainterPath RectSelectTool::buildPath(const QPoint &start, const QPoint &end) const
{
    QPainterPath path;
    path.addRect(QRect(start, end).normalized());
    return path;
}
