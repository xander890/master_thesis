#include <QApplication>
#include <QDebug>
#include "IntroScene.h"

#include <iostream>

int main(int argc, char *argv[])
{
    QApplication a( argc, argv );
    QGLFormat glFormat;
    glFormat.setVersion( 3, 2 );
    glFormat.setProfile( QGLFormat::CoreProfile ); // Requires >=Qt-4.8.0
    IntroScene w( glFormat);
    w.resize(WINDOW_WIDTH, WINDOW_HEIGHT);
    w.setFocusPolicy(Qt::ClickFocus);
    w.show();
    return a.exec();
}

