#include "ColorPanel.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QColorDialog>

ColorPanel::ColorPanel(QWidget *parent)
    : QDockWidget(tr("Brush Options"), parent)
    , m_currentColor(Qt::black)
{
    QWidget *content = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(content);

    QLabel *colorLabel = new QLabel("Color:", content);
    m_colorBtn = new QPushButton(content);
    updateColorButtonBackground();

    QLabel *sizeTitle = new QLabel("Size:", content);
    m_sizeSlider = new QSlider(Qt::Horizontal, content);
    m_sizeSlider->setRange(1, 100);
    m_sizeSlider->setValue(5);
    
    m_sizeLabel = new QLabel("5 px", content);

    layout->addWidget(colorLabel);
    layout->addWidget(m_colorBtn);
    layout->addSpacing(10);
    layout->addWidget(sizeTitle);
    layout->addWidget(m_sizeSlider);
    layout->addWidget(m_sizeLabel);
    layout->addStretch();

    content->setLayout(layout);
    setWidget(content);

    connect(m_colorBtn, &QPushButton::clicked, this, &ColorPanel::chooseColor);
    connect(m_sizeSlider, &QSlider::valueChanged, this, &ColorPanel::onSizeChanged);
}

void ColorPanel::chooseColor()
{
    QColor color = QColorDialog::getColor(m_currentColor, this, "Choose Brush Color");
    if (color.isValid()) {
        m_currentColor = color;
        updateColorButtonBackground();
        emit colorChanged(m_currentColor);
    }
}

void ColorPanel::onSizeChanged(int size)
{
    m_sizeLabel->setText(QString::number(size) + " px");
    emit brushSizeChanged(size);
}

void ColorPanel::updateColorButtonBackground()
{
    m_colorBtn->setStyleSheet(QString("background-color: %1; border: 1px solid gray;").arg(m_currentColor.name()));
}

void ColorPanel::setColor(const QColor &color)
{
    if (m_currentColor != color) {
        m_currentColor = color;
        updateColorButtonBackground();
        // Do not emit colorChanged here to avoid feedback loops if Canvas triggered this
    }
}
