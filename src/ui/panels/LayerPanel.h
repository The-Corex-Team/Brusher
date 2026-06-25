#pragma once

#include <QDockWidget>
#include <QMetaObject>

class QListWidget;
class QListWidgetItem;
class QToolButton;
class QSlider;
class QComboBox;
class QSpinBox;
class CanvasWidget;

class LayerPanel : public QDockWidget {
    Q_OBJECT
public:
    explicit LayerPanel(CanvasWidget *canvas, QWidget *parent = nullptr);

private slots:
    void onAddLayerClicked();
    void onRemoveLayerClicked();
    void onDuplicateLayerClicked();
    void onListSelectionChanged();
    void onOpacityChanged(int value);
    void onBlendModeChanged(int index);
    void onVisibilityToggled(int row);
    void onRowReorderRequested(int fromRow, int toRow);
    void syncLayersFromCanvas();

protected:
    // Custom drag-drop on the layer list. We override the QListWidget by
    // installing an event filter on it (see LayerPanel ctor), so we don't
    // need to subclass.
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void setupLayerItem(int row, const QString &name, bool visible);
    int rowAtDropPos(const QPoint &pos) const;

    CanvasWidget *m_canvas;
    QListWidget *m_layerList;

    QComboBox *m_blendModeCombo;
    QSlider *m_opacitySlider;
    QSpinBox *m_opacitySpinBox;

    QToolButton *m_addBtn;
    QToolButton *m_removeBtn;
    QToolButton *m_duplicateBtn;

    // Custom drag-drop state. Source row captured on press, cleared on
    // release. Distance threshold avoids accidental drags on click.
    int m_dragStartRow = -1;
    QPoint m_dragStartPos;

    // Held across the panel lifetime; replaced exactly once in ctor.
    QMetaObject::Connection m_itemChangedConnection;

    bool m_isSyncing;
};
