/********************************************************************************
** Form generated from reading UI file 'vectorform.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VECTORFORM_H
#define UI_VECTORFORM_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>
#include "onelinetextedit.h"

QT_BEGIN_NAMESPACE

class Ui_VectorForm
{
public:
    QWidget *horizontalLayoutWidget;
    QHBoxLayout *horivec;
    QLabel *name;
    QLabel *xLabel;
    OneLineTextEdit *x;
    QLabel *yLabel;
    OneLineTextEdit *y;
    QLabel *zLabel;
    OneLineTextEdit *z;
    QLabel *aLabel;
    OneLineTextEdit *a;
    QSpacerItem *horizontalSpacer;

    void setupUi(QWidget *VectorForm)
    {
        if (VectorForm->objectName().isEmpty())
            VectorForm->setObjectName(QStringLiteral("VectorForm"));
        VectorForm->resize(400, 30);
        QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(VectorForm->sizePolicy().hasHeightForWidth());
        VectorForm->setSizePolicy(sizePolicy);
        VectorForm->setMinimumSize(QSize(0, 30));
        horizontalLayoutWidget = new QWidget(VectorForm);
        horizontalLayoutWidget->setObjectName(QStringLiteral("horizontalLayoutWidget"));
        horizontalLayoutWidget->setGeometry(QRect(0, 0, 451, 22));
        horivec = new QHBoxLayout(horizontalLayoutWidget);
        horivec->setObjectName(QStringLiteral("horivec"));
        horivec->setSizeConstraint(QLayout::SetDefaultConstraint);
        horivec->setContentsMargins(0, 0, 0, 0);
        name = new QLabel(horizontalLayoutWidget);
        name->setObjectName(QStringLiteral("name"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(name->sizePolicy().hasHeightForWidth());
        name->setSizePolicy(sizePolicy1);
        name->setMinimumSize(QSize(75, 0));

        horivec->addWidget(name);

        xLabel = new QLabel(horizontalLayoutWidget);
        xLabel->setObjectName(QStringLiteral("xLabel"));
        sizePolicy1.setHeightForWidth(xLabel->sizePolicy().hasHeightForWidth());
        xLabel->setSizePolicy(sizePolicy1);
        xLabel->setMinimumSize(QSize(15, 0));

        horivec->addWidget(xLabel);

        x = new OneLineTextEdit(horizontalLayoutWidget);
        x->setObjectName(QStringLiteral("x"));
        x->setMaximumSize(QSize(50, 20));

        horivec->addWidget(x);

        yLabel = new QLabel(horizontalLayoutWidget);
        yLabel->setObjectName(QStringLiteral("yLabel"));
        sizePolicy1.setHeightForWidth(yLabel->sizePolicy().hasHeightForWidth());
        yLabel->setSizePolicy(sizePolicy1);
        yLabel->setMinimumSize(QSize(15, 0));

        horivec->addWidget(yLabel);

        y = new OneLineTextEdit(horizontalLayoutWidget);
        y->setObjectName(QStringLiteral("y"));
        y->setMaximumSize(QSize(50, 20));

        horivec->addWidget(y);

        zLabel = new QLabel(horizontalLayoutWidget);
        zLabel->setObjectName(QStringLiteral("zLabel"));
        sizePolicy1.setHeightForWidth(zLabel->sizePolicy().hasHeightForWidth());
        zLabel->setSizePolicy(sizePolicy1);
        zLabel->setMinimumSize(QSize(15, 0));

        horivec->addWidget(zLabel);

        z = new OneLineTextEdit(horizontalLayoutWidget);
        z->setObjectName(QStringLiteral("z"));
        z->setMaximumSize(QSize(50, 20));

        horivec->addWidget(z);

        aLabel = new QLabel(horizontalLayoutWidget);
        aLabel->setObjectName(QStringLiteral("aLabel"));
        aLabel->setMinimumSize(QSize(15, 0));

        horivec->addWidget(aLabel);

        a = new OneLineTextEdit(horizontalLayoutWidget);
        a->setObjectName(QStringLiteral("a"));
        a->setMaximumSize(QSize(50, 20));

        horivec->addWidget(a);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horivec->addItem(horizontalSpacer);


        retranslateUi(VectorForm);

        QMetaObject::connectSlotsByName(VectorForm);
    } // setupUi

    void retranslateUi(QWidget *VectorForm)
    {
        VectorForm->setWindowTitle(QApplication::translate("VectorForm", "Form", 0));
        name->setText(QApplication::translate("VectorForm", "TextLabel", 0));
        xLabel->setText(QApplication::translate("VectorForm", "X:", 0));
        yLabel->setText(QApplication::translate("VectorForm", "Y:", 0));
        zLabel->setText(QApplication::translate("VectorForm", "Z:", 0));
        aLabel->setText(QApplication::translate("VectorForm", "W:", 0));
    } // retranslateUi

};

namespace Ui {
    class VectorForm: public Ui_VectorForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VECTORFORM_H
