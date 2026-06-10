/********************************************************************************
** Form generated from reading UI file 'LayersPanel.ui'
**
** Created by: Qt User Interface Compiler version 6.2.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LAYERSPANEL_H
#define UI_LAYERSPANEL_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LayersPanel
{
public:
    QVBoxLayout *mainLayout;
    QHBoxLayout *blendLayout;
    QLabel *blendLabel;
    QComboBox *blendModeCombo;
    QHBoxLayout *opacityLayout;
    QLabel *opacityTextLabel;
    QSlider *opacitySlider;
    QLabel *opacityLabel;
    QListWidget *layerList;
    QHBoxLayout *buttonLayout;
    QPushButton *btnAddLayer;
    QPushButton *btnDeleteLayer;
    QPushButton *btnDuplicateLayer;
    QSpacerItem *btnSpacer;
    QPushButton *btnMoveUp;
    QPushButton *btnMoveDown;

    void setupUi(QWidget *LayersPanel)
    {
        if (LayersPanel->objectName().isEmpty())
            LayersPanel->setObjectName(QString::fromUtf8("LayersPanel"));
        LayersPanel->setMinimumSize(QSize(200, 0));
        mainLayout = new QVBoxLayout(LayersPanel);
        mainLayout->setSpacing(4);
        mainLayout->setObjectName(QString::fromUtf8("mainLayout"));
        mainLayout->setContentsMargins(4, 4, 4, 4);
        blendLayout = new QHBoxLayout();
        blendLayout->setObjectName(QString::fromUtf8("blendLayout"));
        blendLabel = new QLabel(LayersPanel);
        blendLabel->setObjectName(QString::fromUtf8("blendLabel"));

        blendLayout->addWidget(blendLabel);

        blendModeCombo = new QComboBox(LayersPanel);
        blendModeCombo->setObjectName(QString::fromUtf8("blendModeCombo"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(blendModeCombo->sizePolicy().hasHeightForWidth());
        blendModeCombo->setSizePolicy(sizePolicy);

        blendLayout->addWidget(blendModeCombo);


        mainLayout->addLayout(blendLayout);

        opacityLayout = new QHBoxLayout();
        opacityLayout->setObjectName(QString::fromUtf8("opacityLayout"));
        opacityTextLabel = new QLabel(LayersPanel);
        opacityTextLabel->setObjectName(QString::fromUtf8("opacityTextLabel"));

        opacityLayout->addWidget(opacityTextLabel);

        opacitySlider = new QSlider(LayersPanel);
        opacitySlider->setObjectName(QString::fromUtf8("opacitySlider"));
        opacitySlider->setOrientation(Qt::Horizontal);
        sizePolicy.setHeightForWidth(opacitySlider->sizePolicy().hasHeightForWidth());
        opacitySlider->setSizePolicy(sizePolicy);

        opacityLayout->addWidget(opacitySlider);

        opacityLabel = new QLabel(LayersPanel);
        opacityLabel->setObjectName(QString::fromUtf8("opacityLabel"));
        opacityLabel->setMinimumSize(QSize(40, 0));
        opacityLabel->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

        opacityLayout->addWidget(opacityLabel);


        mainLayout->addLayout(opacityLayout);

        layerList = new QListWidget(LayersPanel);
        layerList->setObjectName(QString::fromUtf8("layerList"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(1);
        sizePolicy1.setHeightForWidth(layerList->sizePolicy().hasHeightForWidth());
        layerList->setSizePolicy(sizePolicy1);

        mainLayout->addWidget(layerList);

        buttonLayout = new QHBoxLayout();
        buttonLayout->setSpacing(2);
        buttonLayout->setObjectName(QString::fromUtf8("buttonLayout"));
        btnAddLayer = new QPushButton(LayersPanel);
        btnAddLayer->setObjectName(QString::fromUtf8("btnAddLayer"));
        btnAddLayer->setMaximumSize(QSize(32, 28));

        buttonLayout->addWidget(btnAddLayer);

        btnDeleteLayer = new QPushButton(LayersPanel);
        btnDeleteLayer->setObjectName(QString::fromUtf8("btnDeleteLayer"));
        btnDeleteLayer->setMaximumSize(QSize(32, 28));

        buttonLayout->addWidget(btnDeleteLayer);

        btnDuplicateLayer = new QPushButton(LayersPanel);
        btnDuplicateLayer->setObjectName(QString::fromUtf8("btnDuplicateLayer"));
        btnDuplicateLayer->setMaximumSize(QSize(40, 28));

        buttonLayout->addWidget(btnDuplicateLayer);

        btnSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

        buttonLayout->addItem(btnSpacer);

        btnMoveUp = new QPushButton(LayersPanel);
        btnMoveUp->setObjectName(QString::fromUtf8("btnMoveUp"));
        btnMoveUp->setMaximumSize(QSize(36, 28));

        buttonLayout->addWidget(btnMoveUp);

        btnMoveDown = new QPushButton(LayersPanel);
        btnMoveDown->setObjectName(QString::fromUtf8("btnMoveDown"));
        btnMoveDown->setMaximumSize(QSize(36, 28));

        buttonLayout->addWidget(btnMoveDown);


        mainLayout->addLayout(buttonLayout);


        retranslateUi(LayersPanel);

        QMetaObject::connectSlotsByName(LayersPanel);
    } // setupUi

    void retranslateUi(QWidget *LayersPanel)
    {
        blendLabel->setText(QCoreApplication::translate("LayersPanel", "Blend:", nullptr));
        opacityTextLabel->setText(QCoreApplication::translate("LayersPanel", "Opacity:", nullptr));
        opacityLabel->setText(QCoreApplication::translate("LayersPanel", "100%", nullptr));
        btnAddLayer->setText(QCoreApplication::translate("LayersPanel", "+", nullptr));
#if QT_CONFIG(tooltip)
        btnAddLayer->setToolTip(QCoreApplication::translate("LayersPanel", "Add Layer", nullptr));
#endif // QT_CONFIG(tooltip)
        btnDeleteLayer->setText(QCoreApplication::translate("LayersPanel", "-", nullptr));
#if QT_CONFIG(tooltip)
        btnDeleteLayer->setToolTip(QCoreApplication::translate("LayersPanel", "Delete Layer", nullptr));
#endif // QT_CONFIG(tooltip)
        btnDuplicateLayer->setText(QCoreApplication::translate("LayersPanel", "Dup", nullptr));
#if QT_CONFIG(tooltip)
        btnDuplicateLayer->setToolTip(QCoreApplication::translate("LayersPanel", "Duplicate Layer", nullptr));
#endif // QT_CONFIG(tooltip)
        btnMoveUp->setText(QCoreApplication::translate("LayersPanel", "Up", nullptr));
#if QT_CONFIG(tooltip)
        btnMoveUp->setToolTip(QCoreApplication::translate("LayersPanel", "Move Layer Up", nullptr));
#endif // QT_CONFIG(tooltip)
        btnMoveDown->setText(QCoreApplication::translate("LayersPanel", "Dn", nullptr));
#if QT_CONFIG(tooltip)
        btnMoveDown->setToolTip(QCoreApplication::translate("LayersPanel", "Move Layer Down", nullptr));
#endif // QT_CONFIG(tooltip)
        (void)LayersPanel;
    } // retranslateUi

};

namespace Ui {
    class LayersPanel: public Ui_LayersPanel {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LAYERSPANEL_H
