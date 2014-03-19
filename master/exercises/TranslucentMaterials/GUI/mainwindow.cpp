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
    ui->clearColor->init("Clear color", ui->translucentMaterials->getClearColor());
    connect(ui->clearColor,SIGNAL(vectorChanged(CGLA::Vec4f&)),ui->translucentMaterials,SLOT(setClearColor(CGLA::Vec4f&)));
    ui->userPosition->init("User position", Vec3f(0.0f,0.0f,0.0f));
    connect(ui->userPosition,SIGNAL(vectorChanged(CGLA::Vec3f&)),ui->translucentMaterials,SLOT(setUserPosition(CGLA::Vec3f&)));
    connect(ui->translucentMaterials,SIGNAL(userPosition(CGLA::Vec3f&)),ui->userPosition,SLOT(setValue(CGLA::Vec3f&)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

int main(int argc, char *argv[])
{

    const int WINDOW_WIDTH = 1280;
    const int WINDOW_HEIGHT = 800;

    qDebug() << argv[0] << " - " << argv[1] << endl;
    QApplication a( argc, argv );

    MainWindow w;
    w.resize(WINDOW_WIDTH, WINDOW_HEIGHT);
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
