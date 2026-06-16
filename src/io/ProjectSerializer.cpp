#include "ProjectSerializer.h"
#include "../history/DocumentSnapshot.h"
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QBuffer>

namespace {

QString imageToBase64(const QImage &image)
{
    QByteArray bytes;
    QBuffer buffer(&bytes);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "PNG");
    return QString::fromLatin1(bytes.toBase64());
}

QImage imageFromBase64(const QString &base64)
{
    const QByteArray bytes = QByteArray::fromBase64(base64.toLatin1());
    QImage image;
    image.loadFromData(bytes, "PNG");
    return image.convertToFormat(QImage::Format_ARGB32);
}

QJsonObject layerToJson(const Layer &layer)
{
    QJsonObject obj;
    obj["name"] = layer.name;
    obj["visible"] = layer.visible;
    obj["opacity"] = layer.opacity;
    obj["blendMode"] = static_cast<int>(layer.blendMode);
    obj["image"] = imageToBase64(layer.image);
    return obj;
}

Layer layerFromJson(const QJsonObject &obj)
{
    Layer layer;
    layer.name = obj["name"].toString();
    layer.visible = obj["visible"].toBool(true);
    layer.opacity = static_cast<float>(obj["opacity"].toDouble(1.0));
    layer.blendMode = static_cast<BlendMode>(obj["blendMode"].toInt(0));
    layer.image = imageFromBase64(obj["image"].toString());
    return layer;
}

QJsonObject snapshotToJson(const DocumentSnapshot &snapshot)
{
    QJsonObject obj;
    obj["description"] = snapshot.description;
    obj["canvasWidth"] = snapshot.canvasSize.width();
    obj["canvasHeight"] = snapshot.canvasSize.height();
    obj["activeLayerIndex"] = snapshot.activeLayerIndex;
    obj["selectionX"] = snapshot.selectionBounds.x();
    obj["selectionY"] = snapshot.selectionBounds.y();
    obj["selectionWidth"] = snapshot.selectionBounds.width();
    obj["selectionHeight"] = snapshot.selectionBounds.height();

    if (!snapshot.selectionMask.isNull()) {
        obj["selectionMask"] = imageToBase64(snapshot.selectionMask);
    }

    QJsonArray layersArray;
    for (const auto &layer : snapshot.layers) {
        layersArray.append(layerToJson(layer));
    }
    obj["layers"] = layersArray;
    return obj;
}

DocumentSnapshot snapshotFromJson(const QJsonObject &obj)
{
    DocumentSnapshot snapshot;
    snapshot.description = obj["description"].toString();
    snapshot.canvasSize = QSize(obj["canvasWidth"].toInt(), obj["canvasHeight"].toInt());
    snapshot.activeLayerIndex = obj["activeLayerIndex"].toInt(0);
    snapshot.selectionBounds = QRect(
        obj["selectionX"].toInt(),
        obj["selectionY"].toInt(),
        obj["selectionWidth"].toInt(),
        obj["selectionHeight"].toInt());

    if (obj.contains("selectionMask")) {
        snapshot.selectionMask = imageFromBase64(obj["selectionMask"].toString());
    }

    const QJsonArray layersArray = obj["layers"].toArray();
    for (const auto &layerVal : layersArray) {
        snapshot.layers.push_back(layerFromJson(layerVal.toObject()));
    }

    return snapshot;
}

} // namespace

bool ProjectSerializer::saveProject(
    const QString &fileName,
    const QSize &canvasSize,
    const std::vector<Layer> &layers,
    int activeLayerIndex,
    const QImage &selectionMask,
    const QRect &selectionBounds,
    const HistoryManager &history)
{
    QJsonObject root;
    root["version"] = 1;
    root["canvasWidth"] = canvasSize.width();
    root["canvasHeight"] = canvasSize.height();
    root["activeLayerIndex"] = activeLayerIndex;
    root["historyIndex"] = history.currentIndex();
    root["selectionX"] = selectionBounds.x();
    root["selectionY"] = selectionBounds.y();
    root["selectionWidth"] = selectionBounds.width();
    root["selectionHeight"] = selectionBounds.height();

    if (!selectionMask.isNull()) {
        root["selectionMask"] = imageToBase64(selectionMask);
    }

    QJsonArray layersArray;
    for (const auto &layer : layers) {
        layersArray.append(layerToJson(layer));
    }
    root["layers"] = layersArray;

    QJsonArray historyArray;
    for (const auto &state : history.states()) {
        historyArray.append(snapshotToJson(state));
    }
    root["history"] = historyArray;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
    return true;
}

bool ProjectSerializer::loadProject(
    const QString &fileName,
    QSize &canvasSize,
    std::vector<Layer> &layers,
    int &activeLayerIndex,
    QImage &selectionMask,
    QRect &selectionBounds,
    HistoryManager &history)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    const QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (!doc.isObject()) {
        return false;
    }

    const QJsonObject root = doc.object();
    canvasSize = QSize(root["canvasWidth"].toInt(), root["canvasHeight"].toInt());
    activeLayerIndex = root["activeLayerIndex"].toInt(0);

    selectionMask = QImage();
    selectionBounds = QRect();
    if (root.contains("selectionMask")) {
        selectionMask = imageFromBase64(root["selectionMask"].toString());
        selectionBounds = QRect(
            root["selectionX"].toInt(),
            root["selectionY"].toInt(),
            root["selectionWidth"].toInt(),
            root["selectionHeight"].toInt());
    }

    layers.clear();
    const QJsonArray layersArray = root["layers"].toArray();
    for (const auto &layerVal : layersArray) {
        layers.push_back(layerFromJson(layerVal.toObject()));
    }

    std::vector<DocumentSnapshot> states;
    const QJsonArray historyArray = root["history"].toArray();
    for (const auto &stateVal : historyArray) {
        states.push_back(snapshotFromJson(stateVal.toObject()));
    }

    if (states.empty()) {
        states.push_back(DocumentSnapshot::capture(
            canvasSize, layers, activeLayerIndex, selectionMask, selectionBounds, "Loaded"));
    }

    const int historyIndex = root.contains("historyIndex")
        ? root["historyIndex"].toInt(static_cast<int>(states.size()) - 1)
        : static_cast<int>(states.size()) - 1;

    history.restoreStates(states, historyIndex);
    return true;
}
