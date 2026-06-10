#pragma once

#include <QOpenGLWidget>
#include <QImage>
#include <QPoint>
#include <QColor>
#include <memory>
#include <vector>
#include "../models/Layer.h"

class Tool;

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
        Line
    };

    void clearCanvas();
    bool saveImage(const QString &fileName);

    void setTool(ToolType type);
    void setBrushColor(const QColor &color);
    void setBrushSize(int size);

    QColor brushColor() const { return m_brushColor; }
    int brushSize() const { return m_brushSize; }

    // Layer Management
    void addLayer(const QString &name);
    void removeLayer(int index);
    void setActiveLayer(int index);
    void setLayerVisible(int index, bool visible);
    
    int getLayerCount() const;
    QString getLayerName(int index) const;
    int getActiveLayerIndex() const;
    
    // Canvas Management
    void setCanvasSize(int width, int height);
    QImage getFlattenedImage() const;

signals:
    void layersChanged();
    void brushColorChanged(const QColor &color);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    void drawCheckerboard(QPainter &painter, const QRect &rect);

    std::vector<Layer> m_layers;
    int m_activeLayerIndex;
    
    QSize m_canvasSize;
    QColor m_brushColor;
    int m_brushSize;
    
    std::unique_ptr<Tool> m_activeTool;
};
