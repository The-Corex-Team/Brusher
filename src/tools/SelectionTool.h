#pragma once

#include "Tool.h"
#include <QPoint>
#include <QPainterPath>

class CanvasWidget;

class SelectionTool : public Tool {
public:
    void mousePressEvent(QMouseEvent *event, CanvasWidget *canvas, QImage *image) override;
    void mouseMoveEvent(QMouseEvent *event, CanvasWidget *canvas, QImage *image) override;
    void mouseReleaseEvent(QMouseEvent *event, CanvasWidget *canvas, QImage *image) override;
    void drawPreview(QPainter *painter, CanvasWidget *canvas) override;
    bool isBusy() const override { return m_selecting; }

protected:
    virtual QPainterPath buildPath(const QPoint &start, const QPoint &end) const = 0;
    virtual void applySelection(CanvasWidget *canvas, const QPainterPath &path);

    QPoint m_startPoint;
    QPoint m_currentPoint;
    bool m_selecting = false;
};
