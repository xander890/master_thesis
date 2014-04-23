#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFileDialog>
#include "vectorform.h"
#include "colorform.h"
#include "sliderlabel.h"

using namespace std;
using namespace CGLA;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->clearColor->init("Clear color: ", ui->translucentMaterials->getClearColor());
    connect(ui->clearColor,SIGNAL(vectorChanged(CGLA::Vec4f&)),ui->translucentMaterials,SLOT(setClearColor(CGLA::Vec4f&)));

    Vec3f userpos = Vec3f(0, -12, 2.5f);
    ui->userPosition->init("User position: ", userpos);
    ui->translucentMaterials->setUserPosition(userpos);
    connect(ui->userPosition,SIGNAL(vectorChanged(CGLA::Vec3f&)),ui->translucentMaterials,SLOT(setUserPosition(CGLA::Vec3f&)));
    connect(ui->translucentMaterials,SIGNAL(userPositionChanged(CGLA::Vec3f&)),ui->userPosition,SLOT(setValue(CGLA::Vec3f&)));

    Vec3f userdir = Vec3f(0,1,-0.3f);
    ui->userDirection->init("User direction: ", userdir);
    ui->translucentMaterials->setUserDirection(userdir);
    connect(ui->userDirection, SIGNAL(vectorChanged(CGLA::Vec3f&)),ui->translucentMaterials,SLOT(setUserDirection(CGLA::Vec3f&)));
    connect(ui->translucentMaterials,SIGNAL(userDirectionChanged(CGLA::Vec3f&)),ui->userDirection,SLOT(setValue(CGLA::Vec3f&)));

    ui->translucentMaterials->setLightIntensity(12.0f);
    ui->intensity->setValue(12);

    connect(ui->vertexpixel, SIGNAL(toggled(bool)), ui->translucentMaterials, SLOT(setVertexPixelMode(bool)));
    ui->vertexpixel->setChecked(true);

    connect(ui->showgrid, SIGNAL(toggled(bool)), ui->translucentMaterials, SLOT(setGridVisible(bool)));
    ui->showgrid->setChecked(true);

    connect(ui->showaxes, SIGNAL(toggled(bool)), ui->translucentMaterials, SLOT(setAxesVisible(bool)));
    ui->showaxes->setChecked(true);

    connect(ui->translucentMaterials, SIGNAL(timeMeasurement(int)), this, SLOT(timeMeasurement(int)));
    ui->fpslabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

    GLGraphics::ThreeDObject * bunny = ui->translucentMaterials->getDefaultObject();
    ui->objTest->setObject(bunny);
    ui->translucentMaterials->addObject(bunny);

    ui->radius->init("Radius: ", ui->translucentMaterials->getParameters()->circleradius, 0.001, 1.0, false);
    connect(ui->radius, SIGNAL(valueChanged(float)), this, SLOT(radiusChanged(float)));

    ui->samples->init("Samples: ",ui->translucentMaterials->getParameters()->samples,1,1000, true);
    connect(ui->samples, SIGNAL(valueChanged(float)), this, SLOT(samplesChanged(float)));

    ui->epsilon_combination->init("Comb. offset: ", ui->translucentMaterials->getParameters()->epsilon_combination, 0.0, 0.1, false);
    connect(ui->epsilon_combination,SIGNAL(valueChanged(float)), this, SLOT(epsilonCombinationChanged(float)));

    ui->epsilon_gbuffer->init("Gbuffer samp. offset: ", ui->translucentMaterials->getParameters()->epsilon_gbuffer, 0.0, 0.1, false);
    connect(ui->epsilon_gbuffer,SIGNAL(valueChanged(float)), this, SLOT(epsilonGBufferChanged(float)));

    ui->shadow_bias->init("Shadow bias: ", ui->translucentMaterials->getParameters()->shadow_bias, 0.0, 1.0, false);
    connect(ui->shadow_bias, SIGNAL(valueChanged(float)), this, SLOT(shadowBiasChanged(float)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

int main(int argc, char *argv[])
{

    qDebug() << argv[0] << " - " << argv[1] << endl;
    QApplication a( argc, argv );

    MainWindow w;
    w.setFocusPolicy(Qt::ClickFocus);
    w.setWindowTitle("Translucent Material Demo v0.1");
    w.show();

    return a.exec();
}

void MainWindow::on_actionSave_Image_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Image"), QString(),
                tr("PNG files (*.png);;JPEG Files (*.jpg *.jpeg)"));

    if (!fileName.isEmpty()) {

        QImage * image = ui->translucentMaterials->takeScreenshot();
        image->save(fileName);

    }

}

void MainWindow::on_actionQuit_triggered()
{
    qApp->quit();
}

void MainWindow::on_intensity_valueChanged(int value)
{
    float intensity = (float)(value);
    ui->intensitylabel->setText(QString::number(intensity,'f',1));
    ui->translucentMaterials->setLightIntensity(intensity);
}

void MainWindow::timeMeasurement(int millis)
{
   ui->fpslabel->setText(QString("Median time frame (ms): ").append(QString::number(millis)));
}

void MainWindow::keyPressEvent(QKeyEvent * e)
{
    ui->translucentMaterials->keyPressEvent(e);
    QMainWindow::keyPressEvent(e);
}

void MainWindow::keyReleaseEvent(QKeyEvent * e)
{
    ui->translucentMaterials->keyReleaseEvent(e);
    QMainWindow::keyReleaseEvent(e);
}

void MainWindow::radiusChanged(float value)
{

    ui->translucentMaterials->getParameters()->circleradius = value;
}

void MainWindow::samplesChanged(float value)
{

    ui->translucentMaterials->getParameters()->samples = (int) value;
}

void MainWindow::epsilonGBufferChanged(float value)
{
    ui->translucentMaterials->getParameters()->epsilon_gbuffer = value;
}

void MainWindow::epsilonCombinationChanged(float value)
{
    ui->translucentMaterials->getParameters()->epsilon_combination = value;
}

void MainWindow::shadowBiasChanged(float value)
{
    ui->translucentMaterials->getParameters()->shadow_bias = value;
}
