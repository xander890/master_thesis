#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFileDialog>
#include "vectorform.h"
#include "colorform.h"

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
