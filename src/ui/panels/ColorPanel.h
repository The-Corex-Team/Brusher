#pragma once

#include <QDockWidget>
#include <QColor>

class QPushButton;
class QSlider;
class QLabel;

class ColorPanel : public QDockWidget {
    Q_OBJECT
public:
    explicit ColorPanel(QWidget *parent = nullptr);

public slots:
    void setColor(const QColor &color);

signals:
    void colorChanged(const QColor &color);
    void brushSizeChanged(int size);

private slots:
    void chooseColor();
    void onSizeChanged(int size);

private:
    void updateColorButtonBackground();

    QPushButton *m_colorBtn;
    QSlider *m_sizeSlider;
    QLabel *m_sizeLabel;
    QColor m_currentColor;
};
