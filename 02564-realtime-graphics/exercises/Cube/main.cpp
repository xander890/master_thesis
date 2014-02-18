#include <QApplication>
#include <QLabel>
#include <qplatformdefs.h>

#ifndef QT_NO_OPENGL
#include "mainwidget.h"
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("cube");
    a.setApplicationVersion("0.1");
#ifndef QT_NO_OPENGL
    MainWidget w;
    w.resize(800, 480);
#if defined(MEEGO_EDITION_HARMATTAN)
    w.showFullScreen();
#else
    w.show();
#endif

#else
    QLabel * notifyLabel = new QLabel("OpenGL Support required");
    notifyLabel->show();
#endif
    return a.exec();
}
