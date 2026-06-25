#include "CanvasWidget.h"
#include "../tools/PenTool.h"
#include "../tools/EraserTool.h"
#include "../tools/FillTool.h"
#include "../tools/EyedropperTool.h"
#include "../tools/LineTool.h"
#include "../tools/RectSelectTool.h"
#include "../tools/EllipseSelectTool.h"
#include "../tools/LassoSelectTool.h"
#include "../tools/TextTool.h"
#include "../tools/MoveTool.h"
#include "../tools/ZoomTool.h"
#include "../history/DocumentSnapshot.h"
#include "../filters/ImageFilters.h"
#include "../io/ProjectSerializer.h"
#include "../ui/BrushSettingsPopup.h"
#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QPaintEvent>
#include <QTransform>
#include <QFileInfo>
#include <QTemporaryFile>
#include <QStandardPaths>
#include <QDir>
#include <QtMath>
#include <algorithm>
#include <cstdio>
#include <utility>

namespace {

QPainter::CompositionMode compositionModeForBlendMode(BlendMode blendMode)
{
    switch (blendMode) {
        case BlendMode::Multiply: return QPainter::CompositionMode_Multiply;
        case BlendMode::Screen: return QPainter::CompositionMode_Screen;
        case BlendMode::Overlay: return QPainter::CompositionMode_Overlay;
        case BlendMode::Darken: return QPainter::CompositionMode_Darken;
        case BlendMode::Lighten: return QPainter::CompositionMode_Lighten;
        case BlendMode::Normal: return QPainter::CompositionMode_SourceOver;
    }

    return QPainter::CompositionMode_SourceOver;
}

} // namespace

CanvasWidget::CanvasWidget(QWidget *parent)
    : QOpenGLWidget(parent)
    , m_activeLayerIndex(0)
    , m_canvasSize(800, 600)
    , m_brushColor(Qt::black)
    , m_brushSize(5)
    , m_brushOpacity(255)
    , m_fillTolerance(32)
    , m_textFontFamily("Sans Serif")
    , m_textFontSize(24)
    , m_zoomLevel(1.0)
    , m_panOffset(0, 0)
    , m_panning(false)
    , m_spacePanning(false)
    , m_strokeInProgress(false)
    , m_recordingPatchHistory(false)
    , m_currentToolType(Pen)
{
    setUpdateBehavior(QOpenGLWidget::NoPartialUpdate);
    setAttribute(Qt::WA_OpaquePaintEvent, true);
    setAttribute(Qt::WA_NoSystemBackground, true);
    setAutoFillBackground(false);

    Layer bgLayer;
    bgLayer.name = "Background";
    bgLayer.image = QImage(m_canvasSize, QImage::Format_ARGB32);
    bgLayer.image.fill(Qt::white);
    m_layers.push_back(bgLayer);

    initHistory("New Image");
    setTool(Pen);
    setMinimumSize(m_canvasSize);
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);

    // Right-click brush settings popup. Constructed once, owned for the
    // canvas's lifetime; shown via showAt() in mousePressEvent.
    m_brushSettingsPopup = new BrushSettingsPopup(this);
}

CanvasWidget::~CanvasWidget() = default;

DocumentSnapshot CanvasWidget::captureSnapshot(const QString &description) const
{
    return DocumentSnapshot::capture(
        m_canvasSize, m_layers, m_activeLayerIndex, m_selectionMask, m_selectionBounds, description);
}

void CanvasWidget::restoreFromSnapshot(const DocumentSnapshot &snapshot)
{
    snapshot.applyTo(m_canvasSize, m_layers, m_activeLayerIndex, m_selectionMask, m_selectionBounds);
    emit layersChanged();
    update();
}

void CanvasWidget::initHistory(const QString &description)
{
    m_history.clear();
    clearPatchHistory();
    m_history.pushState(captureSnapshot(description));
    emit historyChanged();
}

void CanvasWidget::pushHistoryState(const QString &description)
{
    clearPatchHistory();
    m_history.pushState(captureSnapshot(description));
    emit historyChanged();
    emit documentEdited();
}

void CanvasWidget::beginPatchHistory(const QString &description)
{
    m_pendingPatchHistory = PatchHistoryEntry();
    m_pendingPatchHistory.description = description;
    m_recordingPatchHistory = true;
}

void CanvasWidget::commitPatchHistory()
{
    if (!m_recordingPatchHistory) {
        return;
    }

    m_recordingPatchHistory = false;

    if (m_pendingPatchHistory.patches.empty()) {
        m_pendingPatchHistory = PatchHistoryEntry();
        return;
    }

    for (auto &patch : m_pendingPatchHistory.patches) {
        if (patch.layerIndex >= 0 && patch.layerIndex < static_cast<int>(m_layers.size())) {
            patch.after = m_layers[patch.layerIndex].image.copy(patch.rect);
        }
    }

    m_patchUndoStack.push_back(std::move(m_pendingPatchHistory));
    while (static_cast<int>(m_patchUndoStack.size()) > HistoryManager::MaxHistory) {
        m_patchUndoStack.erase(m_patchUndoStack.begin());
    }
    m_patchRedoStack.clear();
    m_pendingPatchHistory = PatchHistoryEntry();
    emit historyChanged();
    emit documentEdited();
}

void CanvasWidget::clearPatchHistory()
{
    m_patchUndoStack.clear();
    m_patchRedoStack.clear();
    m_pendingPatchHistory = PatchHistoryEntry();
    m_recordingPatchHistory = false;
}

void CanvasWidget::undo()
{
    if (!m_patchUndoStack.empty()) {
        PatchHistoryEntry entry = std::move(m_patchUndoStack.back());
        m_patchUndoStack.pop_back();

        for (auto it = entry.patches.rbegin(); it != entry.patches.rend(); ++it) {
            if (it->layerIndex >= 0 && it->layerIndex < static_cast<int>(m_layers.size())) {
                QPainter painter(&m_layers[it->layerIndex].image);
                painter.setCompositionMode(QPainter::CompositionMode_Source);
                painter.drawImage(it->rect.topLeft(), it->before);
                updateCanvasRect(it->rect);
            }
        }

        m_patchRedoStack.push_back(std::move(entry));
        emit historyChanged();
        emit documentEdited();
        return;
    }

    if (!m_history.canUndo()) {
        return;
    }
    m_patchRedoStack.clear();
    m_history.undo();
    restoreFromSnapshot(m_history.currentState());
    emit historyChanged();
    emit documentEdited();
}

void CanvasWidget::redo()
{
    if (!m_patchRedoStack.empty()) {
        PatchHistoryEntry entry = std::move(m_patchRedoStack.back());
        m_patchRedoStack.pop_back();

        for (const auto &patch : entry.patches) {
            if (patch.layerIndex >= 0 && patch.layerIndex < static_cast<int>(m_layers.size())) {
                QPainter painter(&m_layers[patch.layerIndex].image);
                painter.setCompositionMode(QPainter::CompositionMode_Source);
                painter.drawImage(patch.rect.topLeft(), patch.after);
                updateCanvasRect(patch.rect);
            }
        }

        m_patchUndoStack.push_back(std::move(entry));
        emit historyChanged();
        emit documentEdited();
        return;
    }

    if (!m_history.canRedo()) {
        return;
    }
    clearPatchHistory();
    m_history.redo();
    restoreFromSnapshot(m_history.currentState());
    emit historyChanged();
    emit documentEdited();
}

bool CanvasWidget::canUndo() const { return !m_patchUndoStack.empty() || m_history.canUndo(); }
bool CanvasWidget::canRedo() const { return !m_patchRedoStack.empty() || m_history.canRedo(); }

void CanvasWidget::newCanvas(int width, int height)
{
    m_canvasSize = QSize(width, height);
    m_layers.clear();

    Layer bgLayer;
    bgLayer.name = "Background";
    bgLayer.image = QImage(m_canvasSize, QImage::Format_ARGB32);
    bgLayer.image.fill(Qt::white);
    m_layers.push_back(bgLayer);

    m_activeLayerIndex = 0;
    clearSelection();
    m_zoomLevel = 1.0;
    m_panOffset = QPointF(0, 0);

    initHistory("New Image");
    emit zoomChanged(m_zoomLevel);
    emit layersChanged();
    update();
}

void CanvasWidget::setCanvasSize(int width, int height)
{
    newCanvas(width, height);
}

void CanvasWidget::resizeCanvas(int width, int height)
{
    pushHistoryState("Resize Canvas");

    const QSize oldSize = m_canvasSize;
    m_canvasSize = QSize(width, height);

    const int offsetX = (width - oldSize.width()) / 2;
    const int offsetY = (height - oldSize.height()) / 2;

    for (auto &layer : m_layers) {
        QImage newImage(m_canvasSize, QImage::Format_ARGB32);
        newImage.fill(Qt::transparent);
        QPainter painter(&newImage);
        painter.drawImage(offsetX, offsetY, layer.image);
        layer.image = newImage;
    }

    if (!m_selectionMask.isNull()) {
        clearSelection();
    }

    emit layersChanged();
    update();
}

void CanvasWidget::rotateCanvas(bool clockwise)
{
    pushHistoryState(clockwise ? "Rotate Clockwise" : "Rotate Counter-Clockwise");

    m_canvasSize = QSize(m_canvasSize.height(), m_canvasSize.width());

    for (auto &layer : m_layers) {
        QTransform transform;
        transform.rotate(clockwise ? 90 : -90);
        layer.image = layer.image.transformed(transform, Qt::SmoothTransformation);
    }

    if (!m_selectionMask.isNull()) {
        QTransform transform;
        transform.rotate(clockwise ? 90 : -90);
        m_selectionMask = m_selectionMask.transformed(transform, Qt::SmoothTransformation);
        m_selectionBounds = QRect(0, 0, m_canvasSize.width(), m_canvasSize.height());
    }

    emit layersChanged();
    update();
}

void CanvasWidget::flipCanvas(bool horizontal)
{
    pushHistoryState(horizontal ? "Flip Horizontal" : "Flip Vertical");

    for (auto &layer : m_layers) {
        layer.image = layer.image.mirrored(horizontal, !horizontal);
    }

    if (!m_selectionMask.isNull()) {
        m_selectionMask = m_selectionMask.mirrored(horizontal, !horizontal);
    }

    update();
}

void CanvasWidget::cropToSelection()
{
    if (!hasSelection()) {
        return;
    }

    pushHistoryState("Crop");

    const QRect cropRect = m_selectionBounds;
    m_canvasSize = cropRect.size();

    for (auto &layer : m_layers) {
        layer.image = layer.image.copy(cropRect);
    }

    m_selectionMask = QImage();
    m_selectionBounds = QRect();

    emit layersChanged();
    update();
}

void CanvasWidget::addLayer(const QString &name)
{
    pushHistoryState("New Layer");

    Layer newLayer;
    newLayer.name = name;
    newLayer.image = QImage(m_canvasSize, QImage::Format_ARGB32);
    newLayer.image.fill(Qt::transparent);

    m_layers.insert(m_layers.begin() + m_activeLayerIndex + 1, newLayer);
    m_activeLayerIndex++;

    emit layersChanged();
    update();
}

void CanvasWidget::removeLayer(int index)
{
    if (index >= 0 && index < static_cast<int>(m_layers.size()) && m_layers.size() > 1) {
        pushHistoryState("Delete Layer");
        m_layers.erase(m_layers.begin() + index);
        if (m_activeLayerIndex >= static_cast<int>(m_layers.size())) {
            m_activeLayerIndex = static_cast<int>(m_layers.size()) - 1;
        }
        emit layersChanged();
        update();
    }
}

void CanvasWidget::duplicateLayer(int index)
{
    if (index >= 0 && index < static_cast<int>(m_layers.size())) {
        pushHistoryState("Duplicate Layer");

        Layer newLayer = m_layers[index];
        newLayer.name += " copy";
        newLayer.image = m_layers[index].image.copy();

        m_layers.insert(m_layers.begin() + index + 1, newLayer);
        m_activeLayerIndex = index + 1;

        emit layersChanged();
        update();
    }
}

void CanvasWidget::moveLayer(int fromIndex, int toIndex)
{
    const int layerCount = static_cast<int>(m_layers.size());
    if (fromIndex < 0 || fromIndex >= layerCount || toIndex < 0 || toIndex >= layerCount || fromIndex == toIndex) {
        return;
    }

    // Only push history once we know the move is real. A no-op or
    // out-of-range reorder would otherwise pollute the undo stack.
    pushHistoryState("Reorder Layers");

    Layer movedLayer = std::move(m_layers[fromIndex]);
    m_layers.erase(m_layers.begin() + fromIndex);
    m_layers.insert(m_layers.begin() + toIndex, std::move(movedLayer));

    if (m_activeLayerIndex == fromIndex) {
        m_activeLayerIndex = toIndex;
    } else if (fromIndex < m_activeLayerIndex && toIndex >= m_activeLayerIndex) {
        --m_activeLayerIndex;
    } else if (fromIndex > m_activeLayerIndex && toIndex <= m_activeLayerIndex) {
        ++m_activeLayerIndex;
    }

    emit layersChanged();
    update();
}

void CanvasWidget::renameLayer(int index, const QString &name)
{
    const QString trimmedName = name.trimmed();
    if (index < 0 || index >= static_cast<int>(m_layers.size()) || trimmedName.isEmpty()
        || m_layers[index].name == trimmedName) {
        return;
    }

    pushHistoryState("Rename Layer");
    m_layers[index].name = trimmedName;
    emit layersChanged();
}

void CanvasWidget::setActiveLayer(int index)
{
    if (index >= 0 && index < static_cast<int>(m_layers.size())) {
        m_activeLayerIndex = index;
    }
}

void CanvasWidget::setLayerVisible(int index, bool visible)
{
    if (index >= 0 && index < static_cast<int>(m_layers.size())) {
        m_layers[index].visible = visible;
        update();
    }
}

void CanvasWidget::setLayerOpacity(int index, float opacity)
{
    if (index >= 0 && index < static_cast<int>(m_layers.size())) {
        m_layers[index].opacity = std::clamp(opacity, 0.0f, 1.0f);
        update();
    }
}

void CanvasWidget::setLayerBlendMode(int index, BlendMode mode)
{
    if (index >= 0 && index < static_cast<int>(m_layers.size())) {
        m_layers[index].blendMode = mode;
        update();
    }
}

void CanvasWidget::flattenAllLayers()
{
    pushHistoryState("Flatten Image");

    QImage flat = blendLayers();
    m_layers.clear();

    Layer bgLayer;
    bgLayer.name = "Background";
    bgLayer.image = flat;
    m_layers.push_back(bgLayer);
    m_activeLayerIndex = 0;

    emit layersChanged();
    update();
}

int CanvasWidget::getLayerCount() const { return static_cast<int>(m_layers.size()); }
QString CanvasWidget::getLayerName(int index) const
{
    return index >= 0 && index < static_cast<int>(m_layers.size()) ? m_layers[index].name : QString();
}
int CanvasWidget::getActiveLayerIndex() const { return m_activeLayerIndex; }
bool CanvasWidget::getLayerVisible(int index) const
{
    return index >= 0 && index < static_cast<int>(m_layers.size()) ? m_layers[index].visible : false;
}
float CanvasWidget::getLayerOpacity(int index) const
{
    return index >= 0 && index < static_cast<int>(m_layers.size()) ? m_layers[index].opacity : 1.0f;
}
BlendMode CanvasWidget::getLayerBlendMode(int index) const
{
    return index >= 0 && index < static_cast<int>(m_layers.size()) ? m_layers[index].blendMode : BlendMode::Normal;
}

TextTool *CanvasWidget::textTool() const
{
    return dynamic_cast<TextTool *>(m_activeTool.get());
}

void CanvasWidget::setTool(ToolType type)
{
    m_currentToolType = type;
    if (type == Pen) m_activeTool = std::make_unique<PenTool>();
    else if (type == Eraser) m_activeTool = std::make_unique<EraserTool>();
    else if (type == Fill) m_activeTool = std::make_unique<FillTool>();
    else if (type == Eyedropper) m_activeTool = std::make_unique<EyedropperTool>();
    else if (type == Line) m_activeTool = std::make_unique<LineTool>();
    else if (type == Move) m_activeTool = std::make_unique<MoveTool>();
    else if (type == RectSelect) m_activeTool = std::make_unique<RectSelectTool>();
    else if (type == EllipseSelect) m_activeTool = std::make_unique<EllipseSelectTool>();
    else if (type == LassoSelect) m_activeTool = std::make_unique<LassoSelectTool>();
    else if (type == Text) {
        auto tool = std::make_unique<TextTool>();
        tool->setFontFamily(m_textFontFamily);
        tool->setFontSize(m_textFontSize);
        m_activeTool = std::move(tool);
    }
    else if (type == Zoom) m_activeTool = std::make_unique<ZoomTool>();
    else m_activeTool.reset();

    emit toolChanged(type);
}

void CanvasWidget::setBrushColor(const QColor &color)
{
    if (m_brushColor != color) {
        m_brushColor = color;
        emit brushColorChanged(m_brushColor);
    }
}

void CanvasWidget::setBrushSize(int size)
{
    const int clamped = std::clamp(size, 1, 200);
    if (m_brushSize != clamped) {
        m_brushSize = clamped;
        emit brushSizeChanged(m_brushSize);
    }
}

void CanvasWidget::setBrushOpacity(int opacity)
{
    const int clamped = std::clamp(opacity, 0, 255);
    if (m_brushOpacity != clamped) {
        m_brushOpacity = clamped;
        emit brushOpacityChanged(m_brushOpacity);
    }
}
void CanvasWidget::setFillTolerance(int tolerance) { m_fillTolerance = std::clamp(tolerance, 0, 255); }
void CanvasWidget::setTextFontFamily(const QString &family) { m_textFontFamily = family; }
void CanvasWidget::setTextFontSize(int size) { m_textFontSize = std::clamp(size, 6, 200); }

void CanvasWidget::setSelectionMask(const QImage &mask, const QRect &bounds)
{
    m_selectionMask = mask;
    m_selectionBounds = bounds;
    update();
}

void CanvasWidget::clearSelection()
{
    m_selectionMask = QImage();
    m_selectionBounds = QRect();
    update();
}

bool CanvasWidget::hasSelection() const
{
    return !m_selectionMask.isNull() && !m_selectionBounds.isEmpty();
}

QRect CanvasWidget::effectiveFilterRegion() const
{
    if (hasSelection()) {
        return m_selectionBounds;
    }
    return QRect(0, 0, m_canvasSize.width(), m_canvasSize.height());
}

void CanvasWidget::applyFilterToActiveLayer(const QImage &filtered, const QRect &region)
{
    if (m_layers.empty()) {
        return;
    }

    QImage &layerImage = m_layers[m_activeLayerIndex].image;

    if (hasSelection()) {
        for (int y = 0; y < region.height(); ++y) {
            for (int x = 0; x < region.width(); ++x) {
                if (qAlpha(m_selectionMask.pixel(x, y)) > 0) {
                    const QPoint pos(region.x() + x, region.y() + y);
                    layerImage.setPixel(pos, filtered.pixel(x, y));
                }
            }
        }
    } else {
        QPainter painter(&layerImage);
        painter.drawImage(region.topLeft(), filtered);
    }
}

void CanvasWidget::applyBlurFilter(int radius)
{
    if (m_layers.empty()) {
        return;
    }

    pushHistoryState("Blur");

    const QRect region = effectiveFilterRegion();
    const QImage source = m_layers[m_activeLayerIndex].image.copy(region);
    const QImage filtered = ImageFilters::applyBlur(source, radius);
    applyFilterToActiveLayer(filtered, region);
    update();
}

void CanvasWidget::applySharpenFilter(float amount)
{
    if (m_layers.empty()) {
        return;
    }

    pushHistoryState("Sharpen");

    const QRect region = effectiveFilterRegion();
    const QImage source = m_layers[m_activeLayerIndex].image.copy(region);
    const QImage filtered = ImageFilters::applySharpen(source, amount);
    applyFilterToActiveLayer(filtered, region);
    update();
}

void CanvasWidget::clearCanvas()
{
    if (!m_layers.empty()) {
        pushHistoryState("Clear Canvas");
        m_layers[m_activeLayerIndex].image.fill(Qt::transparent);
        update();
    }
}

QImage CanvasWidget::blendLayers() const
{
    const auto visibleLayer = std::find_if(m_layers.begin(), m_layers.end(), [](const Layer &layer) {
        return layer.visible && layer.opacity > 0.0f;
    });

    if (visibleLayer != m_layers.end()
        && std::find_if(std::next(visibleLayer), m_layers.end(), [](const Layer &layer) {
               return layer.visible && layer.opacity > 0.0f;
           }) == m_layers.end()
        && visibleLayer->opacity >= 1.0f
        && visibleLayer->blendMode == BlendMode::Normal) {
        return visibleLayer->image;
    }

    QImage flat(m_canvasSize, QImage::Format_ARGB32);
    flat.fill(Qt::transparent);

    QPainter painter(&flat);
    for (const auto &layer : m_layers) {
        if (layer.visible) {
            painter.setOpacity(layer.opacity);
            painter.setCompositionMode(compositionModeForBlendMode(layer.blendMode));
            painter.drawImage(0, 0, layer.image);
        }
    }
    return flat;
}

QImage CanvasWidget::getFlattenedImage() const
{
    return blendLayers();
}

void CanvasWidget::updateCanvasRect(const QRect &canvasRect)
{
    if (canvasRect.isEmpty()) {
        return;
    }

    QTransform transform;
    transform.translate(width() / 2.0 + m_panOffset.x(), height() / 2.0 + m_panOffset.y());
    transform.scale(m_zoomLevel, m_zoomLevel);
    transform.translate(-m_canvasSize.width() / 2.0, -m_canvasSize.height() / 2.0);

    update(transform.mapRect(QRectF(canvasRect)).toAlignedRect().adjusted(-2, -2, 2, 2));
}

void CanvasWidget::recordActiveLayerHistoryRegion(const QRect &canvasRect)
{
    if (!m_recordingPatchHistory || m_layers.empty()) {
        return;
    }

    const QRect imageBounds(QPoint(0, 0), m_canvasSize);
    const QRect clipped = canvasRect.intersected(imageBounds);
    if (clipped.isEmpty()) {
        return;
    }

    constexpr int TileSize = 128;
    const int startTileX = clipped.left() / TileSize;
    const int endTileX = clipped.right() / TileSize;
    const int startTileY = clipped.top() / TileSize;
    const int endTileY = clipped.bottom() / TileSize;

    for (int tileY = startTileY; tileY <= endTileY; ++tileY) {
        for (int tileX = startTileX; tileX <= endTileX; ++tileX) {
            const QRect tileRect(tileX * TileSize, tileY * TileSize, TileSize, TileSize);
            const QRect patchRect = tileRect.intersected(imageBounds);
            const auto alreadyCaptured = std::find_if(
                m_pendingPatchHistory.patches.begin(),
                m_pendingPatchHistory.patches.end(),
                [this, &patchRect](const LayerPatch &patch) {
                    return patch.layerIndex == m_activeLayerIndex && patch.rect == patchRect;
                });

            if (alreadyCaptured != m_pendingPatchHistory.patches.end()) {
                continue;
            }

            LayerPatch patch;
            patch.layerIndex = m_activeLayerIndex;
            patch.rect = patchRect;
            patch.before = m_layers[m_activeLayerIndex].image.copy(patchRect);
            m_pendingPatchHistory.patches.push_back(std::move(patch));
        }
    }
}

bool CanvasWidget::saveImage(const QString &fileName)
{
    return getFlattenedImage().save(fileName);
}

bool CanvasWidget::exportImage(const QString &fileName)
{
    return getFlattenedImage().save(fileName);
}

bool CanvasWidget::saveProject(const QString &fileName)
{
    return ProjectSerializer::saveProject(
        fileName, m_canvasSize, m_layers, m_activeLayerIndex,
        m_selectionMask, m_selectionBounds, m_history);
}

bool CanvasWidget::openProject(const QString &fileName)
{
    QSize size;
    std::vector<Layer> layers;
    int activeLayer = 0;
    QImage selectionMask;
    QRect selectionBounds;

    if (!ProjectSerializer::loadProject(
            fileName, size, layers, activeLayer, selectionMask, selectionBounds, m_history)) {
        return false;
    }

    m_canvasSize = size;
    m_layers = std::move(layers);
    m_activeLayerIndex = activeLayer;
    m_selectionMask = selectionMask;
    m_selectionBounds = selectionBounds;

    m_zoomLevel = 1.0;
    m_panOffset = QPointF(0, 0);

    emit layersChanged();
    emit zoomChanged(m_zoomLevel);
    emit historyChanged();
    emit documentEdited();
    update();
    return true;
}

bool CanvasWidget::openImage(const QString &fileName)
{
    if (fileName.endsWith(".brusher", Qt::CaseInsensitive)) {
        return openProject(fileName);
    }

    QImage newImage;
    if (!newImage.load(fileName)) {
        return false;
    }

    newImage = newImage.convertToFormat(QImage::Format_ARGB32);
    m_canvasSize = newImage.size();
    m_layers.clear();

    Layer layer;
    QFileInfo info(fileName);
    layer.name = info.baseName();
    layer.image = newImage;
    m_layers.push_back(layer);
    m_activeLayerIndex = 0;

    clearSelection();
    m_zoomLevel = 1.0;
    m_panOffset = QPointF(0, 0);

    initHistory("Open Image");
    emit layersChanged();
    emit zoomChanged(m_zoomLevel);
    update();
    return true;
}

void CanvasWidget::applyZoomAt(const QPointF &widgetPos, double factor)
{
    const QPointF canvasBefore = widgetToCanvas(widgetPos);
    m_zoomLevel = std::clamp(m_zoomLevel * factor, 0.05, 32.0);

    const double cx = width() / 2.0;
    const double cy = height() / 2.0;
    const double canvasCenterX = m_canvasSize.width() / 2.0;
    const double canvasCenterY = m_canvasSize.height() / 2.0;

    const double newWidgetX = (canvasBefore.x() - canvasCenterX) * m_zoomLevel + cx + m_panOffset.x();
    const double newWidgetY = (canvasBefore.y() - canvasCenterY) * m_zoomLevel + cy + m_panOffset.y();

    m_panOffset.setX(m_panOffset.x() + (widgetPos.x() - newWidgetX));
    m_panOffset.setY(m_panOffset.y() + (widgetPos.y() - newWidgetY));

    emit zoomChanged(m_zoomLevel);
    update();
}

void CanvasWidget::zoomIn() { applyZoomAt(QPointF(width() / 2.0, height() / 2.0), 1.25); }
void CanvasWidget::zoomOut() { applyZoomAt(QPointF(width() / 2.0, height() / 2.0), 1.0 / 1.25); }
void CanvasWidget::zoomInAt(const QPointF &widgetPos) { applyZoomAt(widgetPos, 1.25); }
void CanvasWidget::zoomOutAt(const QPointF &widgetPos) { applyZoomAt(widgetPos, 1.0 / 1.25); }

void CanvasWidget::fitToWindow()
{
    const double zoomX = static_cast<double>(width()) / m_canvasSize.width();
    const double zoomY = static_cast<double>(height()) / m_canvasSize.height();
    m_zoomLevel = std::min(zoomX, zoomY) * 0.95;
    m_panOffset = QPointF(0, 0);
    emit zoomChanged(m_zoomLevel);
    update();
}

void CanvasWidget::actualSize()
{
    m_zoomLevel = 1.0;
    emit zoomChanged(m_zoomLevel);
    update();
}

void CanvasWidget::drawCheckerboard(QPainter &painter, const QRect &rect)
{
    const int checkSize = 10;
    const QColor color1(200, 200, 200);
    const QColor color2(255, 255, 255);

    for (int y = rect.top() - (rect.top() % checkSize); y < rect.bottom(); y += checkSize) {
        for (int x = rect.left() - (rect.left() % checkSize); x < rect.right(); x += checkSize) {
            const bool isEven = ((x / checkSize) + (y / checkSize)) % 2 == 0;
            painter.fillRect(x, y, checkSize, checkSize, isEven ? color1 : color2);
        }
    }
}

void CanvasWidget::drawSelectionOverlay(QPainter &painter)
{
    if (!hasSelection()) {
        return;
    }

    QPen pen(QColor(0, 120, 215), 0, Qt::DashLine);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(m_selectionBounds);
}

QPointF CanvasWidget::widgetToCanvas(const QPointF &widgetPos) const
{
    const double cx = width() / 2.0;
    const double cy = height() / 2.0;

    const double dx = widgetPos.x() - cx - m_panOffset.x();
    const double dy = widgetPos.y() - cy - m_panOffset.y();

    return QPointF(
        (dx / m_zoomLevel) + (m_canvasSize.width() / 2.0),
        (dy / m_zoomLevel) + (m_canvasSize.height() / 2.0));
}

void CanvasWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.fillRect(rect(), QColor("#535353"));

    const double cx = width() / 2.0;
    const double cy = height() / 2.0;

    QTransform transform;
    transform.translate(cx + m_panOffset.x(), cy + m_panOffset.y());
    transform.scale(m_zoomLevel, m_zoomLevel);
    transform.translate(-m_canvasSize.width() / 2.0, -m_canvasSize.height() / 2.0);

    const QRect canvasRect(0, 0, m_canvasSize.width(), m_canvasSize.height());
    painter.setTransform(transform);

    drawCheckerboard(painter, canvasRect);

    painter.save();
    painter.setPen(QPen(Qt::black, 0));
    painter.drawRect(canvasRect);
    painter.restore();

    for (const auto &layer : m_layers) {
        if (!layer.visible) {
            continue;
        }

        painter.setOpacity(layer.opacity);
        painter.setCompositionMode(compositionModeForBlendMode(layer.blendMode));
        painter.drawImage(0, 0, layer.image);
    }

    painter.setOpacity(1.0);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    drawSelectionOverlay(painter);

    if (m_activeTool) {
        m_activeTool->drawPreview(&painter, this);
    }

    // Brush-size cursor indicator goes on top of everything so it always
    // stays visible, even over a tool's in-progress preview stroke.
    drawBrushSizeIndicator(&painter);
}

bool CanvasWidget::isPanActive(QMouseEvent *event) const
{
    return event->button() == Qt::MiddleButton
        || (m_currentToolType == Pan && event->button() == Qt::LeftButton)
        || (m_spacePanning && event->button() == Qt::LeftButton);
}

void CanvasWidget::mousePressEvent(QMouseEvent *event)
{
    if (isPanActive(event)) {
        m_panning = true;
        m_panStartPos = event->position();
        m_panStartOffset = m_panOffset;
        setCursor(Qt::ClosedHandCursor);
        return;
    }

    // Right-click over the canvas, while a brush-using tool is active,
    // pops up the Brush Settings menu at the cursor. Anything else
    // (selection tools, fill, eyedropper, etc.) ignores right-click.
    if (event->button() == Qt::RightButton && isDrawingTool() && m_brushSettingsPopup) {
        m_brushSettingsPopup->showAt(event->globalPosition().toPoint());
        event->accept();
        return;
    }

    if (m_activeTool && !m_layers.empty() && m_layers[m_activeLayerIndex].visible) {
        const bool isDrawingTool = m_currentToolType == Pen
            || m_currentToolType == Eraser
            || m_currentToolType == Line;

        if (isDrawingTool && event->button() == Qt::LeftButton && !m_strokeInProgress) {
            beginPatchHistory("Draw");
            m_strokeInProgress = true;
        } else if (m_currentToolType == Fill && event->button() == Qt::LeftButton) {
            pushHistoryState("Fill");
        }

        QMouseEvent mappedEvent(
            event->type(),
            widgetToCanvas(event->position()),
            widgetToCanvas(event->globalPosition()),
            event->button(),
            event->buttons(),
            event->modifiers());
        m_activeTool->mousePressEvent(&mappedEvent, this, &m_layers[m_activeLayerIndex].image);
    }
}

void CanvasWidget::mouseMoveEvent(QMouseEvent *event)
{
    // Cache the hover position unconditionally so the brush-size indicator
    // can paint with the freshest cursor location on the next frame, then
    // throttle repaints to the cases that actually move the indicator.
    if (m_lastMousePos != event->position()) {
        m_lastMousePos = event->position();
        if (m_mouseInside && !m_panning && !m_strokeInProgress && isDrawingTool()) {
            update();
        }
    }

    if (m_panning || (event->buttons() & Qt::MiddleButton)) {
        if (!m_panning) {
            m_panning = true;
            m_panStartPos = event->position();
            m_panStartOffset = m_panOffset;
            setCursor(Qt::ClosedHandCursor);
        }
        m_panOffset = m_panStartOffset + (event->position() - m_panStartPos);
        update();
        return;
    }

    if (m_activeTool && !m_layers.empty() && m_layers[m_activeLayerIndex].visible) {
        QMouseEvent mappedEvent(
            event->type(),
            widgetToCanvas(event->position()),
            widgetToCanvas(event->globalPosition()),
            event->button(),
            event->buttons(),
            event->modifiers());
        m_activeTool->mouseMoveEvent(&mappedEvent, this, &m_layers[m_activeLayerIndex].image);
    }
}

void CanvasWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_panning && (event->button() == Qt::MiddleButton || event->button() == Qt::LeftButton)) {
        m_panning = false;
        setCursor(Qt::ArrowCursor);
        return;
    }

    if (m_activeTool && !m_layers.empty() && m_layers[m_activeLayerIndex].visible) {
        QMouseEvent mappedEvent(
            event->type(),
            widgetToCanvas(event->position()),
            widgetToCanvas(event->globalPosition()),
            event->button(),
            event->buttons(),
            event->modifiers());
        m_activeTool->mouseReleaseEvent(&mappedEvent, this, &m_layers[m_activeLayerIndex].image);
    }

    if (m_strokeInProgress && event->button() == Qt::LeftButton) {
        m_strokeInProgress = false;
        commitPatchHistory();
    }
}

void CanvasWidget::wheelEvent(QWheelEvent *event)
{
    // Ctrl+Alt+wheel adjusts brush size by ±10% per notch (GIMP-style).
    // Without modifiers, the wheel zooms the canvas as before.
    const bool sizeShortcut =
        (event->modifiers() & (Qt::ControlModifier | Qt::AltModifier))
            == (Qt::ControlModifier | Qt::AltModifier);

    if (sizeShortcut) {
        const int step = std::max(1, static_cast<int>(qRound(m_brushSize * 0.10)));
        // Use the dominant axis (|y| vs |x|): many Linux/X11 mice report
        // vertical wheel motion on the x channel instead of y.
        const int dy = event->angleDelta().y();
        const int dx = event->angleDelta().x();
        const int delta = (dy != 0 ? dy : dx) > 0 ? step : -step;
        setBrushSize(m_brushSize + delta);
        event->accept();
        return;
    }

    const double factor = event->angleDelta().y() > 0 ? 1.1 : 1.0 / 1.1;
    applyZoomAt(event->position(), factor);
    event->accept();
}

void CanvasWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space && !event->isAutoRepeat()) {
        m_spacePanning = true;
        setCursor(Qt::OpenHandCursor);
    }
    QOpenGLWidget::keyPressEvent(event);
}

void CanvasWidget::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space && !event->isAutoRepeat()) {
        m_spacePanning = false;
        if (!m_panning) {
            setCursor(Qt::ArrowCursor);
        }
    }
    QOpenGLWidget::keyReleaseEvent(event);
}

// ---------------------------------------------------------------------------
// Low Memory Mode — idle-resource release/restore.
//
// When the user enables Low Memory Mode and the main window is deactivated or
// minimized, we write hidden layer images and full history snapshots out to
// QTemporaryFile objects in the system temp dir, then drop the in-process
// QImage buffers. The selection mask and the patch undo/redo stacks are
// discarded (cheap to rebuild — the user can re-select or just keeps editing
// with the patched history truncated). The active, visible layer is never
// released: the user is editing it.
//
// On window reactivation we read the temp files back into fresh QImages,
// restore the selection/patch state, and request a repaint. The OS handles
// deleting the temp files when the QFile objects are destroyed.
// ---------------------------------------------------------------------------

namespace {

// Create a brand-new QTemporaryFile for a layer/snapshot image. Ownership of
// the returned pointer transfers to the caller; the file stays open (and
// pinned on disk) for the duration of the release window.
QTemporaryFile *createBrusherTempFile()
{
    const QString dir = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    QTemporaryFile *file = new QTemporaryFile(
        QDir(dir).filePath(QStringLiteral("brusher-XXXXXX.png")));
    file->setAutoRemove(true);
    if (!file->open()) {
        delete file;
        return nullptr;
    }
    return file;
}

} // namespace


QImage CanvasWidget::readImageFromMappedFile(const MappedImage &mapped)
{
    if (mapped.path.isEmpty()) {
        return QImage();
    }

    QFile in(mapped.path);
    if (!in.open(QIODevice::ReadOnly)) {
        return QImage();
    }
    const QByteArray bytes = in.readAll();
    in.close();

    QImage image;
    image.loadFromData(bytes, "PNG");

    // Defensive: ensure the format we rehydrate to matches what we saved, so
    // downstream blend math doesn't see a surprise Format_Invalid.
    if (!image.isNull() && image.format() != mapped.format) {
        image = image.convertToFormat(mapped.format);
    }
    return image;
}

bool CanvasWidget::isBusy() const
{
    if (m_strokeInProgress) {
        return true;
    }
    if (m_activeTool && m_activeTool->isBusy()) {
        return true;
    }
    return false;
}

void CanvasWidget::setLowMemoryMode(bool enabled)
{
    if (m_lowMemoryMode == enabled) {
        return;
    }
    m_lowMemoryMode = enabled;
    emit lowMemoryModeChanged(m_lowMemoryMode);

    // Turning the mode off while released restores immediately so the user
    // can interact normally without waiting for a focus event.
    if (!enabled && m_resourcesReleased) {
        restoreIdleResources();
    }
}

void CanvasWidget::mapLayerOut(int layerIndex)
{
    if (layerIndex < 0 || layerIndex >= static_cast<int>(m_layers.size())) {
        return;
    }
    QImage &image = m_layers[layerIndex].image;
    if (image.isNull()) {
        return;
    }

    QTemporaryFile *file = createBrusherTempFile();
    if (!file) {
        return;
    }
    if (!image.save(file, "PNG")) {
        delete file;
        return;
    }
    file->close();

    MappedImage mapped;
    mapped.size = image.size();
    mapped.format = image.format();
    mapped.path = file->fileName();
    mapped.bytesPerLine = static_cast<int>(image.bytesPerLine());
    mapped.layerIndex = layerIndex;

    m_idleTempFiles.push_back(file);

    // Drop the in-RAM pixels.
    image = QImage();
}

void CanvasWidget::mapSnapshotOut(int historyIndex)
{
    std::vector<DocumentSnapshot> &states = m_history.mutableStates();
    if (historyIndex < 0 || historyIndex >= static_cast<int>(states.size())) {
        return;
    }
    DocumentSnapshot &snap = states[historyIndex];
    std::vector<MappedImage> layerMaps;
    layerMaps.reserve(snap.layers.size());

    for (auto &layer : snap.layers) {
        if (layer.image.isNull()) {
            layerMaps.push_back(MappedImage{});
            continue;
        }

        QTemporaryFile *file = createBrusherTempFile();
        if (!file) {
            // Roll back: rehydrate everything we've already swapped so the
            // snapshot remains consistent.
            for (size_t i = 0; i < layerMaps.size() && i < snap.layers.size(); ++i) {
                snap.layers[i].image = readImageFromMappedFile(layerMaps[i]);
            }
            return;
        }
        if (!layer.image.save(file, "PNG")) {
            delete file;
            for (size_t i = 0; i < layerMaps.size() && i < snap.layers.size(); ++i) {
                snap.layers[i].image = readImageFromMappedFile(layerMaps[i]);
            }
            return;
        }
        file->close();

        MappedImage m;
        m.size = layer.image.size();
        m.format = layer.image.format();
        m.path = file->fileName();
        m.bytesPerLine = static_cast<int>(layer.image.bytesPerLine());
        layerMaps.push_back(m);

        m_idleTempFiles.push_back(file);
        layer.image = QImage();
    }

    m_mappedSnapshots.push_back({historyIndex, std::move(layerMaps)});
}

void CanvasWidget::releaseSelectionAndPatches()
{
    m_savedSelectionMask = m_selectionMask;
    m_savedSelectionBounds = m_selectionBounds;
    m_savedPatchUndoStack = m_patchUndoStack;
    m_savedPatchRedoStack = m_patchRedoStack;
    m_selectionMask = QImage();
    m_selectionBounds = QRect();
    m_patchUndoStack.clear();
    m_patchRedoStack.clear();
}

void CanvasWidget::restoreSelectionAndPatches()
{
    m_selectionMask = m_savedSelectionMask;
    m_selectionBounds = m_savedSelectionBounds;
    m_patchUndoStack = std::move(m_savedPatchUndoStack);
    m_patchRedoStack = std::move(m_savedPatchRedoStack);
    m_savedSelectionMask = QImage();
    m_savedSelectionBounds = QRect();
    m_savedPatchUndoStack.clear();
    m_savedPatchRedoStack.clear();
}

void CanvasWidget::releaseIdleResources()
{
    if (!m_lowMemoryMode || m_resourcesReleased) {
        return;
    }
    if (isBusy()) {
        return;
    }

    // 1. Hidden layers (anything not currently the active layer).
    const int layerCount = static_cast<int>(m_layers.size());
    for (int i = 0; i < layerCount; ++i) {
        if (i == m_activeLayerIndex) {
            continue;
        }
        mapLayerOut(i);
    }

    // 2. History snapshots. We map every snapshot's layer images out; the
    //    snapshot metadata (description, indices, selection) stays in RAM.
    const int snapCount = static_cast<int>(m_history.mutableStates().size());
    for (int i = 0; i < snapCount; ++i) {
        mapSnapshotOut(i);
    }

    // 3. Selection and patch stacks (lossy — documented).
    releaseSelectionAndPatches();

    m_resourcesReleased = true;
    setUpdatesEnabled(false);
    emit historyChanged();
}

void CanvasWidget::restoreIdleResources()
{
    if (!m_resourcesReleased) {
        return;
    }

    // Hidden layers.
    for (const MappedImage &mapped : m_mappedHiddenLayers) {
        // The layerIndex was tacked on by mapLayerOut via a parallel field;
        // see MappedImage extension below.
        if (mapped.layerIndex >= 0 && mapped.layerIndex < static_cast<int>(m_layers.size())) {
            m_layers[mapped.layerIndex].image = readImageFromMappedFile(mapped);
        }
    }
    m_mappedHiddenLayers.clear();

    // History snapshots — rehydrate layer images back into each snapshot.
    std::vector<DocumentSnapshot> &states = m_history.mutableStates();
    for (const auto &entry : m_mappedSnapshots) {
        const int historyIndex = entry.first;
        if (historyIndex < 0 || historyIndex >= static_cast<int>(states.size())) {
            continue;
        }
        DocumentSnapshot &snap = states[historyIndex];
        const int layerCount = std::min(static_cast<int>(snap.layers.size()),
                                        static_cast<int>(entry.second.size()));
        for (int j = 0; j < layerCount; ++j) {
            snap.layers[j].image = readImageFromMappedFile(entry.second[j]);
        }
    }
    m_mappedSnapshots.clear();

    // Selection and patch stacks.
    restoreSelectionAndPatches();

    // Close and delete temp files. Setting AutoRemove before destruction is
    // belt-and-suspenders; the destructor also cleans up.
    for (QFile *file : m_idleTempFiles) {
        delete file;
    }
    m_idleTempFiles.clear();

    m_resourcesReleased = false;
    setUpdatesEnabled(true);
    update();
    emit historyChanged();
    emit layersChanged();
}

bool CanvasWidget::isDrawingTool() const
{
    // Matches the predicate in mousePressEvent that gates patch-history
    // recording. Keep these two in sync if you add a new drawing tool.
    return m_currentToolType == Pen
        || m_currentToolType == Eraser
        || m_currentToolType == Line;
}

void CanvasWidget::enterEvent(QEnterEvent *event)
{
    QOpenGLWidget::enterEvent(event);
    m_mouseInside = true;
    m_lastMousePos = event->position();
    if (isDrawingTool() && !m_panning && !m_strokeInProgress) {
        update();
    }
}

void CanvasWidget::leaveEvent(QEvent *event)
{
    QOpenGLWidget::leaveEvent(event);
    if (!m_mouseInside) {
        return;
    }
    m_mouseInside = false;
    // Always repaint on leave so the indicator clears, even if a tool
    // switch had already hidden it via isDrawingTool() == false.
    update();
}

void CanvasWidget::drawBrushSizeIndicator(QPainter *painter) const
{
    if (!m_mouseInside || m_panning || m_strokeInProgress
        || !isDrawingTool() || m_layers.empty()) {
        return;
    }

    // Switch to widget coordinates so the outline stays 1 px wide at every
    // zoom level and the on-screen diameter matches brushSize * zoomLevel.
    painter->save();
    painter->setTransform(QTransform());

    const double radius = (m_brushSize / 2.0) * m_zoomLevel;
    const QPointF c = m_lastMousePos;
    const QRectF ring(c.x() - radius, c.y() - radius, radius * 2, radius * 2);

    // Adaptive contrast: a dark stroke and a 1-px-outset light halo, so the
    // ring stays readable over both light and dark canvas content.
    QPen darkPen(QColor(0, 0, 0, 220));
    darkPen.setWidthF(1.0);
    QPen lightPen(QColor(255, 255, 255, 220));
    lightPen.setWidthF(1.0);

    painter->setPen(darkPen);
    painter->setBrush(Qt::NoBrush);
    painter->drawEllipse(ring);

    painter->setPen(lightPen);
    painter->drawEllipse(ring.adjusted(-1, -1, 1, 1));

    // Center dot marks the click hotspot. Filled dark, with a light halo.
    const QRectF dot(c.x() - 2, c.y() - 2, 4, 4);
    painter->setPen(darkPen);
    painter->setBrush(QColor(0, 0, 0, 220));
    painter->drawEllipse(dot);

    painter->setPen(lightPen);
    painter->setBrush(Qt::NoBrush);
    painter->drawEllipse(dot.adjusted(-1, -1, 1, 1));

    painter->restore();
}
