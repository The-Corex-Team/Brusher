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
#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QPaintEvent>
#include <QTransform>
#include <QFileInfo>
#include <QtMath>
#include <algorithm>

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
        return;
    }

    if (!m_history.canUndo()) {
        return;
    }
    m_patchRedoStack.clear();
    m_history.undo();
    restoreFromSnapshot(m_history.currentState());
    emit historyChanged();
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
        return;
    }

    if (!m_history.canRedo()) {
        return;
    }
    clearPatchHistory();
    m_history.redo();
    restoreFromSnapshot(m_history.currentState());
    emit historyChanged();
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

void CanvasWidget::setBrushSize(int size) { m_brushSize = size; }
void CanvasWidget::setBrushOpacity(int opacity) { m_brushOpacity = std::clamp(opacity, 0, 255); }
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
