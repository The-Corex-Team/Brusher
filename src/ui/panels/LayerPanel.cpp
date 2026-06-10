#include "LayerPanel.h"
#include "../../canvas/CanvasWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QPushButton>

LayerPanel::LayerPanel(CanvasWidget *canvas, QWidget *parent)
    : QDockWidget(tr("Layers"), parent)
    , m_canvas(canvas)
    , m_isSyncing(false)
{
    QWidget *content = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(content);
    layout->setContentsMargins(2, 2, 2, 2);

    m_layerList = new QListWidget(content);
    layout->addWidget(m_layerList);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    m_addBtn = new QPushButton("+", content);
    m_removeBtn = new QPushButton("-", content);
    
    btnLayout->addWidget(m_addBtn);
    btnLayout->addWidget(m_removeBtn);
    layout->addLayout(btnLayout);

    content->setLayout(layout);
    setWidget(content);

    connect(m_addBtn, &QPushButton::clicked, this, &LayerPanel::onAddLayerClicked);
    connect(m_removeBtn, &QPushButton::clicked, this, &LayerPanel::onRemoveLayerClicked);
    connect(m_layerList, &QListWidget::currentRowChanged, this, &LayerPanel::onListSelectionChanged);
    
    connect(m_canvas, &CanvasWidget::layersChanged, this, &LayerPanel::syncLayersFromCanvas);

    // Initial sync
    syncLayersFromCanvas();
}

void LayerPanel::onAddLayerClicked()
{
    m_canvas->addLayer("Layer " + QString::number(m_layerList->count()));
}

void LayerPanel::onRemoveLayerClicked()
{
    int row = m_layerList->currentRow();
    if (row >= 0) {
        m_canvas->removeLayer(m_layerList->count() - 1 - row); // Invert index for bottom-up list
    }
}

void LayerPanel::onListSelectionChanged()
{
    if (m_isSyncing) return;
    
    int row = m_layerList->currentRow();
    if (row >= 0) {
        m_canvas->setActiveLayer(m_layerList->count() - 1 - row); // Invert index
    }
}

void LayerPanel::syncLayersFromCanvas()
{
    m_isSyncing = true;
    m_layerList->clear();
    
    int count = m_canvas->getLayerCount();
    for (int i = 0; i < count; ++i) {
        // Insert at beginning so index 0 (bottom) is at the bottom of the list
        m_layerList->insertItem(0, m_canvas->getLayerName(i));
    }
    
    int activeIndex = m_canvas->getActiveLayerIndex();
    if (activeIndex >= 0 && activeIndex < count) {
        // The list is inverted, so row = (count - 1) - activeIndex
        m_layerList->setCurrentRow(count - 1 - activeIndex);
    }
    
    m_isSyncing = false;
}
