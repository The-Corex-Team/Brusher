#include "CanvasSizeDialog.h"
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpinBox>
#include <QPushButton>

CanvasSizeDialog::CanvasSizeDialog(int currentWidth, int currentHeight, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Canvas Size"));

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QFormLayout *formLayout = new QFormLayout();

    m_widthSpinBox = new QSpinBox(this);
    m_widthSpinBox->setRange(1, 4000);
    m_widthSpinBox->setValue(currentWidth);

    m_heightSpinBox = new QSpinBox(this);
    m_heightSpinBox->setRange(1, 4000);
    m_heightSpinBox->setValue(currentHeight);

    formLayout->addRow(tr("Width (px):"), m_widthSpinBox);
    formLayout->addRow(tr("Height (px):"), m_heightSpinBox);
    mainLayout->addLayout(formLayout);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *okBtn = new QPushButton(tr("Apply"), this);
    QPushButton *cancelBtn = new QPushButton(tr("Cancel"), this);
    buttonLayout->addStretch();
    buttonLayout->addWidget(okBtn);
    buttonLayout->addWidget(cancelBtn);
    mainLayout->addLayout(buttonLayout);

    connect(okBtn, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
}

int CanvasSizeDialog::getCanvasWidth() const { return m_widthSpinBox->value(); }
int CanvasSizeDialog::getCanvasHeight() const { return m_heightSpinBox->value(); }
