/********************************************************************************
** Form generated from reading UI file 'colorform.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_COLORFORM_H
#define UI_COLORFORM_H

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

class Ui_ColorForm
{
public:
    QWidget *horizontalLayoutWidget;
    QHBoxLayout *horivec;
    QLabel *name;
    QLabel *xLabel;
    OneLineTextEdit *r;
    QLabel *yLabel;
    OneLineTextEdit *g;
    QLabel *zLabel;
    OneLineTextEdit *b;
    QLabel *aLabel;
    OneLineTextEdit *a;
    QSpacerItem *horizontalSpacer;

    void setupUi(QWidget *ColorForm)
    {
        if (ColorForm->objectName().isEmpty())
            ColorForm->setObjectName(QStringLiteral("ColorForm"));
        ColorForm->resize(450, 21);
        horizontalLayoutWidget = new QWidget(ColorForm);
        horizontalLayoutWidget->setObjectName(QStringLiteral("horizontalLayoutWidget"));
        horizontalLayoutWidget->setGeometry(QRect(0, 0, 451, 22));
        horivec = new QHBoxLayout(horizontalLayoutWidget);
        horivec->setObjectName(QStringLiteral("horivec"));
        horivec->setSizeConstraint(QLayout::SetDefaultConstraint);
        horivec->setContentsMargins(0, 0, 0, 0);
        name = new QLabel(horizontalLayoutWidget);
        name->setObjectName(QStringLiteral("name"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(name->sizePolicy().hasHeightForWidth());
        name->setSizePolicy(sizePolicy);
        name->setMinimumSize(QSize(75, 0));

        horivec->addWidget(name);

        xLabel = new QLabel(horizontalLayoutWidget);
        xLabel->setObjectName(QStringLiteral("xLabel"));
        sizePolicy.setHeightForWidth(xLabel->sizePolicy().hasHeightForWidth());
        xLabel->setSizePolicy(sizePolicy);
        xLabel->setMinimumSize(QSize(15, 0));

        horivec->addWidget(xLabel);

        r = new OneLineTextEdit(horizontalLayoutWidget);
        r->setObjectName(QStringLiteral("r"));
        r->setMaximumSize(QSize(50, 20));

        horivec->addWidget(r);

        yLabel = new QLabel(horizontalLayoutWidget);
        yLabel->setObjectName(QStringLiteral("yLabel"));
        sizePolicy.setHeightForWidth(yLabel->sizePolicy().hasHeightForWidth());
        yLabel->setSizePolicy(sizePolicy);
        yLabel->setMinimumSize(QSize(15, 0));

        horivec->addWidget(yLabel);

        g = new OneLineTextEdit(horizontalLayoutWidget);
        g->setObjectName(QStringLiteral("g"));
        g->setMaximumSize(QSize(50, 20));

        horivec->addWidget(g);

        zLabel = new QLabel(horizontalLayoutWidget);
        zLabel->setObjectName(QStringLiteral("zLabel"));
        sizePolicy.setHeightForWidth(zLabel->sizePolicy().hasHeightForWidth());
        zLabel->setSizePolicy(sizePolicy);
        zLabel->setMinimumSize(QSize(15, 0));

        horivec->addWidget(zLabel);

        b = new OneLineTextEdit(horizontalLayoutWidget);
        b->setObjectName(QStringLiteral("b"));
        b->setMaximumSize(QSize(50, 20));

        horivec->addWidget(b);

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


        retranslateUi(ColorForm);

        QMetaObject::connectSlotsByName(ColorForm);
    } // setupUi

    void retranslateUi(QWidget *ColorForm)
    {
        ColorForm->setWindowTitle(QApplication::translate("ColorForm", "Form", 0));
        name->setText(QApplication::translate("ColorForm", "TextLabel", 0));
        xLabel->setText(QApplication::translate("ColorForm", "R:", 0));
        yLabel->setText(QApplication::translate("ColorForm", "G:", 0));
        zLabel->setText(QApplication::translate("ColorForm", "B:", 0));
        aLabel->setText(QApplication::translate("ColorForm", "A:", 0));
    } // retranslateUi

};

namespace Ui {
    class ColorForm: public Ui_ColorForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_COLORFORM_H
