#pragma once

#include <QDockWidget>

class QListWidget;
class QPushButton;
class CanvasWidget;

class LayerPanel : public QDockWidget {
    Q_OBJECT
public:
    explicit LayerPanel(CanvasWidget *canvas, QWidget *parent = nullptr);

private slots:
    void onAddLayerClicked();
    void onRemoveLayerClicked();
    void onListSelectionChanged();
    void syncLayersFromCanvas();

private:
    CanvasWidget *m_canvas;
    QListWidget *m_layerList;
    QPushButton *m_addBtn;
    QPushButton *m_removeBtn;
    
    bool m_isSyncing;
};
