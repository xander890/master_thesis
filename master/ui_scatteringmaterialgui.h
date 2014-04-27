/********************************************************************************
** Form generated from reading UI file 'scatteringmaterialgui.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SCATTERINGMATERIALGUI_H
#define UI_SCATTERINGMATERIALGUI_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "sliderlabel.h"
#include "vectorform.h"

QT_BEGIN_NAMESPACE

class Ui_ScatteringMaterialGUI
{
public:
    QGroupBox *groupBox;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    SliderLabel *iorLabel;
    VectorForm *scatteringLabel;
    VectorForm *absorptionLabel;
    VectorForm *meanCosineLabel;
    QSpacerItem *verticalSpacer;

    void setupUi(QWidget *ScatteringMaterialGUI)
    {
        if (ScatteringMaterialGUI->objectName().isEmpty())
            ScatteringMaterialGUI->setObjectName(QStringLiteral("ScatteringMaterialGUI"));
        ScatteringMaterialGUI->resize(400, 150);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(ScatteringMaterialGUI->sizePolicy().hasHeightForWidth());
        ScatteringMaterialGUI->setSizePolicy(sizePolicy);
        ScatteringMaterialGUI->setMinimumSize(QSize(400, 150));
        ScatteringMaterialGUI->setMaximumSize(QSize(400, 150));
        groupBox = new QGroupBox(ScatteringMaterialGUI);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setGeometry(QRect(10, 0, 391, 171));
        verticalLayoutWidget = new QWidget(groupBox);
        verticalLayoutWidget->setObjectName(QStringLiteral("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(10, 20, 381, 121));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        iorLabel = new SliderLabel(verticalLayoutWidget);
        iorLabel->setObjectName(QStringLiteral("iorLabel"));
        iorLabel->setMinimumSize(QSize(400, 30));
        iorLabel->setMaximumSize(QSize(400, 30));

        verticalLayout->addWidget(iorLabel);

        scatteringLabel = new VectorForm(verticalLayoutWidget);
        scatteringLabel->setObjectName(QStringLiteral("scatteringLabel"));
        scatteringLabel->setMinimumSize(QSize(400, 30));
        scatteringLabel->setMaximumSize(QSize(400, 30));

        verticalLayout->addWidget(scatteringLabel);

        absorptionLabel = new VectorForm(verticalLayoutWidget);
        absorptionLabel->setObjectName(QStringLiteral("absorptionLabel"));
        absorptionLabel->setMinimumSize(QSize(400, 30));
        absorptionLabel->setMaximumSize(QSize(400, 30));

        verticalLayout->addWidget(absorptionLabel);

        meanCosineLabel = new VectorForm(verticalLayoutWidget);
        meanCosineLabel->setObjectName(QStringLiteral("meanCosineLabel"));
        meanCosineLabel->setMinimumSize(QSize(400, 30));
        meanCosineLabel->setMaximumSize(QSize(400, 30));

        verticalLayout->addWidget(meanCosineLabel);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        retranslateUi(ScatteringMaterialGUI);

        QMetaObject::connectSlotsByName(ScatteringMaterialGUI);
    } // setupUi

    void retranslateUi(QWidget *ScatteringMaterialGUI)
    {
        ScatteringMaterialGUI->setWindowTitle(QApplication::translate("ScatteringMaterialGUI", "Form", 0));
        groupBox->setTitle(QApplication::translate("ScatteringMaterialGUI", "Material", 0));
    } // retranslateUi

};

namespace Ui {
    class ScatteringMaterialGUI: public Ui_ScatteringMaterialGUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SCATTERINGMATERIALGUI_H
