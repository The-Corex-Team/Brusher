#pragma once

#include "Tool.h"
#include <QPoint>
#include <QVector>

class LassoSelectTool : public Tool {
public:
    void mousePressEvent(QMouseEvent *event, CanvasWidget *canvas, QImage *image) override;
    void mouseMoveEvent(QMouseEvent *event, CanvasWidget *canvas, QImage *image) override;
    void mouseReleaseEvent(QMouseEvent *event, CanvasWidget *canvas, QImage *image) override;
    void drawPreview(QPainter *painter, CanvasWidget *canvas) override;
    bool isBusy() const override { return m_selecting; }

private:
    QVector<QPoint> m_points;
    bool m_selecting = false;
};
