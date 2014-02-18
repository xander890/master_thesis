/*
 *  main.cpp
 *  Pool Scene, 02564 Framework
 *
 *  Created by J. Andreas Bærentzen on 01/02/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include <QApplication>
#include <QDebug>
#include <iostream>

#include <CGLA/Vec2f.h>
#include <CGLA/Vec3f.h>
#include <CGLA/Vec4f.h>
#include <GLGraphics/GLHeader.h>
#include <GLGraphics/ShaderProgram.h>
#include <GLGraphics/ThreeDObject.h>
#include <GLGraphics/User.h>
#include <Mesh/Texmap.h>
#include "PoolScene.h"
#include "Mesh/TriangleMesh.h"
#include "GLGraphics/ResourceLoader.h"

using namespace std;
using namespace CGLA;
using namespace GLGraphics;
using namespace Mesh;


int main(int argc, char *argv[])
{


    const int WINDOW_WIDTH = 1136;
    const int WINDOW_HEIGHT = 640;

    qDebug() << argv[0] << " - " << argv[1] << endl;
    QApplication a( argc, argv );


    QGLFormat glFormat;
    glFormat.setVersion( 3, 2 );
    glFormat.setProfile( QGLFormat::CoreProfile ); // Requires >=Qt-4.8.0
    PoolScene w( glFormat);
    w.resize(WINDOW_WIDTH, WINDOW_HEIGHT);
    w.setFocusPolicy(Qt::ClickFocus);
    w.show();

    return a.exec();
}
