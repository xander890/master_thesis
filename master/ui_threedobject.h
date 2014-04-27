/********************************************************************************
** Form generated from reading UI file 'threedobject.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_THREEDOBJECT_H
#define UI_THREEDOBJECT_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "onelinetextedit.h"
#include "vectorform.h"

QT_BEGIN_NAMESPACE

class Ui_ThreeDObjectGUI
{
public:
    QGroupBox *groupBox;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label;
    OneLineTextEdit *name;
    VectorForm *position;
    VectorForm *rotation;
    VectorForm *scale;
    QSpacerItem *verticalSpacer;

    void setupUi(QWidget *ThreeDObjectGUI)
    {
        if (ThreeDObjectGUI->objectName().isEmpty())
            ThreeDObjectGUI->setObjectName(QStringLiteral("ThreeDObjectGUI"));
        ThreeDObjectGUI->resize(430, 210);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(ThreeDObjectGUI->sizePolicy().hasHeightForWidth());
        ThreeDObjectGUI->setSizePolicy(sizePolicy);
        ThreeDObjectGUI->setMinimumSize(QSize(430, 210));
        groupBox = new QGroupBox(ThreeDObjectGUI);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setGeometry(QRect(10, 10, 411, 191));
        groupBox->setFlat(false);
        groupBox->setCheckable(true);
        verticalLayoutWidget = new QWidget(groupBox);
        verticalLayoutWidget->setObjectName(QStringLiteral("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(10, 20, 402, 161));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setSizeConstraint(QLayout::SetMinimumSize);
        label = new QLabel(verticalLayoutWidget);
        label->setObjectName(QStringLiteral("label"));
        sizePolicy.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy);
        label->setMinimumSize(QSize(100, 30));

        horizontalLayout_2->addWidget(label);

        name = new OneLineTextEdit(verticalLayoutWidget);
        name->setObjectName(QStringLiteral("name"));
        name->setMaximumSize(QSize(16777215, 20));

        horizontalLayout_2->addWidget(name);


        verticalLayout->addLayout(horizontalLayout_2);

        position = new VectorForm(verticalLayoutWidget);
        position->setObjectName(QStringLiteral("position"));
        position->setEnabled(true);
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(position->sizePolicy().hasHeightForWidth());
        position->setSizePolicy(sizePolicy1);
        position->setMinimumSize(QSize(400, 30));
        position->setMaximumSize(QSize(400, 30));

        verticalLayout->addWidget(position);

        rotation = new VectorForm(verticalLayoutWidget);
        rotation->setObjectName(QStringLiteral("rotation"));
        sizePolicy1.setHeightForWidth(rotation->sizePolicy().hasHeightForWidth());
        rotation->setSizePolicy(sizePolicy1);
        rotation->setMinimumSize(QSize(400, 30));
        rotation->setMaximumSize(QSize(400, 30));

        verticalLayout->addWidget(rotation);

        scale = new VectorForm(verticalLayoutWidget);
        scale->setObjectName(QStringLiteral("scale"));
        sizePolicy1.setHeightForWidth(scale->sizePolicy().hasHeightForWidth());
        scale->setSizePolicy(sizePolicy1);
        scale->setMinimumSize(QSize(400, 30));
        scale->setMaximumSize(QSize(400, 30));

        verticalLayout->addWidget(scale);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        verticalLayoutWidget->raise();
        groupBox->raise();
        scale->raise();
        scale->raise();

        retranslateUi(ThreeDObjectGUI);

        QMetaObject::connectSlotsByName(ThreeDObjectGUI);
    } // setupUi

    void retranslateUi(QWidget *ThreeDObjectGUI)
    {
        ThreeDObjectGUI->setWindowTitle(QApplication::translate("ThreeDObjectGUI", "Form", 0));
        groupBox->setTitle(QApplication::translate("ThreeDObjectGUI", "Object", 0));
        label->setText(QApplication::translate("ThreeDObjectGUI", "Name: ", 0));
    } // retranslateUi

};

namespace Ui {
    class ThreeDObjectGUI: public Ui_ThreeDObjectGUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_THREEDOBJECT_H
