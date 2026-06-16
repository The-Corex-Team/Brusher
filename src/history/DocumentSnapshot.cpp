#include "DocumentSnapshot.h"

DocumentSnapshot DocumentSnapshot::capture(
    const QSize &canvasSize,
    const std::vector<Layer> &layers,
    int activeLayerIndex,
    const QImage &selectionMask,
    const QRect &selectionBounds,
    const QString &description)
{
    DocumentSnapshot snapshot;
    snapshot.canvasSize = canvasSize;
    snapshot.activeLayerIndex = activeLayerIndex;
    snapshot.description = description;
    snapshot.selectionMask = selectionMask;
    snapshot.selectionBounds = selectionBounds;

    snapshot.layers.reserve(layers.size());
    for (const auto &layer : layers) {
        snapshot.layers.push_back(layer);
    }

    return snapshot;
}

void DocumentSnapshot::applyTo(
    QSize &canvasSize,
    std::vector<Layer> &layers,
    int &activeLayerIndex,
    QImage &selectionMask,
    QRect &selectionBounds) const
{
    canvasSize = this->canvasSize;
    activeLayerIndex = this->activeLayerIndex;
    selectionMask = this->selectionMask;
    selectionBounds = this->selectionBounds;

    layers = this->layers;
}
