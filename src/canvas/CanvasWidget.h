#pragma once

#include <QOpenGLWidget>
#include <QImage>
#include <QPoint>
#include <QColor>
#include <QFile>
#include <memory>
#include <vector>
#include "../models/Layer.h"
#include "../history/HistoryManager.h"

class Tool;
class BrushSettingsPopup;
class TextTool;

class CanvasWidget : public QOpenGLWidget {
    Q_OBJECT

public:
    explicit CanvasWidget(QWidget *parent = nullptr);
    ~CanvasWidget();

    enum ToolType {
        Pen,
        Eraser,
        Fill,
        Eyedropper,
        Line,
        Move,
        RectSelect,
        EllipseSelect,
        LassoSelect,
        Text,
        Zoom,
        Pan
    };

    void clearCanvas();
    bool saveImage(const QString &fileName);
    bool exportImage(const QString &fileName);
    bool openImage(const QString &fileName);
    bool saveProject(const QString &fileName);
    bool openProject(const QString &fileName);

    void setTool(ToolType type);
    void setBrushColor(const QColor &color);
    void setBrushSize(int size);
    void setBrushOpacity(int opacity);
    void setFillTolerance(int tolerance);
    void setTextFontFamily(const QString &family);
    void setTextFontSize(int size);

    QColor brushColor() const { return m_brushColor; }
    int brushSize() const { return m_brushSize; }
    int brushOpacity() const { return m_brushOpacity; }
    int fillTolerance() const { return m_fillTolerance; }
    QString textFontFamily() const { return m_textFontFamily; }
    int textFontSize() const { return m_textFontSize; }

    TextTool *textTool() const;

    // Layer Management
    void addLayer(const QString &name);
    void removeLayer(int index);
    void duplicateLayer(int index);
    void moveLayer(int fromIndex, int toIndex);
    void renameLayer(int index, const QString &name);
    void setActiveLayer(int index);
    void setLayerVisible(int index, bool visible);
    void setLayerOpacity(int index, float opacity);
    void setLayerBlendMode(int index, BlendMode mode);
    void flattenAllLayers();

    int getLayerCount() const;
    QString getLayerName(int index) const;
    int getActiveLayerIndex() const;
    bool getLayerVisible(int index) const;
    float getLayerOpacity(int index) const;
    BlendMode getLayerBlendMode(int index) const;

    // Canvas Management
    void newCanvas(int width, int height);
    void setCanvasSize(int width, int height);
    void resizeCanvas(int width, int height);
    void rotateCanvas(bool clockwise);
    void flipCanvas(bool horizontal);
    void cropToSelection();

    QImage getFlattenedImage() const;
    QSize canvasSize() const { return m_canvasSize; }
    void updateCanvasRect(const QRect &canvasRect);
    void recordActiveLayerHistoryRegion(const QRect &canvasRect);

    // Selection
    void setSelectionMask(const QImage &mask, const QRect &bounds);
    void clearSelection();
    bool hasSelection() const;
    QImage selectionMask() const { return m_selectionMask; }
    QRect selectionBounds() const { return m_selectionBounds; }

    // Filters
    void applyBlurFilter(int radius = 2);
    void applySharpenFilter(float amount = 1.0f);

    // History
    void pushHistoryState(const QString &description);
    void undo();
    void redo();
    bool canUndo() const;
    bool canRedo() const;
    int currentHistoryIndex() const { return m_history.currentIndex(); }
    int patchUndoDepth() const { return static_cast<int>(m_patchUndoStack.size()); }
    int patchRedoDepth() const { return static_cast<int>(m_patchRedoStack.size()); }

    // Zoom
    void zoomIn();
    void zoomOut();
    void zoomInAt(const QPointF &widgetPos);
    void zoomOutAt(const QPointF &widgetPos);
    void fitToWindow();
    void actualSize();
    double zoomLevel() const { return m_zoomLevel; }

    // Low Memory Mode — when enabled, the canvas swaps hidden layer images
    // and history snapshots out to memory-mapped temp files while the window
    // is inactive, freeing RAM at the cost of one mmap round-trip per swap.
    // No-op while a stroke or selection is in progress.
    void setLowMemoryMode(bool enabled);
    bool isLowMemoryMode() const { return m_lowMemoryMode; }
    bool isResourcesReleased() const { return m_resourcesReleased; }

    bool isBusy() const;

    void releaseIdleResources();
    void restoreIdleResources();
    void handleEnterKey();

signals:
    void layersChanged();
    void brushColorChanged(const QColor &color);
    void brushSizeChanged(int size);
    void brushOpacityChanged(int opacity);
    void zoomChanged(double level);
    void toolChanged(ToolType type);
    void historyChanged();
    void lowMemoryModeChanged(bool enabled);

    // Emitted when the document content actually changes (coarse edit, patch
    // commit, undo/redo, project load). Distinct from historyChanged, which
    // also fires on low-memory idle release/restore — those are not user edits
    // and must not flip the dirty flag in MainWindow.
    void documentEdited();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    void drawCheckerboard(QPainter &painter, const QRect &rect);
    void drawSelectionOverlay(QPainter &painter);
    QImage blendLayers() const;
    QPointF widgetToCanvas(const QPointF &widgetPos) const;
    void applyZoomAt(const QPointF &widgetPos, double factor);
    void restoreFromSnapshot(const DocumentSnapshot &snapshot);
    DocumentSnapshot captureSnapshot(const QString &description) const;
    void initHistory(const QString &description);
    void beginPatchHistory(const QString &description);
    void commitPatchHistory();
    void clearPatchHistory();
    QRect effectiveFilterRegion() const;
    void applyFilterToActiveLayer(const QImage &filtered, const QRect &region);
    bool isPanActive(QMouseEvent *event) const;

    // Low-memory idle swap helpers. A MappedImage records everything needed
    // to recreate a QImage that was written to a QTemporaryFile while idle.
    // The accompanying QFile* is held in m_idleTempFiles for the duration of
    // the release so the kernel can evict the pages under pressure.
    struct MappedImage {
        QSize size;
        QImage::Format format;
        QString path;
        int bytesPerLine = 0;
        int layerIndex = -1; // for hidden-layer mapping; -1 for snapshot layers
    };

    static QImage readImageFromMappedFile(const MappedImage &mapped);

    void mapLayerOut(int layerIndex);
    void mapSnapshotOut(int historyIndex);
    void releaseSelectionAndPatches();
    void restoreSelectionAndPatches();

    std::vector<Layer> m_layers;
    int m_activeLayerIndex;

    QSize m_canvasSize;
    QColor m_brushColor;
    int m_brushSize;
    int m_brushOpacity;
    int m_fillTolerance;
    QString m_textFontFamily;
    int m_textFontSize;

    QImage m_selectionMask;
    QRect m_selectionBounds;

    HistoryManager m_history;

    struct LayerPatch {
        int layerIndex = -1;
        QRect rect;
        QImage before;
        QImage after;
    };

    struct PatchHistoryEntry {
        QString description;
        std::vector<LayerPatch> patches;
    };

    std::vector<PatchHistoryEntry> m_patchUndoStack;
    std::vector<PatchHistoryEntry> m_patchRedoStack;
    PatchHistoryEntry m_pendingPatchHistory;
    bool m_recordingPatchHistory;

    // Low-memory idle state. m_lowMemoryMode is the user-visible toggle;
    // m_resourcesReleased flips on after a successful release/restore cycle.
    // m_idleTempFiles keeps QTemporaryFile instances alive between release
    // and restore so the OS can actually page the data out.
    bool m_lowMemoryMode = false;
    bool m_resourcesReleased = false;
    std::vector<QFile*> m_idleTempFiles;
    std::vector<MappedImage> m_mappedHiddenLayers;
    std::vector<std::pair<int, std::vector<MappedImage>>> m_mappedSnapshots;
    QImage m_savedSelectionMask;
    QRect m_savedSelectionBounds;
    std::vector<PatchHistoryEntry> m_savedPatchUndoStack;
    std::vector<PatchHistoryEntry> m_savedPatchRedoStack;

    double m_zoomLevel;
    QPointF m_panOffset;
    bool m_panning;
    QPointF m_panStartPos;
    QPointF m_panStartOffset;
    bool m_spacePanning;

    bool m_strokeInProgress;

    std::unique_ptr<Tool> m_activeTool;
    ToolType m_currentToolType;

    // Brush-size cursor indicator state. m_lastMousePos is in widget
    // coordinates (from QMouseEvent::position()); m_mouseInside is toggled
    // by enterEvent/leaveEvent so the indicator vanishes when the cursor
    // leaves the widget. mouseMoveEvent triggers a repaint only when the
    // indicator is actually visible AND the position changed, to keep
    // hover repaints cheap.
    QPointF m_lastMousePos;
    bool m_mouseInside = false;

    bool isDrawingTool() const;
    void drawBrushSizeIndicator(QPainter *painter) const;

    // Right-click brush settings popup. Created once in the constructor;
    // owned for the canvas's lifetime.
    BrushSettingsPopup *m_brushSettingsPopup = nullptr;
};
