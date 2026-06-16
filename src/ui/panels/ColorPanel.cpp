#include "ColorPanel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QSlider>
#include <QSpinBox>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QColorDialog>
#include <QRegularExpressionValidator>

ColorPanel::ColorPanel(QWidget *parent)
    : QDockWidget(tr("Color"), parent)
    , m_currentColor(Qt::black)
    , m_isUpdatingUI(false)
{
    setObjectName("ColorPanel");
    
    QWidget *content = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(content);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(10);

    // Color preview
    QHBoxLayout *topLayout = new QHBoxLayout();
    m_colorPreview = new QLabel();
    m_colorPreview->setFixedSize(60, 60);
    m_colorPreview->setStyleSheet("background-color: black; border: 1px solid #191919;");
    topLayout->addWidget(m_colorPreview);
    topLayout->addStretch();
    
    layout->addLayout(topLayout);

    // RGB Sliders
    QGridLayout *grid = new QGridLayout();
    grid->setSpacing(5);
    
    // Helper to create slider rows
    auto createSliderRow = [this, grid](int row, const QString &label, QSlider *&slider, QSpinBox *&spin) {
        grid->addWidget(new QLabel(label), row, 0);
        
        slider = new QSlider(Qt::Horizontal);
        slider->setRange(0, 255);
        grid->addWidget(slider, row, 1);
        
        spin = new QSpinBox();
        spin->setRange(0, 255);
        spin->setFixedWidth(50);
        spin->setButtonSymbols(QAbstractSpinBox::NoButtons);
        grid->addWidget(spin, row, 2);
        
        connect(slider, &QSlider::valueChanged, this, &ColorPanel::onSliderChanged);
        connect(spin, qOverload<int>(&QSpinBox::valueChanged), this, &ColorPanel::onSliderChanged);
    };
    
    createSliderRow(0, "R", m_rSlider, m_rSpinBox);
    createSliderRow(1, "G", m_gSlider, m_gSpinBox);
    createSliderRow(2, "B", m_bSlider, m_bSpinBox);
    
    layout->addLayout(grid);
    
    // Hex input
    QHBoxLayout *hexLayout = new QHBoxLayout();
    hexLayout->addWidget(new QLabel("#"));
    m_hexInput = new QLineEdit();
    m_hexInput->setMaxLength(6);
    m_hexInput->setFixedWidth(70);
    QRegularExpression hexRegex("[0-9A-Fa-f]{6}");
    m_hexInput->setValidator(new QRegularExpressionValidator(hexRegex, this));
    connect(m_hexInput, &QLineEdit::textEdited, this, &ColorPanel::onHexChanged);
    
    hexLayout->addWidget(m_hexInput);
    hexLayout->addStretch();
    layout->addLayout(hexLayout);

    QPushButton *pickBtn = new QPushButton(tr("Pick Color..."));
    connect(pickBtn, &QPushButton::clicked, this, [this]() {
        const QColor picked = QColorDialog::getColor(m_currentColor, this, tr("Pick Color"));
        if (picked.isValid()) {
            setColor(picked);
            emit colorChanged(m_currentColor);
        }
    });
    layout->addWidget(pickBtn);

    layout->addStretch();
    
    content->setLayout(layout);
    setWidget(content);
    
    updateUIFromColor();
}

void ColorPanel::setColor(const QColor &color)
{
    if (m_currentColor != color) {
        m_currentColor = color;
        updateUIFromColor();
    }
}

void ColorPanel::onSliderChanged()
{
    if (m_isUpdatingUI) return;
    
    QObject *senderObj = sender();
    
    // Sync slider <-> spinbox
    m_isUpdatingUI = true;
    if (senderObj == m_rSlider) m_rSpinBox->setValue(m_rSlider->value());
    else if (senderObj == m_rSpinBox) m_rSlider->setValue(m_rSpinBox->value());
    else if (senderObj == m_gSlider) m_gSpinBox->setValue(m_gSlider->value());
    else if (senderObj == m_gSpinBox) m_gSlider->setValue(m_gSpinBox->value());
    else if (senderObj == m_bSlider) m_bSpinBox->setValue(m_bSlider->value());
    else if (senderObj == m_bSpinBox) m_bSlider->setValue(m_bSpinBox->value());
    m_isUpdatingUI = false;

    m_currentColor = QColor(m_rSlider->value(), m_gSlider->value(), m_bSlider->value());
    
    m_isUpdatingUI = true;
    m_hexInput->setText(m_currentColor.name().mid(1).toUpper());
    m_colorPreview->setStyleSheet(QString("background-color: %1; border: 1px solid #191919;").arg(m_currentColor.name()));
    m_isUpdatingUI = false;
    
    emit colorChanged(m_currentColor);
}

void ColorPanel::onHexChanged()
{
    if (m_isUpdatingUI || m_hexInput->text().length() < 6) return;
    
    QColor newColor(QString("#") + m_hexInput->text());
    if (newColor.isValid()) {
        m_currentColor = newColor;
        updateUIFromColor();
        emit colorChanged(m_currentColor);
    }
}

void ColorPanel::updateUIFromColor()
{
    m_isUpdatingUI = true;
    
    m_rSlider->setValue(m_currentColor.red());
    m_rSpinBox->setValue(m_currentColor.red());
    
    m_gSlider->setValue(m_currentColor.green());
    m_gSpinBox->setValue(m_currentColor.green());
    
    m_bSlider->setValue(m_currentColor.blue());
    m_bSpinBox->setValue(m_currentColor.blue());
    
    m_hexInput->setText(m_currentColor.name().mid(1).toUpper());
    m_colorPreview->setStyleSheet(QString("background-color: %1; border: 1px solid #191919;").arg(m_currentColor.name()));
    
    m_isUpdatingUI = false;
}
