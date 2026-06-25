#include "LayerPanel.h"
#include "../../canvas/CanvasWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMouseEvent>
#include <QDrag>
#include <QMimeData>
#include <QToolButton>
#include <QApplication>
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
    // We own drag-drop via an event filter (see eventFilter below). Disabling
    // QListWidget's InternalMove stops Qt from mutating the model mid-drag,
    // which previously caused re-entrancy crashes against our canvas layer
    // mutation in moveLayer().
    //
    // acceptDrops stays true so the QDrag we start in our event filter can
    // deliver a QDropEvent (translated to QEvent::Drop) to the list widget,
    // which our filter handles. DragDropMode = NoDragDrop prevents Qt's
    // built-in handler from also acting on it.
    m_layerList->setDragDropMode(QAbstractItemView::NoDragDrop);
    m_layerList->setSelectionMode(QAbstractItemView::SingleSelection);
    m_layerList->setAcceptDrops(true);
    m_layerList->setDragEnabled(false);
    m_layerList->setDropIndicatorShown(false);
    m_layerList->installEventFilter(this);
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

    // itemChanged is connected exactly once. Connecting it inside
    // syncLayersFromCanvas previously leaked a fresh lambda on every call,
    // because the disconnect(nullptr, nullptr) call doesn't match lambdas.
    m_itemChangedConnection = connect(m_layerList, &QListWidget::itemChanged, this,
        [this](QListWidgetItem *item) {
            if (m_isSyncing) return;
            const int row = m_layerList->row(item);
            if (row < 0) return;
            const bool isVisible = item->checkState() == Qt::Checked;

            if (isVisible) {
                item->setIcon(QIcon(":/src/icons/layer_visible.svg"));
            } else {
                item->setIcon(QIcon());
            }

            const int canvasIndex = m_layerList->count() - 1 - row;
            m_canvas->setLayerVisible(canvasIndex, isVisible);
            m_canvas->renameLayer(canvasIndex, item->text());
        });

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

void LayerPanel::onRowReorderRequested(int fromRow, int toRow)
{
    if (m_isSyncing) return;
    const int count = m_layerList->count();
    if (fromRow < 0 || fromRow >= count || toRow < 0 || toRow >= count || fromRow == toRow) {
        return;
    }

    const int fromCanvasIndex = count - 1 - fromRow;
    const int toCanvasIndex = count - 1 - toRow;

    // moveLayer() emits layersChanged, which calls syncLayersFromCanvas().
    // That's fine now because we're no longer inside a QListWidget drag
    // handler — we're inside a QDropEvent that we own.
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
    // Suppress itemChanged while we rebuild. The lambda connected in the ctor
    // checks m_isSyncing, but blocking is cheaper than re-checking for every
    // item's checkState change during clear().
    m_layerList->blockSignals(true);
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

    m_layerList->blockSignals(false);
    m_isSyncing = false;
}

int LayerPanel::rowAtDropPos(const QPoint &pos) const
{
    // indexAt() returns the row under pos, or an invalid index if pos is in
    // the empty area at the bottom of the list. In the empty-area case,
    // clamp to the last row — moving "below everything" means "put it at
    // the bottom of the stack" (canvas index 0).
    const QModelIndex idx = m_layerList->indexAt(pos);
    if (idx.isValid()) {
        return idx.row();
    }
    const int n = m_layerList->count();
    if (n == 0) return -1;
    return n - 1;
}

bool LayerPanel::eventFilter(QObject *watched, QEvent *event)
{
    if (watched != m_layerList) {
        return QDockWidget::eventFilter(watched, event);
    }

    switch (event->type()) {
    case QEvent::MouseButtonPress: {
        auto *me = static_cast<QMouseEvent *>(event);
        if (me->button() != Qt::LeftButton || m_isSyncing) {
            break;
        }
        const QModelIndex idx = m_layerList->indexAt(me->pos());
        if (!idx.isValid()) {
            break;
        }
        m_dragStartRow = idx.row();
        m_dragStartPos = me->pos();
        break;
    }
    case QEvent::MouseMove: {
        auto *me = static_cast<QMouseEvent *>(event);
        if (!(me->buttons() & Qt::LeftButton) || m_dragStartRow < 0) {
            break;
        }
        // Standard QDrag distance threshold (matches Qt's own QListWidget).
        if ((me->pos() - m_dragStartPos).manhattanLength()
            < QApplication::startDragDistance()) {
            break;
        }

        QListWidgetItem *item = m_layerList->item(m_dragStartRow);
        if (!item) {
            m_dragStartRow = -1;
            break;
        }

        auto *mime = new QMimeData;
        // Encode the source row in the mime data so a future cross-widget
        // drop could read it; for now we only support intra-list drops.
        mime->setData(QStringLiteral("application/x-brusher-layer-row"),
                      QByteArray::number(m_dragStartRow));

        auto *drag = new QDrag(m_layerList);
        drag->setMimeData(mime);
        // A simple pixmap of the dragged row's icon + name keeps the user
        // oriented during the drag without pulling in custom painting.
        QPixmap pm = item->icon().pixmap(m_layerList->iconSize());
        if (!pm.isNull()) {
            drag->setPixmap(pm);
            drag->setHotSpot(QPoint(pm.width() / 2, pm.height() / 2));
        }

        // Clear source state before exec(). On a successful drop the model
        // is rebuilt by syncLayersFromCanvas(); on cancel we want a clean
        // slate for the next press.
        m_dragStartRow = -1;

        // exec() runs its own event loop. The Drop event from this drag is
        // delivered to m_layerList as QEvent::Drop and handled below.
        drag->exec(Qt::MoveAction);
        return true;
    }
    case QEvent::DragEnter:
    case QEvent::DragMove: {
        auto *de = static_cast<QDragMoveEvent *>(event);
        if (de->mimeData()->hasFormat(QStringLiteral("application/x-brusher-layer-row"))) {
            de->acceptProposedAction();
            return true;
        }
        break;
    }
    case QEvent::Drop: {
        auto *de = static_cast<QDropEvent *>(event);
        const QByteArray srcBytes =
            de->mimeData()->data(QStringLiteral("application/x-brusher-layer-row"));
        if (srcBytes.isEmpty()) {
            break;
        }
        const int targetRow = rowAtDropPos(de->pos());
        if (targetRow < 0) {
            break;
        }
        bool ok = false;
        const int fromRow = srcBytes.toInt(&ok);
        if (!ok) {
            break;
        }
        onRowReorderRequested(fromRow, targetRow);
        de->acceptProposedAction();
        return true;
    }
    default:
        break;
    }

    return QDockWidget::eventFilter(watched, event);
}
