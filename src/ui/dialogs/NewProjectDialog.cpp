#include "NewProjectDialog.h"
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>

NewProjectDialog::NewProjectDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("New Project"));

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QFormLayout *formLayout = new QFormLayout();

    m_widthSpinBox = new QSpinBox(this);
    m_widthSpinBox->setRange(1, 4000);
    m_widthSpinBox->setValue(800);
    
    m_heightSpinBox = new QSpinBox(this);
    m_heightSpinBox->setRange(1, 4000);
    m_heightSpinBox->setValue(600);

    formLayout->addRow(tr("Width (px):"), m_widthSpinBox);
    formLayout->addRow(tr("Height (px):"), m_heightSpinBox);

    mainLayout->addLayout(formLayout);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *okBtn = new QPushButton(tr("Create"), this);
    QPushButton *cancelBtn = new QPushButton(tr("Cancel"), this);
    
    buttonLayout->addStretch();
    buttonLayout->addWidget(okBtn);
    buttonLayout->addWidget(cancelBtn);

    mainLayout->addLayout(buttonLayout);

    connect(okBtn, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
}

int NewProjectDialog::getCanvasWidth() const
{
    return m_widthSpinBox->value();
}

int NewProjectDialog::getCanvasHeight() const
{
    return m_heightSpinBox->value();
}
