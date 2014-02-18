#include <QApplication>
#include <QDebug>
#include "IntroScene.h"

int main(int argc, char *argv[])
{
    QApplication a( argc, argv );
    a.setApplicationName("cube");
    a.setApplicationVersion("0.1");

    IntroScene w;
    w.show();

    return a.exec();
}
