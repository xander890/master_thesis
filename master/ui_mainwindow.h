/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "colorform.h"
#include "scatteringmaterialgui.h"
#include "sliderlabel.h"
#include "threedobjectgui.h"
#include "translucentmaterials.h"
#include "vectorform.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionSave_Image;
    QAction *actionQuit;
    QAction *actionCube;
    QAction *actionSphere;
    QAction *actionBunny;
    QAction *actionCow;
    QAction *actionPlane;
    QWidget *centralwidget;
    QWidget *horizontalLayoutWidget;
    QHBoxLayout *mainLayout;
    TranslucentMaterials *translucentMaterials;
    QVBoxLayout *rightLayout;
    QTabWidget *Objects;
    QWidget *General;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QGroupBox *groupBox;
    VectorForm *userPosition;
    ColorForm *clearColor;
    VectorForm *userDirection;
    QWidget *horizontalLayoutWidget_2;
    QHBoxLayout *horizontalLayout;
    QLabel *intlable;
    QSlider *intensity;
    QLabel *intensitylabel;
    QWidget *horizontalLayoutWidget_3;
    QHBoxLayout *horizontalLayout_2;
    QCheckBox *showaxes;
    QCheckBox *showgrid;
    SliderLabel *gamma;
    QGroupBox *statistics;
    QWidget *verticalLayoutWidget_2;
    QVBoxLayout *verticalLayout_2;
    QLabel *fpslabel;
    QSpacerItem *verticalSpacer_2;
    QCheckBox *vertexpixel;
    QVBoxLayout *verticalLayout_4;
    QLabel *label_2;
    QRadioButton *jensenbutton;
    QRadioButton *directionalbutton;
    QSpacerItem *verticalSpacer;
    QWidget *Objects_2;
    QWidget *verticalLayoutWidget_3;
    QVBoxLayout *objslayout;
    ThreeDObjectGUI *objTest;
    ScatteringMaterialGUI *materialTest;
    QSpacerItem *verticalSpacer_3;
    QWidget *tab;
    QWidget *verticalLayoutWidget_4;
    QVBoxLayout *verticalLayout_3;
    SliderLabel *radius;
    SliderLabel *samples;
    SliderLabel *epsilon_gbuffer;
    SliderLabel *epsilon_combination;
    SliderLabel *shadow_bias;
    SliderLabel *lod;
    QGridLayout *gridLayout;
    QCheckBox *cubemapVisible;
    QLabel *label;
    QCheckBox *minusZcheck;
    QCheckBox *minusYcheck;
    QCheckBox *plusZcheck;
    QCheckBox *plusYcheck;
    QCheckBox *minusXcheck;
    QCheckBox *plusXcheck;
    QSpacerItem *verticalSpacer_4;
    QMenuBar *menubar;
    QMenu *menuFile;
    QMenu *menuObjects;
    QMenu *menuAdd;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(1143, 746);
        actionSave_Image = new QAction(MainWindow);
        actionSave_Image->setObjectName(QStringLiteral("actionSave_Image"));
        actionQuit = new QAction(MainWindow);
        actionQuit->setObjectName(QStringLiteral("actionQuit"));
        actionCube = new QAction(MainWindow);
        actionCube->setObjectName(QStringLiteral("actionCube"));
        actionSphere = new QAction(MainWindow);
        actionSphere->setObjectName(QStringLiteral("actionSphere"));
        actionBunny = new QAction(MainWindow);
        actionBunny->setObjectName(QStringLiteral("actionBunny"));
        actionCow = new QAction(MainWindow);
        actionCow->setObjectName(QStringLiteral("actionCow"));
        actionPlane = new QAction(MainWindow);
        actionPlane->setObjectName(QStringLiteral("actionPlane"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        horizontalLayoutWidget = new QWidget(centralwidget);
        horizontalLayoutWidget->setObjectName(QStringLiteral("horizontalLayoutWidget"));
        horizontalLayoutWidget->setGeometry(QRect(0, 0, 1141, 702));
        mainLayout = new QHBoxLayout(horizontalLayoutWidget);
        mainLayout->setSpacing(6);
        mainLayout->setObjectName(QStringLiteral("mainLayout"));
        mainLayout->setContentsMargins(0, 0, 0, 0);
        translucentMaterials = new TranslucentMaterials(horizontalLayoutWidget);
        translucentMaterials->setObjectName(QStringLiteral("translucentMaterials"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Maximum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(translucentMaterials->sizePolicy().hasHeightForWidth());
        translucentMaterials->setSizePolicy(sizePolicy);
        translucentMaterials->setMinimumSize(QSize(700, 700));

        mainLayout->addWidget(translucentMaterials);

        rightLayout = new QVBoxLayout();
        rightLayout->setSpacing(0);
        rightLayout->setObjectName(QStringLiteral("rightLayout"));
        rightLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
        Objects = new QTabWidget(horizontalLayoutWidget);
        Objects->setObjectName(QStringLiteral("Objects"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(Objects->sizePolicy().hasHeightForWidth());
        Objects->setSizePolicy(sizePolicy1);
        Objects->setMinimumSize(QSize(430, 0));
        General = new QWidget();
        General->setObjectName(QStringLiteral("General"));
        verticalLayoutWidget = new QWidget(General);
        verticalLayoutWidget->setObjectName(QStringLiteral("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(0, 0, 421, 641));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(6, 0, 0, 0);
        groupBox = new QGroupBox(verticalLayoutWidget);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        QSizePolicy sizePolicy2(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(groupBox->sizePolicy().hasHeightForWidth());
        groupBox->setSizePolicy(sizePolicy2);
        groupBox->setMinimumSize(QSize(400, 250));
        userPosition = new VectorForm(groupBox);
        userPosition->setObjectName(QStringLiteral("userPosition"));
        userPosition->setGeometry(QRect(10, 50, 400, 30));
        QSizePolicy sizePolicy3(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(userPosition->sizePolicy().hasHeightForWidth());
        userPosition->setSizePolicy(sizePolicy3);
        userPosition->setMinimumSize(QSize(400, 30));
        userPosition->setMaximumSize(QSize(400, 30));
        clearColor = new ColorForm(groupBox);
        clearColor->setObjectName(QStringLiteral("clearColor"));
        clearColor->setGeometry(QRect(10, 20, 400, 30));
        sizePolicy3.setHeightForWidth(clearColor->sizePolicy().hasHeightForWidth());
        clearColor->setSizePolicy(sizePolicy3);
        clearColor->setMinimumSize(QSize(400, 30));
        clearColor->setMaximumSize(QSize(400, 30));
        userDirection = new VectorForm(groupBox);
        userDirection->setObjectName(QStringLiteral("userDirection"));
        userDirection->setGeometry(QRect(10, 80, 400, 30));
        sizePolicy3.setHeightForWidth(userDirection->sizePolicy().hasHeightForWidth());
        userDirection->setSizePolicy(sizePolicy3);
        userDirection->setMinimumSize(QSize(400, 30));
        userDirection->setMaximumSize(QSize(400, 30));
        horizontalLayoutWidget_2 = new QWidget(groupBox);
        horizontalLayoutWidget_2->setObjectName(QStringLiteral("horizontalLayoutWidget_2"));
        horizontalLayoutWidget_2->setGeometry(QRect(10, 170, 381, 29));
        horizontalLayout = new QHBoxLayout(horizontalLayoutWidget_2);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        intlable = new QLabel(horizontalLayoutWidget_2);
        intlable->setObjectName(QStringLiteral("intlable"));

        horizontalLayout->addWidget(intlable);

        intensity = new QSlider(horizontalLayoutWidget_2);
        intensity->setObjectName(QStringLiteral("intensity"));
        intensity->setMinimum(1);
        intensity->setMaximum(100);
        intensity->setOrientation(Qt::Horizontal);
        intensity->setTickPosition(QSlider::TicksAbove);
        intensity->setTickInterval(2);

        horizontalLayout->addWidget(intensity);

        intensitylabel = new QLabel(horizontalLayoutWidget_2);
        intensitylabel->setObjectName(QStringLiteral("intensitylabel"));

        horizontalLayout->addWidget(intensitylabel);

        horizontalLayoutWidget_3 = new QWidget(groupBox);
        horizontalLayoutWidget_3->setObjectName(QStringLiteral("horizontalLayoutWidget_3"));
        horizontalLayoutWidget_3->setGeometry(QRect(10, 140, 371, 31));
        horizontalLayout_2 = new QHBoxLayout(horizontalLayoutWidget_3);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        showaxes = new QCheckBox(horizontalLayoutWidget_3);
        showaxes->setObjectName(QStringLiteral("showaxes"));

        horizontalLayout_2->addWidget(showaxes);

        showgrid = new QCheckBox(horizontalLayoutWidget_3);
        showgrid->setObjectName(QStringLiteral("showgrid"));

        horizontalLayout_2->addWidget(showgrid);

        gamma = new SliderLabel(groupBox);
        gamma->setObjectName(QStringLiteral("gamma"));
        gamma->setGeometry(QRect(0, 110, 401, 31));

        verticalLayout->addWidget(groupBox);

        statistics = new QGroupBox(verticalLayoutWidget);
        statistics->setObjectName(QStringLiteral("statistics"));
        statistics->setMinimumSize(QSize(0, 100));
        verticalLayoutWidget_2 = new QWidget(statistics);
        verticalLayoutWidget_2->setObjectName(QStringLiteral("verticalLayoutWidget_2"));
        verticalLayoutWidget_2->setGeometry(QRect(10, 20, 381, 71));
        verticalLayout_2 = new QVBoxLayout(verticalLayoutWidget_2);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        fpslabel = new QLabel(verticalLayoutWidget_2);
        fpslabel->setObjectName(QStringLiteral("fpslabel"));

        verticalLayout_2->addWidget(fpslabel);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer_2);


        verticalLayout->addWidget(statistics);

        vertexpixel = new QCheckBox(verticalLayoutWidget);
        vertexpixel->setObjectName(QStringLiteral("vertexpixel"));

        verticalLayout->addWidget(vertexpixel);

        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        label_2 = new QLabel(verticalLayoutWidget);
        label_2->setObjectName(QStringLiteral("label_2"));

        verticalLayout_4->addWidget(label_2);

        jensenbutton = new QRadioButton(verticalLayoutWidget);
        jensenbutton->setObjectName(QStringLiteral("jensenbutton"));
        jensenbutton->setChecked(true);

        verticalLayout_4->addWidget(jensenbutton);

        directionalbutton = new QRadioButton(verticalLayoutWidget);
        directionalbutton->setObjectName(QStringLiteral("directionalbutton"));

        verticalLayout_4->addWidget(directionalbutton);


        verticalLayout->addLayout(verticalLayout_4);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        Objects->addTab(General, QString());
        Objects_2 = new QWidget();
        Objects_2->setObjectName(QStringLiteral("Objects_2"));
        verticalLayoutWidget_3 = new QWidget(Objects_2);
        verticalLayoutWidget_3->setObjectName(QStringLiteral("verticalLayoutWidget_3"));
        verticalLayoutWidget_3->setGeometry(QRect(0, 0, 432, 671));
        objslayout = new QVBoxLayout(verticalLayoutWidget_3);
        objslayout->setObjectName(QStringLiteral("objslayout"));
        objslayout->setContentsMargins(0, 0, 0, 0);
        objTest = new ThreeDObjectGUI(verticalLayoutWidget_3);
        objTest->setObjectName(QStringLiteral("objTest"));
        sizePolicy2.setHeightForWidth(objTest->sizePolicy().hasHeightForWidth());
        objTest->setSizePolicy(sizePolicy2);
        objTest->setMinimumSize(QSize(430, 210));

        objslayout->addWidget(objTest);

        materialTest = new ScatteringMaterialGUI(verticalLayoutWidget_3);
        materialTest->setObjectName(QStringLiteral("materialTest"));
        materialTest->setMinimumSize(QSize(0, 150));

        objslayout->addWidget(materialTest);

        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        objslayout->addItem(verticalSpacer_3);

        Objects->addTab(Objects_2, QString());
        tab = new QWidget();
        tab->setObjectName(QStringLiteral("tab"));
        verticalLayoutWidget_4 = new QWidget(tab);
        verticalLayoutWidget_4->setObjectName(QStringLiteral("verticalLayoutWidget_4"));
        verticalLayoutWidget_4->setGeometry(QRect(0, 0, 421, 671));
        verticalLayout_3 = new QVBoxLayout(verticalLayoutWidget_4);
        verticalLayout_3->setSpacing(0);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(6, 0, 0, 0);
        radius = new SliderLabel(verticalLayoutWidget_4);
        radius->setObjectName(QStringLiteral("radius"));
        radius->setEnabled(true);
        radius->setMinimumSize(QSize(0, 30));

        verticalLayout_3->addWidget(radius);

        samples = new SliderLabel(verticalLayoutWidget_4);
        samples->setObjectName(QStringLiteral("samples"));
        samples->setEnabled(true);
        samples->setMinimumSize(QSize(0, 30));

        verticalLayout_3->addWidget(samples);

        epsilon_gbuffer = new SliderLabel(verticalLayoutWidget_4);
        epsilon_gbuffer->setObjectName(QStringLiteral("epsilon_gbuffer"));
        epsilon_gbuffer->setEnabled(true);
        epsilon_gbuffer->setMinimumSize(QSize(0, 30));

        verticalLayout_3->addWidget(epsilon_gbuffer);

        epsilon_combination = new SliderLabel(verticalLayoutWidget_4);
        epsilon_combination->setObjectName(QStringLiteral("epsilon_combination"));
        epsilon_combination->setEnabled(true);
        epsilon_combination->setMinimumSize(QSize(0, 30));

        verticalLayout_3->addWidget(epsilon_combination);

        shadow_bias = new SliderLabel(verticalLayoutWidget_4);
        shadow_bias->setObjectName(QStringLiteral("shadow_bias"));
        shadow_bias->setEnabled(true);
        shadow_bias->setMinimumSize(QSize(0, 30));

        verticalLayout_3->addWidget(shadow_bias);

        lod = new SliderLabel(verticalLayoutWidget_4);
        lod->setObjectName(QStringLiteral("lod"));
        lod->setEnabled(true);
        lod->setMinimumSize(QSize(0, 30));

        verticalLayout_3->addWidget(lod);

        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        cubemapVisible = new QCheckBox(verticalLayoutWidget_4);
        cubemapVisible->setObjectName(QStringLiteral("cubemapVisible"));

        gridLayout->addWidget(cubemapVisible, 2, 0, 1, 1);

        label = new QLabel(verticalLayoutWidget_4);
        label->setObjectName(QStringLiteral("label"));

        gridLayout->addWidget(label, 3, 0, 1, 1);

        minusZcheck = new QCheckBox(verticalLayoutWidget_4);
        minusZcheck->setObjectName(QStringLiteral("minusZcheck"));
        minusZcheck->setChecked(true);

        gridLayout->addWidget(minusZcheck, 9, 0, 1, 1);

        minusYcheck = new QCheckBox(verticalLayoutWidget_4);
        minusYcheck->setObjectName(QStringLiteral("minusYcheck"));
        minusYcheck->setChecked(true);

        gridLayout->addWidget(minusYcheck, 7, 0, 1, 1);

        plusZcheck = new QCheckBox(verticalLayoutWidget_4);
        plusZcheck->setObjectName(QStringLiteral("plusZcheck"));
        plusZcheck->setChecked(true);

        gridLayout->addWidget(plusZcheck, 8, 0, 1, 1);

        plusYcheck = new QCheckBox(verticalLayoutWidget_4);
        plusYcheck->setObjectName(QStringLiteral("plusYcheck"));
        plusYcheck->setChecked(true);

        gridLayout->addWidget(plusYcheck, 6, 0, 1, 1);

        minusXcheck = new QCheckBox(verticalLayoutWidget_4);
        minusXcheck->setObjectName(QStringLiteral("minusXcheck"));
        minusXcheck->setChecked(true);

        gridLayout->addWidget(minusXcheck, 5, 0, 1, 1);

        plusXcheck = new QCheckBox(verticalLayoutWidget_4);
        plusXcheck->setObjectName(QStringLiteral("plusXcheck"));
        plusXcheck->setChecked(true);

        gridLayout->addWidget(plusXcheck, 4, 0, 1, 1);


        verticalLayout_3->addLayout(gridLayout);

        verticalSpacer_4 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_3->addItem(verticalSpacer_4);

        Objects->addTab(tab, QString());

        rightLayout->addWidget(Objects);


        mainLayout->addLayout(rightLayout);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 1143, 21));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        menuObjects = new QMenu(menubar);
        menuObjects->setObjectName(QStringLiteral("menuObjects"));
        menuAdd = new QMenu(menuObjects);
        menuAdd->setObjectName(QStringLiteral("menuAdd"));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        MainWindow->setStatusBar(statusbar);

        menubar->addAction(menuFile->menuAction());
        menubar->addAction(menuObjects->menuAction());
        menuFile->addAction(actionSave_Image);
        menuFile->addSeparator();
        menuFile->addAction(actionQuit);
        menuObjects->addAction(menuAdd->menuAction());
        menuAdd->addAction(actionCube);
        menuAdd->addAction(actionSphere);
        menuAdd->addAction(actionPlane);
        menuAdd->addSeparator();
        menuAdd->addAction(actionBunny);

        retranslateUi(MainWindow);

        Objects->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0));
        actionSave_Image->setText(QApplication::translate("MainWindow", "Save Screenshot...", 0));
        actionSave_Image->setShortcut(QApplication::translate("MainWindow", "Ctrl+S", 0));
        actionQuit->setText(QApplication::translate("MainWindow", "Quit", 0));
        actionQuit->setShortcut(QApplication::translate("MainWindow", "Esc", 0));
        actionCube->setText(QApplication::translate("MainWindow", "Cube", 0));
        actionSphere->setText(QApplication::translate("MainWindow", "Sphere", 0));
        actionBunny->setText(QApplication::translate("MainWindow", "Model", 0));
        actionCow->setText(QApplication::translate("MainWindow", "Cow", 0));
        actionPlane->setText(QApplication::translate("MainWindow", "Plane", 0));
        groupBox->setTitle(QApplication::translate("MainWindow", "General properties", 0));
        intlable->setText(QApplication::translate("MainWindow", "Light intensity:", 0));
        intensitylabel->setText(QApplication::translate("MainWindow", "0", 0));
        showaxes->setText(QApplication::translate("MainWindow", "Show Axes", 0));
        showgrid->setText(QApplication::translate("MainWindow", "Show Grid", 0));
        statistics->setTitle(QApplication::translate("MainWindow", "Statistics", 0));
        fpslabel->setText(QApplication::translate("MainWindow", "<html><head/><body><p><span style=\" color:#000000;\">Median time frame (ms): - </span></p></body></html>", 0));
        vertexpixel->setText(QApplication::translate("MainWindow", "Vertex Mode", 0));
        label_2->setText(QApplication::translate("MainWindow", "Dipole:", 0));
        jensenbutton->setText(QApplication::translate("MainWindow", "Jensen", 0));
        directionalbutton->setText(QApplication::translate("MainWindow", "Directional", 0));
        Objects->setTabText(Objects->indexOf(General), QApplication::translate("MainWindow", "General", 0));
        Objects->setTabText(Objects->indexOf(Objects_2), QApplication::translate("MainWindow", "Objects", 0));
        cubemapVisible->setText(QApplication::translate("MainWindow", "Enable cubemap", 0));
        label->setText(QApplication::translate("MainWindow", "Enable cubemap faces:", 0));
        minusZcheck->setText(QApplication::translate("MainWindow", "-Z", 0));
        minusYcheck->setText(QApplication::translate("MainWindow", "-Y", 0));
        plusZcheck->setText(QApplication::translate("MainWindow", "+Z", 0));
        plusYcheck->setText(QApplication::translate("MainWindow", "+Y", 0));
        minusXcheck->setText(QApplication::translate("MainWindow", "-X", 0));
        plusXcheck->setText(QApplication::translate("MainWindow", "+X", 0));
        Objects->setTabText(Objects->indexOf(tab), QApplication::translate("MainWindow", "Lights", 0));
        menuFile->setTitle(QApplication::translate("MainWindow", "File", 0));
        menuObjects->setTitle(QApplication::translate("MainWindow", "Objects", 0));
        menuAdd->setTitle(QApplication::translate("MainWindow", "Add...", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
