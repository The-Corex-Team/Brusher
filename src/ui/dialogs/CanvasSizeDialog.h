#pragma once

#include <QDialog>

class QSpinBox;

class CanvasSizeDialog : public QDialog {
    Q_OBJECT
public:
    explicit CanvasSizeDialog(int currentWidth, int currentHeight, QWidget *parent = nullptr);

    int getCanvasWidth() const;
    int getCanvasHeight() const;

private:
    QSpinBox *m_widthSpinBox;
    QSpinBox *m_heightSpinBox;
};
