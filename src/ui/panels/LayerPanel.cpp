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
    m_blendModeCombo->addItems({
        "Normal",
        "Multiply",
        "Screen",
        "Overlay",
        "Darken",
        "Lighten"
    });
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

    // We own drag-drop via an event filter.
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
    m_duplicateBtn->setIcon(QIcon(":/src/icons/layer_duplicate.png"));
    m_duplicateBtn->setToolTip(tr("Duplicate Layer"));
    m_duplicateBtn->setFixedSize(24, 24);

    m_addBtn = new QToolButton(content);
    m_addBtn->setIcon(QIcon(":/src/icons/layer_new.png"));
    m_addBtn->setToolTip(tr("New Layer"));
    m_addBtn->setFixedSize(24, 24);

    m_removeBtn = new QToolButton(content);
    m_removeBtn->setIcon(QIcon(":/src/icons/layer_delete.png"));
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
    connect(
        m_addBtn,
        &QToolButton::clicked,
        this,
        &LayerPanel::onAddLayerClicked
    );

    connect(
        m_removeBtn,
        &QToolButton::clicked,
        this,
        &LayerPanel::onRemoveLayerClicked
    );

    connect(
        m_duplicateBtn,
        &QToolButton::clicked,
        this,
        &LayerPanel::onDuplicateLayerClicked
    );

    connect(
        m_layerList,
        &QListWidget::currentRowChanged,
        this,
        &LayerPanel::onListSelectionChanged
    );

    // itemChanged is connected exactly once.
    m_itemChangedConnection = connect(
        m_layerList,
        &QListWidget::itemChanged,
        this,
        [this](QListWidgetItem *item) {
            if (m_isSyncing)
                return;

            const int row = m_layerList->row(item);
            if (row < 0)
                return;

            const bool isVisible =
                item->checkState() == Qt::Checked;

            if (isVisible) {
                item->setIcon(
                    QIcon(":/src/icons/layer_visible.png")
                );
            } else {
                item->setIcon(QIcon());
            }

            const int canvasIndex =
                m_layerList->count() - 1 - row;

            m_canvas->setLayerVisible(
                canvasIndex,
                isVisible
            );

            m_canvas->renameLayer(
                canvasIndex,
                item->text()
            );
        }
    );

    connect(
        m_opacitySlider,
        &QSlider::valueChanged,
        m_opacitySpinBox,
        &QSpinBox::setValue
    );

    connect(
        m_opacitySpinBox,
        qOverload<int>(&QSpinBox::valueChanged),
        m_opacitySlider,
        &QSlider::setValue
    );

    connect(
        m_opacitySpinBox,
        qOverload<int>(&QSpinBox::valueChanged),
        this,
        &LayerPanel::onOpacityChanged
    );

    connect(
        m_blendModeCombo,
        qOverload<int>(&QComboBox::currentIndexChanged),
        this,
        &LayerPanel::onBlendModeChanged
    );

    connect(
        m_canvas,
        &CanvasWidget::layersChanged,
        this,
        &LayerPanel::syncLayersFromCanvas
    );

    // Initial sync
    syncLayersFromCanvas();
}

void LayerPanel::onAddLayerClicked()
{
    m_canvas->addLayer(
        "Layer " + QString::number(m_layerList->count())
    );
}

void LayerPanel::onRemoveLayerClicked()
{
    int row = m_layerList->currentRow();

    if (row >= 0) {
        m_canvas->removeLayer(
            m_layerList->count() - 1 - row
        );
    }
}

void LayerPanel::onDuplicateLayerClicked()
{
    int row = m_layerList->currentRow();

    if (row >= 0) {
        m_canvas->duplicateLayer(
            m_layerList->count() - 1 - row
        );
    }
}

void LayerPanel::onListSelectionChanged()
{
    if (m_isSyncing)
        return;

    int row = m_layerList->currentRow();

    if (row >= 0) {
        int canvasIndex =
            m_layerList->count() - 1 - row;

        m_canvas->setActiveLayer(canvasIndex);

        m_isSyncing = true;

        m_opacitySpinBox->setValue(
            m_canvas->getLayerOpacity(canvasIndex) * 100
        );

        m_blendModeCombo->setCurrentIndex(
            static_cast<int>(
                m_canvas->getLayerBlendMode(canvasIndex)
            )
        );

        m_isSyncing = false;
    }
}

void LayerPanel::onOpacityChanged(int value)
{
    if (m_isSyncing)
        return;

    int row = m_layerList->currentRow();

    if (row >= 0) {
        m_canvas->setLayerOpacity(
            m_layerList->count() - 1 - row,
            value / 100.0f
        );
    }
}

void LayerPanel::onBlendModeChanged(int index)
{
    if (m_isSyncing)
        return;

    int row = m_layerList->currentRow();

    if (row >= 0) {
        m_canvas->setLayerBlendMode(
            m_layerList->count() - 1 - row,
            static_cast<BlendMode>(index)
        );
    }
}

void LayerPanel::onVisibilityToggled(int row)
{
    if (m_isSyncing)
        return;

    QListWidgetItem *item = m_layerList->item(row);

    if (!item)
        return;

    bool wasVisible =
        item->checkState() == Qt::Checked;

    bool isVisible = !wasVisible;

    item->setCheckState(
        isVisible
            ? Qt::Checked
            : Qt::Unchecked
    );

    if (isVisible) {
        item->setIcon(
            QIcon(":/src/icons/layer_visible.png")
        );
    } else {
        item->setIcon(QIcon());
    }

    m_canvas->setLayerVisible(
        m_layerList->count() - 1 - row,
        isVisible
    );
}

void LayerPanel::onRowReorderRequested(
    int fromRow,
    int toRow
)
{
    if (m_isSyncing)
        return;

    const int count = m_layerList->count();

    if (
        fromRow < 0 ||
        fromRow >= count ||
        toRow < 0 ||
        toRow >= count ||
        fromRow == toRow
    ) {
        return;
    }

    const int fromCanvasIndex =
        count - 1 - fromRow;

    const int toCanvasIndex =
        count - 1 - toRow;

    m_canvas->moveLayer(
        fromCanvasIndex,
        toCanvasIndex
    );
}

void LayerPanel::setupLayerItem(
    int row,
    const QString &name,
    bool visible
)
{
    QListWidgetItem *item =
        new QListWidgetItem(name);

    item->setFlags(
        item->flags()
        | Qt::ItemIsUserCheckable
        | Qt::ItemIsEditable
        | Qt::ItemIsDragEnabled
        | Qt::ItemIsDropEnabled
    );

    item->setCheckState(
        visible
            ? Qt::Checked
            : Qt::Unchecked
    );

    if (visible) {
        item->setIcon(
            QIcon(":/src/icons/layer_visible.png")
        );
    }

    m_layerList->insertItem(row, item);
}

void LayerPanel::syncLayersFromCanvas()
{
    m_isSyncing = true;

    m_layerList->blockSignals(true);
    m_layerList->clear();

    int count =
        m_canvas->getLayerCount();

    for (int i = 0; i < count; ++i) {
        setupLayerItem(
            0,
            m_canvas->getLayerName(i),
            m_canvas->getLayerVisible(i)
        );
    }

    int activeIndex =
        m_canvas->getActiveLayerIndex();

    if (
        activeIndex >= 0 &&
        activeIndex < count
    ) {
        m_layerList->setCurrentRow(
            count - 1 - activeIndex
        );

        m_opacitySpinBox->setValue(
            m_canvas->getLayerOpacity(activeIndex) * 100
        );

        m_blendModeCombo->setCurrentIndex(
            static_cast<int>(
                m_canvas->getLayerBlendMode(activeIndex)
            )
        );
    }

    m_layerList->blockSignals(false);
    m_isSyncing = false;
}

int LayerPanel::rowAtDropPos(const QPoint &pos) const
{
    const QModelIndex idx =
        m_layerList->indexAt(pos);

    if (idx.isValid()) {
        return idx.row();
    }

    const int n =
        m_layerList->count();

    if (n == 0)
        return -1;

    return n - 1;
}

bool LayerPanel::eventFilter(
    QObject *watched,
    QEvent *event
)
{
    if (watched != m_layerList) {
        return QDockWidget::eventFilter(
            watched,
            event
        );
    }

    switch (event->type()) {

    case QEvent::MouseButtonPress: {
        auto *me =
            static_cast<QMouseEvent *>(event);

        if (
            me->button() != Qt::LeftButton ||
            m_isSyncing
        ) {
            break;
        }

        const QModelIndex idx =
            m_layerList->indexAt(me->pos());

        if (!idx.isValid()) {
            break;
        }

        m_dragStartRow = idx.row();
        m_dragStartPos = me->pos();
        break;
    }

    case QEvent::MouseMove: {
        auto *me =
            static_cast<QMouseEvent *>(event);

        if (
            !(me->buttons() & Qt::LeftButton) ||
            m_dragStartRow < 0
        ) {
            break;
        }

        if (
            (me->pos() - m_dragStartPos)
                .manhattanLength()
            < QApplication::startDragDistance()
        ) {
            break;
        }

        QListWidgetItem *item =
            m_layerList->item(m_dragStartRow);

        if (!item) {
            m_dragStartRow = -1;
            break;
        }

        auto *mime = new QMimeData;

        mime->setData(
            QStringLiteral(
                "application/x-brusher-layer-row"
            ),
            QByteArray::number(
                m_dragStartRow
            )
        );

        auto *drag =
            new QDrag(m_layerList);

        drag->setMimeData(mime);

        QPixmap pm =
            item->icon().pixmap(
                m_layerList->iconSize()
            );

        if (!pm.isNull()) {
            drag->setPixmap(pm);

            drag->setHotSpot(
                QPoint(
                    pm.width() / 2,
                    pm.height() / 2
                )
            );
        }

        m_dragStartRow = -1;

        drag->exec(Qt::MoveAction);
        return true;
    }

    case QEvent::DragEnter:
    case QEvent::DragMove: {
        auto *de =
            static_cast<QDragMoveEvent *>(event);

        if (
            de->mimeData()->hasFormat(
                QStringLiteral(
                    "application/x-brusher-layer-row"
                )
            )
        ) {
            de->acceptProposedAction();
            return true;
        }

        break;
    }

    case QEvent::Drop: {
        auto *de =
            static_cast<QDropEvent *>(event);

        const QByteArray srcBytes =
            de->mimeData()->data(
                QStringLiteral(
                    "application/x-brusher-layer-row"
                )
            );

        if (srcBytes.isEmpty()) {
            break;
        }

        const int targetRow =
            rowAtDropPos(de->pos());

        if (targetRow < 0) {
            break;
        }

        bool ok = false;

        const int fromRow =
            srcBytes.toInt(&ok);

        if (!ok) {
            break;
        }

        onRowReorderRequested(
            fromRow,
            targetRow
        );

        de->acceptProposedAction();
        return true;
    }

    default:
        break;
    }

    return QDockWidget::eventFilter(
        watched,
        event
    );
}