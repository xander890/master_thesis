#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFileDialog>

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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
