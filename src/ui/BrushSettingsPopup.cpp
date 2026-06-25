#include "BrushSettingsPopup.h"
#include "../canvas/CanvasWidget.h"

#include <QColorDialog>
#include <QEvent>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QSignalBlocker>
#include <QSlider>
#include <QSpinBox>
#include <QVBoxLayout>

namespace {
// ToolOptionsBar uses 1..200 for Pen/Eraser; match it here.
constexpr int kSizeMin = 1;
constexpr int kSizeMax = 200;

// Opacity is shown as 0..100% in the UI but stored as 0..255 internally
// on CanvasWidget. Keep both ends consistent.
constexpr int kOpacityPercentMin = 0;
constexpr int kOpacityPercentMax = 100;
}

BrushSettingsPopup::BrushSettingsPopup(CanvasWidget *canvas, QWidget *parent)
    : QWidget(parent, Qt::Popup)
    , m_canvas(canvas)
{
    setAttribute(Qt::WA_DeleteOnClose, false); // we own the lifetime in CanvasWidget
    setFocusPolicy(Qt::StrongFocus);

    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(10, 8, 10, 8);
    root->setSpacing(6);

    auto *title = new QLabel(tr("Brush Settings"));
    QFont titleFont = title->font();
    titleFont.setBold(true);
    title->setFont(titleFont);
    root->addWidget(title);

    // ---- Size row: slider | spin | "px" label ----
    auto *sizeLabel = new QLabel(tr("Size:"));
    m_sizeSlider = new QSlider(Qt::Horizontal);
    m_sizeSlider->setRange(kSizeMin, kSizeMax);
    m_sizeSlider->setValue(m_canvas->brushSize());

    m_sizeSpin = new QSpinBox();
    m_sizeSpin->setRange(kSizeMin, kSizeMax);
    m_sizeSpin->setValue(m_canvas->brushSize());

    auto *pxLabel = new QLabel(tr("px"));

    auto *sizeRow = new QHBoxLayout();
    sizeRow->addWidget(sizeLabel);
    sizeRow->addWidget(m_sizeSlider, 1);
    sizeRow->addWidget(m_sizeSpin);
    sizeRow->addWidget(pxLabel);
    root->addLayout(sizeRow);

    // ---- Opacity row: label | slider | spin | "%" label ----
    auto *opacityLabel = new QLabel(tr("Opacity:"));
    m_opacitySlider = new QSlider(Qt::Horizontal);
    m_opacitySlider->setRange(kOpacityPercentMin, kOpacityPercentMax);
    m_opacitySlider->setValue((m_canvas->brushOpacity() * kOpacityPercentMax + 127) / 255);

    m_opacitySpin = new QSpinBox();
    m_opacitySpin->setRange(kOpacityPercentMin, kOpacityPercentMax);
    m_opacitySpin->setValue((m_canvas->brushOpacity() * kOpacityPercentMax + 127) / 255);

    auto *pctLabel = new QLabel(tr("%"));

    auto *opacityRow = new QHBoxLayout();
    opacityRow->addWidget(opacityLabel);
    opacityRow->addWidget(m_opacitySlider, 1);
    opacityRow->addWidget(m_opacitySpin);
    opacityRow->addWidget(pctLabel);
    root->addLayout(opacityRow);

    // ---- Color row: label | swatch ----
    auto *colorLabel = new QLabel(tr("Color:"));
    m_swatch = new QLabel();
    m_swatch->setFixedSize(48, 20);
    m_swatch->setFrameShape(QFrame::Panel);
    m_swatch->setFrameShadow(QFrame::Sunken);
    updateSwatch(m_canvas->brushColor());

    auto *colorRow = new QHBoxLayout();
    colorRow->addWidget(colorLabel);
    colorRow->addWidget(m_swatch);
    colorRow->addStretch(1);
    root->addLayout(colorRow);

    // ---- Wiring ----
    connect(m_sizeSlider, &QSlider::valueChanged, m_sizeSpin, &QSpinBox::setValue);
    connect(m_sizeSpin, qOverload<int>(&QSpinBox::valueChanged), m_sizeSlider, &QSlider::setValue);
    connect(m_sizeSpin, qOverload<int>(&QSpinBox::valueChanged), this, &BrushSettingsPopup::onSizeChanged);

    connect(m_opacitySlider, &QSlider::valueChanged, m_opacitySpin, &QSpinBox::setValue);
    connect(m_opacitySpin, qOverload<int>(&QSpinBox::valueChanged), m_opacitySlider, &QSlider::setValue);
    connect(m_opacitySpin, qOverload<int>(&QSpinBox::valueChanged), this, &BrushSettingsPopup::onOpacityChanged);

    // QLabel doesn't expose a clicked signal directly; install an event
    // filter so left-click on the swatch opens the color dialog.
    m_swatch->installEventFilter(this);

    // Mirror changes originating elsewhere (the wheel shortcut, the bar)
    // back into the popup so it stays consistent.
    connect(m_canvas, &CanvasWidget::brushSizeChanged,
            this, &BrushSettingsPopup::onCanvasBrushSizeChanged);
    connect(m_canvas, &CanvasWidget::brushOpacityChanged,
            this, &BrushSettingsPopup::onCanvasBrushOpacityChanged);
    connect(m_canvas, &CanvasWidget::brushColorChanged,
            this, &BrushSettingsPopup::onCanvasBrushColorChanged);
}

void BrushSettingsPopup::showAt(const QPoint &globalPos)
{
    // Position so the cursor lands near the top-left of the popup, then
    // show. Qt::Popup means clicking outside or pressing Escape closes us.
    adjustSize();
    move(globalPos);
    show();
    raise();
    activateWindow();
    setFocus();
}

void BrushSettingsPopup::onSizeChanged(int size)
{
    // Slider/spin already mirror each other; just push to the canvas.
    m_canvas->setBrushSize(size);
}

void BrushSettingsPopup::onOpacityChanged(int percent)
{
    // UI is in 0..100, canvas stores 0..255.
    const int opacity = (percent * 255) / kOpacityPercentMax;
    m_canvas->setBrushOpacity(opacity);
}

void BrushSettingsPopup::onPickColor()
{
    const QColor chosen = QColorDialog::getColor(m_canvas->brushColor(), this);
    if (chosen.isValid()) {
        m_canvas->setBrushColor(chosen);
    }
}

void BrushSettingsPopup::onCanvasBrushSizeChanged(int size)
{
    QSignalBlocker b1(m_sizeSlider);
    QSignalBlocker b2(m_sizeSpin);
    m_sizeSlider->setValue(size);
    m_sizeSpin->setValue(size);
}

void BrushSettingsPopup::onCanvasBrushOpacityChanged(int opacity)
{
    const int percent = (opacity * kOpacityPercentMax + 127) / 255;
    QSignalBlocker b1(m_opacitySlider);
    QSignalBlocker b2(m_opacitySpin);
    m_opacitySlider->setValue(percent);
    m_opacitySpin->setValue(percent);
}

void BrushSettingsPopup::onCanvasBrushColorChanged(const QColor &color)
{
    updateSwatch(color);
}

void BrushSettingsPopup::updateSwatch(const QColor &color)
{
    QString style = QStringLiteral("background-color: %1; border: 1px solid #444;").arg(color.name());
    m_swatch->setStyleSheet(style);
    m_swatch->setToolTip(tr("Click to pick a color"));
}

bool BrushSettingsPopup::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_swatch && event->type() == QEvent::MouseButtonRelease) {
        const auto *mouse = static_cast<QMouseEvent *>(event);
        if (mouse->button() == Qt::LeftButton) {
            onPickColor();
            return true;
        }
    }
    return QWidget::eventFilter(watched, event);
}