#include "CanvasWidget.h"
#include "../tools/PenTool.h"
#include "../tools/EraserTool.h"
#include "../tools/FillTool.h"
#include "../tools/EyedropperTool.h"
#include "../tools/LineTool.h"
#include <QPainter>
#include <QMouseEvent>
#include <QPaintEvent>

CanvasWidget::CanvasWidget(QWidget *parent)
    : QOpenGLWidget(parent)
    , m_activeLayerIndex(0)
    , m_canvasSize(800, 600)
    , m_brushColor(Qt::black)
    , m_brushSize(5)
{
    // Removed WA_StaticContents for proper OpenGL buffer clearing
    
    Layer bgLayer;
    bgLayer.name = "Background";
    bgLayer.image = QImage(m_canvasSize, QImage::Format_ARGB32);
    bgLayer.image.fill(Qt::white);
    m_layers.push_back(bgLayer);
    
    setTool(Pen);
    setMinimumSize(m_canvasSize);
}

CanvasWidget::~CanvasWidget() = default;

void CanvasWidget::setCanvasSize(int width, int height)
{
    m_canvasSize = QSize(width, height);
    setMinimumSize(m_canvasSize);
    
    m_layers.clear();
    
    Layer bgLayer;
    bgLayer.name = "Background";
    bgLayer.image = QImage(m_canvasSize, QImage::Format_ARGB32);
    bgLayer.image.fill(Qt::white);
    m_layers.push_back(bgLayer);
    
    m_activeLayerIndex = 0;
    
    emit layersChanged();
    update();
}

void CanvasWidget::addLayer(const QString &name)
{
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
    if (index >= 0 && index < m_layers.size() && m_layers.size() > 1) {
        m_layers.erase(m_layers.begin() + index);
        if (m_activeLayerIndex >= m_layers.size()) {
            m_activeLayerIndex = m_layers.size() - 1;
        }
        emit layersChanged();
        update();
    }
}

void CanvasWidget::setActiveLayer(int index)
{
    if (index >= 0 && index < m_layers.size()) {
        m_activeLayerIndex = index;
    }
}

void CanvasWidget::setLayerVisible(int index, bool visible)
{
    if (index >= 0 && index < m_layers.size()) {
        m_layers[index].visible = visible;
        update();
    }
}

int CanvasWidget::getLayerCount() const { return m_layers.size(); }
QString CanvasWidget::getLayerName(int index) const { return index >= 0 && index < m_layers.size() ? m_layers[index].name : QString(); }
int CanvasWidget::getActiveLayerIndex() const { return m_activeLayerIndex; }

void CanvasWidget::setTool(ToolType type)
{
    if (type == Pen) m_activeTool = std::make_unique<PenTool>();
    else if (type == Eraser) m_activeTool = std::make_unique<EraserTool>();
    else if (type == Fill) m_activeTool = std::make_unique<FillTool>();
    else if (type == Eyedropper) m_activeTool = std::make_unique<EyedropperTool>();
    else if (type == Line) m_activeTool = std::make_unique<LineTool>();
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
    m_brushSize = size;
}

void CanvasWidget::clearCanvas()
{
    if (!m_layers.empty()) {
        m_layers[m_activeLayerIndex].image.fill(Qt::transparent);
        update();
    }
}

QImage CanvasWidget::getFlattenedImage() const
{
    QImage flat(m_canvasSize, QImage::Format_ARGB32);
    flat.fill(Qt::transparent);
    
    QPainter painter(&flat);
    for (const auto &layer : m_layers) {
        if (layer.visible) {
            painter.setOpacity(layer.opacity);
            painter.drawImage(0, 0, layer.image);
        }
    }
    return flat;
}

bool CanvasWidget::saveImage(const QString &fileName)
{
    return getFlattenedImage().save(fileName);
}

void CanvasWidget::drawCheckerboard(QPainter &painter, const QRect &rect)
{
    const int checkSize = 10;
    QColor color1(200, 200, 200);
    QColor color2(255, 255, 255);
    
    for (int y = rect.top(); y < rect.bottom(); y += checkSize) {
        for (int x = rect.left(); x < rect.right(); x += checkSize) {
            bool isEven = ((x / checkSize) + (y / checkSize)) % 2 == 0;
            painter.fillRect(x, y, checkSize, checkSize, isEven ? color1 : color2);
        }
    }
}

void CanvasWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QRect dirtyRect = event->rect();
    
    drawCheckerboard(painter, dirtyRect);
    
    QRect canvasRect(0, 0, m_canvasSize.width(), m_canvasSize.height());
    QRect intersectRect = dirtyRect.intersected(canvasRect);
    
    for (const auto &layer : m_layers) {
        if (layer.visible) {
            painter.setOpacity(layer.opacity);
            painter.drawImage(intersectRect, layer.image, intersectRect);
        }
    }
    
    if (m_activeTool) {
        m_activeTool->drawPreview(&painter, this);
    }
}

void CanvasWidget::mousePressEvent(QMouseEvent *event)
{
    if (m_activeTool && !m_layers.empty() && m_layers[m_activeLayerIndex].visible) {
        m_activeTool->mousePressEvent(event, this, &m_layers[m_activeLayerIndex].image);
    }
}

void CanvasWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_activeTool && !m_layers.empty() && m_layers[m_activeLayerIndex].visible) {
        m_activeTool->mouseMoveEvent(event, this, &m_layers[m_activeLayerIndex].image);
    }
}

void CanvasWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_activeTool && !m_layers.empty() && m_layers[m_activeLayerIndex].visible) {
        m_activeTool->mouseReleaseEvent(event, this, &m_layers[m_activeLayerIndex].image);
    }
}
