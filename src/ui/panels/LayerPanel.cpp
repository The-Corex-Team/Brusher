#include "LayerPanel.h"
#include "../../canvas/CanvasWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QToolButton>
#include <QSlider>
#include <QSpinBox>
#include <QComboBox>
#include <QLabel>
#include <QIcon>
#include <QAbstractItemModel>
#include <QAbstractItemView>

LayerPanel::LayerPanel(CanvasWidget *canvas, QWidget *parent)
    : QDockWidget(tr("Layers"), parent)
    , m_canvas(canvas)
    , m_isSyncing(false)
{
    setObjectName("LayerPanel");
    
    QWidget *content = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(content);
    layout->setContentsMargins(4, 4, 4, 4);
    layout->setSpacing(4);

    // Blend mode and Opacity row
    QHBoxLayout *topLayout = new QHBoxLayout();
    
    m_blendModeCombo = new QComboBox();
    m_blendModeCombo->addItems({"Normal", "Multiply", "Screen", "Overlay", "Darken", "Lighten"});
    m_blendModeCombo->setFixedWidth(90);
    topLayout->addWidget(m_blendModeCombo);
    
    topLayout->addSpacing(5);
    
    topLayout->addWidget(new QLabel(tr("Opacity:")));
    
    m_opacitySlider = new QSlider(Qt::Horizontal);
    m_opacitySlider->setRange(0, 100);
    m_opacitySlider->setValue(100);
    
    m_opacitySpinBox = new QSpinBox();
    m_opacitySpinBox->setRange(0, 100);
    m_opacitySpinBox->setValue(100);
    m_opacitySpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
    m_opacitySpinBox->setFixedWidth(40);
    
    topLayout->addWidget(m_opacitySlider);
    topLayout->addWidget(m_opacitySpinBox);
    topLayout->addWidget(new QLabel(tr("%")));
    
    layout->addLayout(topLayout);

    // Layer List
    m_layerList = new QListWidget(content);
    m_layerList->setIconSize(QSize(20, 20));
    m_layerList->setDragDropMode(QAbstractItemView::InternalMove);
    m_layerList->setDefaultDropAction(Qt::MoveAction);
    m_layerList->setSelectionMode(QAbstractItemView::SingleSelection);
    m_layerList->setDragEnabled(true);
    m_layerList->setAcceptDrops(true);
    m_layerList->setDropIndicatorShown(true);
    layout->addWidget(m_layerList);

    // Bottom Action Buttons
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(2);
    
    m_duplicateBtn = new QToolButton(content);
    m_duplicateBtn->setIcon(QIcon(":/src/icons/layer_duplicate.svg"));
    m_duplicateBtn->setToolTip(tr("Duplicate Layer"));
    m_duplicateBtn->setFixedSize(24, 24);
    
    m_addBtn = new QToolButton(content);
    m_addBtn->setIcon(QIcon(":/src/icons/layer_new.svg"));
    m_addBtn->setToolTip(tr("New Layer"));
    m_addBtn->setFixedSize(24, 24);
    
    m_removeBtn = new QToolButton(content);
    m_removeBtn->setIcon(QIcon(":/src/icons/layer_delete.svg"));
    m_removeBtn->setToolTip(tr("Delete Layer"));
    m_removeBtn->setFixedSize(24, 24);
    
    btnLayout->addStretch();
    btnLayout->addWidget(m_duplicateBtn);
    btnLayout->addWidget(m_addBtn);
    btnLayout->addWidget(m_removeBtn);
    layout->addLayout(btnLayout);

    content->setLayout(layout);
    setWidget(content);

    // Connections
    connect(m_addBtn, &QToolButton::clicked, this, &LayerPanel::onAddLayerClicked);
    connect(m_removeBtn, &QToolButton::clicked, this, &LayerPanel::onRemoveLayerClicked);
    connect(m_duplicateBtn, &QToolButton::clicked, this, &LayerPanel::onDuplicateLayerClicked);
    connect(m_layerList, &QListWidget::currentRowChanged, this, &LayerPanel::onListSelectionChanged);
    connect(m_layerList->model(), &QAbstractItemModel::rowsMoved, this, &LayerPanel::onRowsMoved);
    
    connect(m_opacitySlider, &QSlider::valueChanged, m_opacitySpinBox, &QSpinBox::setValue);
    connect(m_opacitySpinBox, qOverload<int>(&QSpinBox::valueChanged), m_opacitySlider, &QSlider::setValue);
    connect(m_opacitySpinBox, qOverload<int>(&QSpinBox::valueChanged), this, &LayerPanel::onOpacityChanged);
    
    connect(m_blendModeCombo, qOverload<int>(&QComboBox::currentIndexChanged), this, &LayerPanel::onBlendModeChanged);
    
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

void LayerPanel::onDuplicateLayerClicked()
{
    int row = m_layerList->currentRow();
    if (row >= 0) {
        m_canvas->duplicateLayer(m_layerList->count() - 1 - row);
    }
}

void LayerPanel::onListSelectionChanged()
{
    if (m_isSyncing) return;
    
    int row = m_layerList->currentRow();
    if (row >= 0) {
        int canvasIndex = m_layerList->count() - 1 - row;
        m_canvas->setActiveLayer(canvasIndex);
        
        m_isSyncing = true;
        m_opacitySpinBox->setValue(m_canvas->getLayerOpacity(canvasIndex) * 100);
        m_blendModeCombo->setCurrentIndex(static_cast<int>(m_canvas->getLayerBlendMode(canvasIndex)));
        m_isSyncing = false;
    }
}

void LayerPanel::onOpacityChanged(int value)
{
    if (m_isSyncing) return;
    
    int row = m_layerList->currentRow();
    if (row >= 0) {
        m_canvas->setLayerOpacity(m_layerList->count() - 1 - row, value / 100.0f);
    }
}

void LayerPanel::onBlendModeChanged(int index)
{
    if (m_isSyncing) return;
    
    int row = m_layerList->currentRow();
    if (row >= 0) {
        m_canvas->setLayerBlendMode(m_layerList->count() - 1 - row, static_cast<BlendMode>(index));
    }
}

void LayerPanel::onVisibilityToggled(int row)
{
    if (m_isSyncing) return;
    
    QListWidgetItem *item = m_layerList->item(row);
    if (!item) return;
    
    // Toggle state
    bool wasVisible = item->checkState() == Qt::Checked;
    bool isVisible = !wasVisible;
    item->setCheckState(isVisible ? Qt::Checked : Qt::Unchecked);
    
    // We use a custom icon for unchecked to hide the checkmark, but list widget handles state
    if (isVisible) {
        item->setIcon(QIcon(":/src/icons/layer_visible.svg"));
    } else {
        item->setIcon(QIcon()); // Empty icon
    }
    
    m_canvas->setLayerVisible(m_layerList->count() - 1 - row, isVisible);
}

void LayerPanel::onRowsMoved(
    const QModelIndex &parent,
    int start,
    int end,
    const QModelIndex &destination,
    int row)
{
    Q_UNUSED(parent);
    Q_UNUSED(destination);

    if (m_isSyncing || start != end) {
        return;
    }

    const int count = m_layerList->count();
    const int finalRow = row > start ? row - 1 : row;
    if (finalRow < 0 || finalRow >= count || finalRow == start) {
        return;
    }

    const int fromCanvasIndex = count - 1 - start;
    const int toCanvasIndex = count - 1 - finalRow;
    m_canvas->moveLayer(fromCanvasIndex, toCanvasIndex);
}

void LayerPanel::setupLayerItem(int row, const QString &name, bool visible)
{
    QListWidgetItem *item = new QListWidgetItem(name);
    
    // Use checkstate for visibility
    item->setFlags(
        item->flags()
        | Qt::ItemIsUserCheckable
        | Qt::ItemIsEditable
        | Qt::ItemIsDragEnabled
        | Qt::ItemIsDropEnabled);
    item->setCheckState(visible ? Qt::Checked : Qt::Unchecked);
    
    if (visible) {
        item->setIcon(QIcon(":/src/icons/layer_visible.svg"));
    }
    
    m_layerList->insertItem(row, item);
}

void LayerPanel::syncLayersFromCanvas()
{
    m_isSyncing = true;
    m_layerList->clear();
    
    int count = m_canvas->getLayerCount();
    for (int i = 0; i < count; ++i) {
        // Insert at beginning so index 0 (bottom) is at the bottom of the list
        setupLayerItem(0, m_canvas->getLayerName(i), m_canvas->getLayerVisible(i));
    }
    
    int activeIndex = m_canvas->getActiveLayerIndex();
    if (activeIndex >= 0 && activeIndex < count) {
        // The list is inverted, so row = (count - 1) - activeIndex
        m_layerList->setCurrentRow(count - 1 - activeIndex);
        
        m_opacitySpinBox->setValue(m_canvas->getLayerOpacity(activeIndex) * 100);
        m_blendModeCombo->setCurrentIndex(static_cast<int>(m_canvas->getLayerBlendMode(activeIndex)));
    }
    
    // Hook up visibility toggle (handled by dataChanged in list widget)
    disconnect(m_layerList, &QListWidget::itemChanged, nullptr, nullptr);
    connect(m_layerList, &QListWidget::itemChanged, [this](QListWidgetItem *item) {
        if (m_isSyncing) return;
        int row = m_layerList->row(item);
        bool isVisible = item->checkState() == Qt::Checked;
        
        if (isVisible) {
            item->setIcon(QIcon(":/src/icons/layer_visible.svg"));
        } else {
            item->setIcon(QIcon());
        }
        
        const int canvasIndex = m_layerList->count() - 1 - row;
        m_canvas->setLayerVisible(canvasIndex, isVisible);
        m_canvas->renameLayer(canvasIndex, item->text());
    });
    
    m_isSyncing = false;
}
