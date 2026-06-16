#include "ToolOptionsBar.h"
#include "../../canvas/CanvasWidget.h"
#include "../../tools/TextTool.h"
#include <QStackedWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include <QFontComboBox>

ToolOptionsBar::ToolOptionsBar(CanvasWidget *canvas, QWidget *parent)
    : QToolBar(parent), m_canvas(canvas)
{
    setObjectName("ToolOptionsBar");
    setMovable(false);
    setFloatable(false);

    m_stack = new QStackedWidget(this);
    setupPages();
    addWidget(m_stack);
}

void ToolOptionsBar::setupPages()
{
    m_stack->addWidget(createBrushOptions());       // 0: Pen
    m_stack->addWidget(createEraserOptions());      // 1: Eraser
    m_stack->addWidget(createFillOptions());      // 2: Fill
    m_stack->addWidget(createEyedropperOptions());// 3: Eyedropper
    m_stack->addWidget(createLineOptions());      // 4: Line
    m_stack->addWidget(createEmptyOptions("Move Tool: drag selected pixels")); // 5: Move
    m_stack->addWidget(createSelectOptions());    // 6: RectSelect
    m_stack->addWidget(createSelectOptions());    // 7: EllipseSelect
    m_stack->addWidget(createSelectOptions());    // 8: LassoSelect
    m_stack->addWidget(createTextOptions());      // 9: Text
    m_stack->addWidget(createEmptyOptions("Zoom Tool: click to zoom in, Alt+click to zoom out")); // 10
    m_stack->addWidget(createEmptyOptions("Hand Tool: drag to pan, or hold Space and drag")); // 11
}

void ToolOptionsBar::setToolType(int type)
{
    if (type >= 0 && type < m_stack->count()) {
        m_stack->setCurrentIndex(type);
    }
}

QWidget* ToolOptionsBar::createBrushOptions()
{
    QWidget *w = new QWidget();
    QHBoxLayout *l = new QHBoxLayout(w);
    l->setContentsMargins(10, 0, 10, 0);
    l->setSpacing(10);

    l->addWidget(new QLabel(tr("Size:")));

    m_brushSizeSlider = new QSlider(Qt::Horizontal);
    m_brushSizeSlider->setRange(1, 200);
    m_brushSizeSlider->setValue(m_canvas->brushSize());
    m_brushSizeSlider->setFixedWidth(100);

    m_brushSizeSpinBox = new QSpinBox();
    m_brushSizeSpinBox->setRange(1, 200);
    m_brushSizeSpinBox->setValue(m_canvas->brushSize());

    connect(m_brushSizeSlider, &QSlider::valueChanged, m_brushSizeSpinBox, &QSpinBox::setValue);
    connect(m_brushSizeSpinBox, qOverload<int>(&QSpinBox::valueChanged), m_brushSizeSlider, &QSlider::setValue);
    connect(m_brushSizeSpinBox, qOverload<int>(&QSpinBox::valueChanged), this, &ToolOptionsBar::onBrushSizeChanged);

    l->addWidget(m_brushSizeSlider);
    l->addWidget(m_brushSizeSpinBox);
    l->addWidget(new QLabel(tr("px")));

    l->addSpacing(20);
    l->addWidget(new QLabel(tr("Opacity:")));

    m_opacitySlider = new QSlider(Qt::Horizontal);
    m_opacitySlider->setRange(0, 100);
    m_opacitySlider->setValue(100);
    m_opacitySlider->setFixedWidth(100);

    m_opacitySpinBox = new QSpinBox();
    m_opacitySpinBox->setRange(0, 100);
    m_opacitySpinBox->setValue(100);

    connect(m_opacitySlider, &QSlider::valueChanged, m_opacitySpinBox, &QSpinBox::setValue);
    connect(m_opacitySpinBox, qOverload<int>(&QSpinBox::valueChanged), m_opacitySlider, &QSlider::setValue);
    connect(m_opacitySpinBox, qOverload<int>(&QSpinBox::valueChanged), this, &ToolOptionsBar::onBrushOpacityChanged);

    l->addWidget(m_opacitySlider);
    l->addWidget(m_opacitySpinBox);
    l->addWidget(new QLabel(tr("%")));
    l->addStretch();
    return w;
}

QWidget* ToolOptionsBar::createEraserOptions()
{
    QWidget *w = new QWidget();
    QHBoxLayout *l = new QHBoxLayout(w);
    l->setContentsMargins(10, 0, 10, 0);
    l->setSpacing(10);

    l->addWidget(new QLabel(tr("Size:")));

    QSlider *sizeSlider = new QSlider(Qt::Horizontal);
    sizeSlider->setRange(1, 200);
    sizeSlider->setValue(m_canvas->brushSize());
    sizeSlider->setFixedWidth(100);

    QSpinBox *sizeSpinBox = new QSpinBox();
    sizeSpinBox->setRange(1, 200);
    sizeSpinBox->setValue(m_canvas->brushSize());

    connect(sizeSlider, &QSlider::valueChanged, sizeSpinBox, &QSpinBox::setValue);
    connect(sizeSpinBox, qOverload<int>(&QSpinBox::valueChanged), sizeSlider, &QSlider::setValue);
    connect(sizeSpinBox, qOverload<int>(&QSpinBox::valueChanged), this, &ToolOptionsBar::onBrushSizeChanged);

    l->addWidget(sizeSlider);
    l->addWidget(sizeSpinBox);
    l->addWidget(new QLabel(tr("px")));
    l->addStretch();
    return w;
}

QWidget* ToolOptionsBar::createFillOptions()
{
    QWidget *w = new QWidget();
    QHBoxLayout *l = new QHBoxLayout(w);
    l->setContentsMargins(10, 0, 10, 0);
    l->setSpacing(10);

    l->addWidget(new QLabel(tr("Tolerance:")));
    m_toleranceSpinBox = new QSpinBox();
    m_toleranceSpinBox->setRange(0, 255);
    m_toleranceSpinBox->setValue(32);
    connect(m_toleranceSpinBox, qOverload<int>(&QSpinBox::valueChanged), this, &ToolOptionsBar::onToleranceChanged);

    l->addWidget(m_toleranceSpinBox);
    l->addStretch();
    return w;
}

QWidget* ToolOptionsBar::createLineOptions()
{
    QWidget *w = new QWidget();
    QHBoxLayout *l = new QHBoxLayout(w);
    l->setContentsMargins(10, 0, 10, 0);
    l->setSpacing(10);

    l->addWidget(new QLabel(tr("Weight:")));
    QSpinBox *weightSpinBox = new QSpinBox();
    weightSpinBox->setRange(1, 100);
    weightSpinBox->setValue(m_canvas->brushSize());
    connect(weightSpinBox, qOverload<int>(&QSpinBox::valueChanged), this, &ToolOptionsBar::onBrushSizeChanged);

    l->addWidget(weightSpinBox);
    l->addWidget(new QLabel(tr("px")));
    l->addStretch();
    return w;
}

QWidget* ToolOptionsBar::createEyedropperOptions()
{
    QWidget *w = new QWidget();
    QHBoxLayout *l = new QHBoxLayout(w);
    l->setContentsMargins(10, 0, 10, 0);
    l->setSpacing(10);

    l->addWidget(new QLabel(tr("Sample Size:")));
    QComboBox *sampleCombo = new QComboBox();
    sampleCombo->addItems({"Point Sample", "3 by 3 Average", "5 by 5 Average"});
    l->addWidget(sampleCombo);
    l->addStretch();
    return w;
}

QWidget* ToolOptionsBar::createSelectOptions()
{
    QWidget *w = new QWidget();
    QHBoxLayout *l = new QHBoxLayout(w);
    l->setContentsMargins(10, 0, 10, 0);
    l->addWidget(new QLabel(tr("Drag to create selection. Use Move tool to reposition.")));
    l->addStretch();
    return w;
}

QWidget* ToolOptionsBar::createTextOptions()
{
    QWidget *w = new QWidget();
    QHBoxLayout *l = new QHBoxLayout(w);
    l->setContentsMargins(10, 0, 10, 0);
    l->setSpacing(10);

    l->addWidget(new QLabel(tr("Font:")));
    QFontComboBox *fontCombo = new QFontComboBox();
    connect(fontCombo, &QFontComboBox::currentFontChanged, this, [this](const QFont &font) {
        m_canvas->setTextFontFamily(font.family());
        if (TextTool *tool = m_canvas->textTool()) {
            tool->setFontFamily(font.family());
        }
    });
    l->addWidget(fontCombo);

    l->addWidget(new QLabel(tr("Size:")));
    QSpinBox *sizeSpin = new QSpinBox();
    sizeSpin->setRange(6, 200);
    sizeSpin->setValue(24);
    connect(sizeSpin, qOverload<int>(&QSpinBox::valueChanged), this, [this](int size) {
        m_canvas->setTextFontSize(size);
        if (TextTool *tool = m_canvas->textTool()) {
            tool->setFontSize(size);
        }
    });
    l->addWidget(sizeSpin);
    l->addStretch();
    return w;
}

QWidget* ToolOptionsBar::createEmptyOptions(const QString &message)
{
    QWidget *w = new QWidget();
    QHBoxLayout *l = new QHBoxLayout(w);
    l->setContentsMargins(10, 0, 10, 0);
    l->addWidget(new QLabel(message));
    l->addStretch();
    return w;
}

void ToolOptionsBar::onBrushSizeChanged(int size)
{
    m_canvas->setBrushSize(size);
}

void ToolOptionsBar::onBrushOpacityChanged(int opacity)
{
    m_canvas->setBrushOpacity((opacity * 255) / 100);
}

void ToolOptionsBar::onToleranceChanged(int tolerance)
{
    m_canvas->setFillTolerance(tolerance);
}
