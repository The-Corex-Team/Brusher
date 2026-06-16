#pragma once

#include <QString>
#include <QImage>
#include "../history/HistoryManager.h"

class ProjectSerializer {
public:
    static bool saveProject(
        const QString &fileName,
        const QSize &canvasSize,
        const std::vector<Layer> &layers,
        int activeLayerIndex,
        const QImage &selectionMask,
        const QRect &selectionBounds,
        const HistoryManager &history);

    static bool loadProject(
        const QString &fileName,
        QSize &canvasSize,
        std::vector<Layer> &layers,
        int &activeLayerIndex,
        QImage &selectionMask,
        QRect &selectionBounds,
        HistoryManager &history);
};
