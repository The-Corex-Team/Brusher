#include "ToolPanel.h"
#include <QGridLayout>
#include <QToolButton>
#include <QWidget>

ToolPanel::ToolPanel(QWidget *parent)
    : QDockWidget(tr("Tools"), parent)
{
    QWidget *content = new QWidget(this);
    QGridLayout *layout = new QGridLayout(content);
    layout->setContentsMargins(5, 5, 5, 5);
    layout->setSpacing(5);

    m_penBtn = createToolButton(QIcon(":/src/icons/brush.svg"), "Pen");
    m_penBtn->setChecked(true); // Default
    
    m_eraserBtn = createToolButton(QIcon(":/src/icons/eraser.svg"), "Eraser");
    m_fillBtn = createToolButton(QIcon(":/src/icons/fill.svg"), "Paint Bucket");
    m_eyedropperBtn = createToolButton(QIcon(":/src/icons/colorpicker.svg"), "Eyedropper");
    m_lineBtn = createToolButton(QIcon(":/src/icons/pointer.svg"), "Line"); // Using pointer as line for now

    layout->addWidget(m_penBtn, 0, 0);
    layout->addWidget(m_eraserBtn, 0, 1);
    layout->addWidget(m_fillBtn, 1, 0);
    layout->addWidget(m_eyedropperBtn, 1, 1);
    layout->addWidget(m_lineBtn, 2, 0);
    layout->setRowStretch(3, 1);
    
    content->setLayout(layout);
    setWidget(content);
}

QToolButton* ToolPanel::createToolButton(const QIcon &icon, const QString &tooltip)
{
    QToolButton *btn = new QToolButton(this);
    btn->setIcon(icon);
    btn->setIconSize(QSize(24, 24));
    btn->setToolTip(tooltip);
    btn->setCheckable(true);
    btn->setFixedSize(36, 36);
    
    connect(btn, &QToolButton::clicked, this, &ToolPanel::onToolClicked);
    return btn;
}

void ToolPanel::onToolClicked()
{
    QToolButton *clickedBtn = qobject_cast<QToolButton*>(sender());
    if (!clickedBtn) return;
    
    m_penBtn->setChecked(clickedBtn == m_penBtn);
    m_eraserBtn->setChecked(clickedBtn == m_eraserBtn);
    m_fillBtn->setChecked(clickedBtn == m_fillBtn);
    m_eyedropperBtn->setChecked(clickedBtn == m_eyedropperBtn);
    m_lineBtn->setChecked(clickedBtn == m_lineBtn);
    
    if (clickedBtn == m_penBtn) emit penToolSelected();
    else if (clickedBtn == m_eraserBtn) emit eraserToolSelected();
    else if (clickedBtn == m_fillBtn) emit fillToolSelected();
    else if (clickedBtn == m_eyedropperBtn) emit eyedropperToolSelected();
    else if (clickedBtn == m_lineBtn) emit lineToolSelected();
}
