/********************************************************************************
** Form generated from reading UI file 'sliderlabel.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SLIDERLABEL_H
#define UI_SLIDERLABEL_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSlider>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SliderLabel
{
public:
    QWidget *horizontalLayoutWidget;
    QHBoxLayout *horizontalLayout;
    QLabel *nameLabel;
    QSlider *slider;
    QLabel *valueLabel;

    void setupUi(QWidget *SliderLabel)
    {
        if (SliderLabel->objectName().isEmpty())
            SliderLabel->setObjectName(QStringLiteral("SliderLabel"));
        SliderLabel->resize(400, 30);
        horizontalLayoutWidget = new QWidget(SliderLabel);
        horizontalLayoutWidget->setObjectName(QStringLiteral("horizontalLayoutWidget"));
        horizontalLayoutWidget->setGeometry(QRect(0, 0, 401, 31));
        horizontalLayout = new QHBoxLayout(horizontalLayoutWidget);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        nameLabel = new QLabel(horizontalLayoutWidget);
        nameLabel->setObjectName(QStringLiteral("nameLabel"));
        nameLabel->setMinimumSize(QSize(100, 0));
        nameLabel->setMaximumSize(QSize(100, 16777215));

        horizontalLayout->addWidget(nameLabel);

        slider = new QSlider(horizontalLayoutWidget);
        slider->setObjectName(QStringLiteral("slider"));
        slider->setOrientation(Qt::Horizontal);

        horizontalLayout->addWidget(slider);

        valueLabel = new QLabel(horizontalLayoutWidget);
        valueLabel->setObjectName(QStringLiteral("valueLabel"));

        horizontalLayout->addWidget(valueLabel);


        retranslateUi(SliderLabel);

        QMetaObject::connectSlotsByName(SliderLabel);
    } // setupUi

    void retranslateUi(QWidget *SliderLabel)
    {
        SliderLabel->setWindowTitle(QApplication::translate("SliderLabel", "Form", 0));
        nameLabel->setText(QApplication::translate("SliderLabel", "TextLabel", 0));
        valueLabel->setText(QApplication::translate("SliderLabel", "0.0", 0));
    } // retranslateUi

};

namespace Ui {
    class SliderLabel: public Ui_SliderLabel {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SLIDERLABEL_H
