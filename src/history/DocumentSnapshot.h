#pragma once

#include <QImage>
#include <QSize>
#include <QString>
#include <vector>
#include "../models/Layer.h"

struct DocumentSnapshot {
    QSize canvasSize;
    std::vector<Layer> layers;
    int activeLayerIndex = 0;
    QImage selectionMask;
    QRect selectionBounds;
    QString description;

    static DocumentSnapshot capture(
        const QSize &canvasSize,
        const std::vector<Layer> &layers,
        int activeLayerIndex,
        const QImage &selectionMask,
        const QRect &selectionBounds,
        const QString &description = QString());

    void applyTo(
        QSize &canvasSize,
        std::vector<Layer> &layers,
        int &activeLayerIndex,
        QImage &selectionMask,
        QRect &selectionBounds) const;
};
