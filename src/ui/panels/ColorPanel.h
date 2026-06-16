#pragma once

#include <QDockWidget>
#include <QColor>

class QSlider;
class QSpinBox;
class QLineEdit;
class QLabel;

class ColorPanel : public QDockWidget {
    Q_OBJECT
public:
    explicit ColorPanel(QWidget *parent = nullptr);

public slots:
    void setColor(const QColor &color);

signals:
    void colorChanged(const QColor &color);

private slots:
    void onSliderChanged();
    void onHexChanged();

private:
    void updateUIFromColor();
    
    QColor m_currentColor;
    bool m_isUpdatingUI;

    QLabel *m_colorPreview;
    
    QSlider *m_rSlider;
    QSpinBox *m_rSpinBox;
    
    QSlider *m_gSlider;
    QSpinBox *m_gSpinBox;
    
    QSlider *m_bSlider;
    QSpinBox *m_bSpinBox;
    
    QLineEdit *m_hexInput;
};
