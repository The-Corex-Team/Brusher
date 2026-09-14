#include "ToolPanel.h"

#include <QVBoxLayout>
#include <QToolButton>
#include <QWidget>
#include <QFrame>
#include <QIcon>
#include <QLabel>
#include <QHBoxLayout>
#include <QSize>


ToolPanel::ToolPanel(QWidget *parent)
    : QDockWidget(tr("Tools"), parent)
{
    setObjectName("ToolBar");

    setFeatures(
        QDockWidget::DockWidgetFloatable |
        QDockWidget::DockWidgetMovable
    );

    setAllowedAreas(
        Qt::LeftDockWidgetArea |
        Qt::RightDockWidgetArea
    );

    QWidget *content = new QWidget(this);

    m_layout = new QVBoxLayout(content);
    m_layout->setContentsMargins(2, 4, 2, 4);
    m_layout->setSpacing(2);
    m_layout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);


    // ============================================================
    // Selection / Navigation Tools
    // ============================================================

    m_moveBtn = createToolButton(
        QIcon(":/icons/move.png"),
        "Move Tool (V)"
    );

    m_rectSelectBtn = createToolButton(
        QIcon(":/icons/rectselect.png"),
        "Rectangular Marquee Tool (M)"
    );

    m_ellipseSelectBtn = createToolButton(
        QIcon(":/icons/ellipseselect.png"),
        "Elliptical Marquee Tool"
    );

    m_lassoSelectBtn = createToolButton(
        QIcon(":/icons/lassoselect.png"),
        "Lasso Tool (L)"
    );


    // ============================================================
    // First Separator
    // ============================================================

    QFrame *sep1 = new QFrame(content);
    sep1->setFrameShape(QFrame::HLine);
    sep1->setFrameShadow(QFrame::Sunken);


    // ============================================================
    // Painting Tools
    // ============================================================

    m_penBtn = createToolButton(
        QIcon(":/icons/brush.png"),
        "Brush Tool (B)"
    );

    m_penBtn->setChecked(true);

    m_lineBtn = createToolButton(
        QIcon(":/icons/line.png"),
        "Line Tool (U)"
    );

    m_eraserBtn = createToolButton(
        QIcon(":/icons/eraser.png"),
        "Eraser Tool (E)"
    );

    m_fillBtn = createToolButton(
        QIcon(":/icons/fill.png"),
        "Paint Bucket Tool (G)"
    );


    // ============================================================
    // Second Separator
    // ============================================================

    QFrame *sep2 = new QFrame(content);
    sep2->setFrameShape(QFrame::HLine);
    sep2->setFrameShadow(QFrame::Sunken);


    // ============================================================
    // Other Tools
    // ============================================================

    m_textBtn = createToolButton(
        QIcon(":/icons/text.png"),
        "Horizontal Type Tool (T)"
    );

    m_eyedropperBtn = createToolButton(
        QIcon(":/icons/colorpicker.png"),
        "Eyedropper Tool (I)"
    );

    m_panBtn = createToolButton(
        QIcon(":/icons/pan.png"),
        "Hand Tool (H)"
    );

    m_zoomBtn = createToolButton(
        QIcon(":/icons/zoom.png"),
        "Zoom Tool (Z)"
    );


    // ============================================================
    // Add Widgets
    // ============================================================

    m_layout->addWidget(m_moveBtn);
    m_layout->addWidget(m_rectSelectBtn);
    m_layout->addWidget(m_ellipseSelectBtn);
    m_layout->addWidget(m_lassoSelectBtn);

    m_layout->addWidget(sep1);

    m_layout->addWidget(m_penBtn);
    m_layout->addWidget(m_lineBtn);
    m_layout->addWidget(m_eraserBtn);
    m_layout->addWidget(m_fillBtn);

    m_layout->addWidget(sep2);

    m_layout->addWidget(m_textBtn);
    m_layout->addWidget(m_eyedropperBtn);
    m_layout->addWidget(m_panBtn);
    m_layout->addWidget(m_zoomBtn);

    m_layout->addStretch();


    // ============================================================
    // Foreground / Background Colors
    // ============================================================

    setupForegroundBackground();


    content->setLayout(m_layout);
    setWidget(content);

    setFixedWidth(40);
}


QToolButton *ToolPanel::createToolButton(
    const QIcon &icon,
    const QString &tooltip)
{
    QToolButton *btn = new QToolButton(this);

    btn->setIcon(icon);
    btn->setIconSize(QSize(20, 20));
    btn->setToolTip(tooltip);

    btn->setCheckable(true);
    btn->setFixedSize(32, 32);

    connect(
        btn,
        &QToolButton::clicked,
        this,
        &ToolPanel::onToolClicked
    );

    return btn;
}


void ToolPanel::setupForegroundBackground()
{
    QWidget *colorWidget = new QWidget(this);

    colorWidget->setObjectName("ColorSwatchWidget");
    colorWidget->setFixedSize(32, 40);


    // Background color

    m_bgColorLabel = new QLabel(colorWidget);

    m_bgColorLabel->setGeometry(8, 12, 16, 16);

    m_bgColorLabel->setStyleSheet(
        "background-color: white;"
        "border: 1px solid #191919;"
    );


    // Foreground color

    m_fgColorLabel = new QLabel(colorWidget);

    m_fgColorLabel->setGeometry(2, 4, 16, 16);

    m_fgColorLabel->setStyleSheet(
        "background-color: black;"
        "border: 1px solid #191919;"
    );


    // Swap foreground/background colors

    m_swapColorsBtn = new QToolButton(colorWidget);

    m_swapColorsBtn->setIcon(
        QIcon(":/icons/swap.png")
    );

    m_swapColorsBtn->setIconSize(QSize(12, 12));

    m_swapColorsBtn->setGeometry(
        20, 0, 14, 14
    );

    m_swapColorsBtn->setStyleSheet(
        "border: none;"
        "background: transparent;"
    );

    connect(
        m_swapColorsBtn,
        &QToolButton::clicked,
        this,
        &ToolPanel::onSwapColorsClicked
    );


    m_layout->addWidget(colorWidget);
}


void ToolPanel::updateSwatches(
    const QColor &fg,
    const QColor &bg)
{
    m_fgColorLabel->setStyleSheet(
        QString(
            "background-color: %1;"
            "border: 1px solid #191919;"
        ).arg(fg.name())
    );

    m_bgColorLabel->setStyleSheet(
        QString(
            "background-color: %1;"
            "border: 1px solid #191919;"
        ).arg(bg.name())
    );
}


void ToolPanel::onToolClicked()
{
    QToolButton *clickedBtn =
        qobject_cast<QToolButton *>(sender());

    if (!clickedBtn) {
        return;
    }


    // Keep only the selected tool checked

    m_moveBtn->setChecked(clickedBtn == m_moveBtn);
    m_rectSelectBtn->setChecked(clickedBtn == m_rectSelectBtn);
    m_ellipseSelectBtn->setChecked(clickedBtn == m_ellipseSelectBtn);
    m_lassoSelectBtn->setChecked(clickedBtn == m_lassoSelectBtn);

    m_penBtn->setChecked(clickedBtn == m_penBtn);
    m_lineBtn->setChecked(clickedBtn == m_lineBtn);
    m_eraserBtn->setChecked(clickedBtn == m_eraserBtn);
    m_fillBtn->setChecked(clickedBtn == m_fillBtn);

    m_textBtn->setChecked(clickedBtn == m_textBtn);
    m_eyedropperBtn->setChecked(clickedBtn == m_eyedropperBtn);
    m_panBtn->setChecked(clickedBtn == m_panBtn);
    m_zoomBtn->setChecked(clickedBtn == m_zoomBtn);


    // Emit tool ID

    if (clickedBtn == m_penBtn) {
        emit toolSelected(0);
    }
    else if (clickedBtn == m_eraserBtn) {
        emit toolSelected(1);
    }
    else if (clickedBtn == m_fillBtn) {
        emit toolSelected(2);
    }
    else if (clickedBtn == m_eyedropperBtn) {
        emit toolSelected(3);
    }
    else if (clickedBtn == m_lineBtn) {
        emit toolSelected(4);
    }
    else if (clickedBtn == m_moveBtn) {
        emit toolSelected(5);
    }
    else if (clickedBtn == m_rectSelectBtn) {
        emit toolSelected(6);
    }
    else if (clickedBtn == m_ellipseSelectBtn) {
        emit toolSelected(7);
    }
    else if (clickedBtn == m_lassoSelectBtn) {
        emit toolSelected(8);
    }
    else if (clickedBtn == m_textBtn) {
        emit toolSelected(9);
    }
    else if (clickedBtn == m_zoomBtn) {
        emit toolSelected(10);
    }
    else if (clickedBtn == m_panBtn) {
        emit toolSelected(11);
    }
}


void ToolPanel::onSwapColorsClicked()
{
    emit swapColors();
}