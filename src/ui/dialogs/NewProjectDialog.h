#pragma once

#include <QDialog>

class QSpinBox;

class NewProjectDialog : public QDialog {
    Q_OBJECT
public:
    explicit NewProjectDialog(QWidget *parent = nullptr);

    int getCanvasWidth() const;
    int getCanvasHeight() const;

private:
    QSpinBox *m_widthSpinBox;
    QSpinBox *m_heightSpinBox;
};
