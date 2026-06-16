#pragma once

#include "SelectionTool.h"

class RectSelectTool : public SelectionTool {
protected:
    QPainterPath buildPath(const QPoint &start, const QPoint &end) const override;
};
