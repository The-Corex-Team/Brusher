#pragma once

#include <QDockWidget>

class QListWidget;
class QPushButton;
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
    void onRowsMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row);
    void syncLayersFromCanvas();

private:
    void setupLayerItem(int row, const QString &name, bool visible);

    CanvasWidget *m_canvas;
    QListWidget *m_layerList;
    
    QComboBox *m_blendModeCombo;
    QSlider *m_opacitySlider;
    QSpinBox *m_opacitySpinBox;
    
    QToolButton *m_addBtn;
    QToolButton *m_removeBtn;
    QToolButton *m_duplicateBtn;
    
    bool m_isSyncing;
};
