/********************************************************************************
** Form generated from reading UI file 'ToolsPanel.ui'
**
** Created by: Qt User Interface Compiler version 6.2.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TOOLSPANEL_H
#define UI_TOOLSPANEL_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ToolsPanel
{
public:
    QVBoxLayout *mainLayout;
    QGridLayout *toolGrid;
    QToolButton *btnPointer;
    QToolButton *btnMove;
    QToolButton *btnBrush;
    QToolButton *btnEraser;
    QToolButton *btnFill;
    QToolButton *btnColorPicker;
    QToolButton *btnText;
    QToolButton *btnRectSelect;
    QToolButton *btnZoom;
    QToolButton *btnPan;
    QFrame *separator1;
    QLabel *sizeLabel;
    QSpinBox *brushSizeSpin;
    QFrame *separator2;
    QLabel *colorLabel;
    QPushButton *btnColor;
    QSpacerItem *verticalSpacer;

    void setupUi(QWidget *ToolsPanel)
    {
        if (ToolsPanel->objectName().isEmpty())
            ToolsPanel->setObjectName(QString::fromUtf8("ToolsPanel"));
        ToolsPanel->setMinimumSize(QSize(80, 0));
        ToolsPanel->setMaximumSize(QSize(110, 16777215));
        mainLayout = new QVBoxLayout(ToolsPanel);
        mainLayout->setSpacing(4);
        mainLayout->setObjectName(QString::fromUtf8("mainLayout"));
        mainLayout->setContentsMargins(4, 4, 4, 4);
        toolGrid = new QGridLayout();
        toolGrid->setSpacing(2);
        toolGrid->setObjectName(QString::fromUtf8("toolGrid"));
        btnPointer = new QToolButton(ToolsPanel);
        btnPointer->setObjectName(QString::fromUtf8("btnPointer"));
        btnPointer->setCheckable(true);
        btnPointer->setMinimumSize(QSize(34, 34));

        toolGrid->addWidget(btnPointer, 0, 0, 1, 1);

        btnMove = new QToolButton(ToolsPanel);
        btnMove->setObjectName(QString::fromUtf8("btnMove"));
        btnMove->setCheckable(true);
        btnMove->setMinimumSize(QSize(34, 34));

        toolGrid->addWidget(btnMove, 0, 1, 1, 1);

        btnBrush = new QToolButton(ToolsPanel);
        btnBrush->setObjectName(QString::fromUtf8("btnBrush"));
        btnBrush->setCheckable(true);
        btnBrush->setMinimumSize(QSize(34, 34));

        toolGrid->addWidget(btnBrush, 1, 0, 1, 1);

        btnEraser = new QToolButton(ToolsPanel);
        btnEraser->setObjectName(QString::fromUtf8("btnEraser"));
        btnEraser->setCheckable(true);
        btnEraser->setMinimumSize(QSize(34, 34));

        toolGrid->addWidget(btnEraser, 1, 1, 1, 1);

        btnFill = new QToolButton(ToolsPanel);
        btnFill->setObjectName(QString::fromUtf8("btnFill"));
        btnFill->setCheckable(true);
        btnFill->setMinimumSize(QSize(34, 34));

        toolGrid->addWidget(btnFill, 2, 0, 1, 1);

        btnColorPicker = new QToolButton(ToolsPanel);
        btnColorPicker->setObjectName(QString::fromUtf8("btnColorPicker"));
        btnColorPicker->setCheckable(true);
        btnColorPicker->setMinimumSize(QSize(34, 34));

        toolGrid->addWidget(btnColorPicker, 2, 1, 1, 1);

        btnText = new QToolButton(ToolsPanel);
        btnText->setObjectName(QString::fromUtf8("btnText"));
        btnText->setCheckable(true);
        btnText->setMinimumSize(QSize(34, 34));

        toolGrid->addWidget(btnText, 3, 0, 1, 1);

        btnRectSelect = new QToolButton(ToolsPanel);
        btnRectSelect->setObjectName(QString::fromUtf8("btnRectSelect"));
        btnRectSelect->setCheckable(true);
        btnRectSelect->setMinimumSize(QSize(34, 34));

        toolGrid->addWidget(btnRectSelect, 3, 1, 1, 1);

        btnZoom = new QToolButton(ToolsPanel);
        btnZoom->setObjectName(QString::fromUtf8("btnZoom"));
        btnZoom->setCheckable(true);
        btnZoom->setMinimumSize(QSize(34, 34));

        toolGrid->addWidget(btnZoom, 4, 0, 1, 1);

        btnPan = new QToolButton(ToolsPanel);
        btnPan->setObjectName(QString::fromUtf8("btnPan"));
        btnPan->setCheckable(true);
        btnPan->setMinimumSize(QSize(34, 34));

        toolGrid->addWidget(btnPan, 4, 1, 1, 1);


        mainLayout->addLayout(toolGrid);

        separator1 = new QFrame(ToolsPanel);
        separator1->setObjectName(QString::fromUtf8("separator1"));
        separator1->setFrameShape(QFrame::HLine);
        separator1->setFrameShadow(QFrame::Sunken);

        mainLayout->addWidget(separator1);

        sizeLabel = new QLabel(ToolsPanel);
        sizeLabel->setObjectName(QString::fromUtf8("sizeLabel"));

        mainLayout->addWidget(sizeLabel);

        brushSizeSpin = new QSpinBox(ToolsPanel);
        brushSizeSpin->setObjectName(QString::fromUtf8("brushSizeSpin"));

        mainLayout->addWidget(brushSizeSpin);

        separator2 = new QFrame(ToolsPanel);
        separator2->setObjectName(QString::fromUtf8("separator2"));
        separator2->setFrameShape(QFrame::HLine);
        separator2->setFrameShadow(QFrame::Sunken);

        mainLayout->addWidget(separator2);

        colorLabel = new QLabel(ToolsPanel);
        colorLabel->setObjectName(QString::fromUtf8("colorLabel"));

        mainLayout->addWidget(colorLabel);

        btnColor = new QPushButton(ToolsPanel);
        btnColor->setObjectName(QString::fromUtf8("btnColor"));
        btnColor->setMinimumSize(QSize(40, 30));
        btnColor->setMaximumSize(QSize(16777215, 30));

        mainLayout->addWidget(btnColor);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        mainLayout->addItem(verticalSpacer);


        retranslateUi(ToolsPanel);

        QMetaObject::connectSlotsByName(ToolsPanel);
    } // setupUi

    void retranslateUi(QWidget *ToolsPanel)
    {
#if QT_CONFIG(tooltip)
        btnPointer->setToolTip(QCoreApplication::translate("ToolsPanel", "Pointer (V)", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        btnMove->setToolTip(QCoreApplication::translate("ToolsPanel", "Move (M)", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        btnBrush->setToolTip(QCoreApplication::translate("ToolsPanel", "Brush (B)", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        btnEraser->setToolTip(QCoreApplication::translate("ToolsPanel", "Eraser (E)", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        btnFill->setToolTip(QCoreApplication::translate("ToolsPanel", "Fill Bucket (G)", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        btnColorPicker->setToolTip(QCoreApplication::translate("ToolsPanel", "Color Picker (I)", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        btnText->setToolTip(QCoreApplication::translate("ToolsPanel", "Text (T)", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        btnRectSelect->setToolTip(QCoreApplication::translate("ToolsPanel", "Rectangle Select (R)", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        btnZoom->setToolTip(QCoreApplication::translate("ToolsPanel", "Zoom (Z)", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        btnPan->setToolTip(QCoreApplication::translate("ToolsPanel", "Pan / Hand (H)", nullptr));
#endif // QT_CONFIG(tooltip)
        sizeLabel->setText(QCoreApplication::translate("ToolsPanel", "Size:", nullptr));
        colorLabel->setText(QCoreApplication::translate("ToolsPanel", "Color:", nullptr));
        btnColor->setText(QString());
        (void)ToolsPanel;
    } // retranslateUi

};

namespace Ui {
    class ToolsPanel: public Ui_ToolsPanel {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TOOLSPANEL_H
