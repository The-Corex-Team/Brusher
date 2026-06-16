#include "EllipseSelectTool.h"

QPainterPath EllipseSelectTool::buildPath(const QPoint &start, const QPoint &end) const
{
    QPainterPath path;
    path.addEllipse(QRect(start, end).normalized());
    return path;
}
