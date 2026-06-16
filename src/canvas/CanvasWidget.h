#pragma once

#include <QOpenGLWidget>
#include <QImage>
#include <QPoint>
#include <QColor>
#include <memory>
#include <vector>
#include "../models/Layer.h"
#include "../history/HistoryManager.h"

class Tool;
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

    // Zoom
    void zoomIn();
    void zoomOut();
    void zoomInAt(const QPointF &widgetPos);
    void zoomOutAt(const QPointF &widgetPos);
    void fitToWindow();
    void actualSize();
    double zoomLevel() const { return m_zoomLevel; }

signals:
    void layersChanged();
    void brushColorChanged(const QColor &color);
    void zoomChanged(double level);
    void toolChanged(ToolType type);
    void historyChanged();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    void drawCheckerboard(QPainter &painter, const QRect &rect);
    void drawSelectionOverlay(QPainter &painter);
    QImage blendLayers() const;
    QPointF widgetToCanvas(const QPointF &widgetPos) const;
    void applyZoomAt(const QPointF &widgetPos, double factor);
    void restoreFromSnapshot(const DocumentSnapshot &snapshot);
    DocumentSnapshot captureSnapshot(const QString &description) const;
    void initHistory(const QString &description);
    QRect effectiveFilterRegion() const;
    void applyFilterToActiveLayer(const QImage &filtered, const QRect &region);
    bool isPanActive(QMouseEvent *event) const;

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

    double m_zoomLevel;
    QPointF m_panOffset;
    bool m_panning;
    QPointF m_panStartPos;
    QPointF m_panStartOffset;
    bool m_spacePanning;

    bool m_strokeInProgress;

    std::unique_ptr<Tool> m_activeTool;
    ToolType m_currentToolType;
};
