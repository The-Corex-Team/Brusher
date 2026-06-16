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
    snapshot.selectionMask = selectionMask.isNull() ? QImage() : selectionMask.copy();
    snapshot.selectionBounds = selectionBounds;

    snapshot.layers.reserve(layers.size());
    for (const auto &layer : layers) {
        Layer copy = layer;
        copy.image = layer.image.copy();
        snapshot.layers.push_back(copy);
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
    selectionMask = this->selectionMask.isNull() ? QImage() : this->selectionMask.copy();
    selectionBounds = this->selectionBounds;

    layers.clear();
    layers.reserve(this->layers.size());
    for (const auto &layer : this->layers) {
        Layer copy = layer;
        copy.image = layer.image.copy();
        layers.push_back(copy);
    }
}
