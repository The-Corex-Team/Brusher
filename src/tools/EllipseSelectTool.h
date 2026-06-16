#pragma once

#include "SelectionTool.h"

class EllipseSelectTool : public SelectionTool {
protected:
    QPainterPath buildPath(const QPoint &start, const QPoint &end) const override;
};
