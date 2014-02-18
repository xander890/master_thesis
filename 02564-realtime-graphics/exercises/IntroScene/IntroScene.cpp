/*
 *  main.cpp - 02561
 *
 *  User interaction:
 *     Mouse rotates,
 *     '+-' zooms in and out,
 *     [ESC] quits,
 *     'r' reloads shaders.
 *
 */

#include <GLGraphics/GLHeader.h>
#include <iostream>
#include <string>
#include <QApplication>
#include <QtGui>
#include <QString>

#include <CGLA/Mat4x4f.h>
#include <CGLA/Vec3f.h>
#include <CGLA/Quatf.h>
#include "introscene.h"


using namespace std;
using namespace CGLA;
using namespace GLGraphics;

const string shader_path = "./shaders/IntroScene/";
const string objects_path = "./data/";

IntroScene::IntroScene( const QGLFormat& format, QWidget* parent)
    : QGLWidget( new Core3_2_context(format), (QWidget*) parent),
      dist(12),ang_x(0),ang_y(0)
{
}

void IntroScene::initializeGL()
{
    setup_gl();
    shader.init(shader_path, "object.vert", "", "object.frag");
    cow.init(objects_path+"cow.obj");
    glClearColor( 0.7f, 0.7f, 0.7f, 0.0f );
    glEnable(GL_DEPTH_TEST);
}

void IntroScene::set_light_and_camera(ShaderProgramDraw& shader_prog)
{
    static const Vec4f light_specular(0.6,0.6,0.3,0.6);
    static const Vec4f light_diffuse(0.6,0.6,0.3,0.6);
    static const Vec4f light_ambient(0.3,0.4,0.6,0.4);
    
    Mat4x4f V =
            translation_Mat4x4f(Vec3f(0,0,- dist))
            *rotation_Mat4x4f(XAXIS, ay)
            *rotation_Mat4x4f(YAXIS, ax);
    
    shader_prog.set_projection_matrix(perspective_Mat4x4f(55, float(WINDOW_WIDTH)/WINDOW_HEIGHT, 1, 100));
    shader_prog.set_light_position(Vec4f(0,0,1,0));
    shader_prog.set_view_matrix(V);
    shader_prog.set_light_intensities(light_diffuse, light_specular, light_ambient);
}

void IntroScene::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    shader.use();
    set_light_and_camera(shader);
    cow.display(shader);
    check_gl_error();
}

void IntroScene::resizeGL( int w, int h )
{
    // Set the viewport to window dimensions
    glViewport( 0, 0, w, h );
}

void IntroScene::mousePressEvent(QMouseEvent *m)
{
    mouse_x0 = m->x();
    mouse_y0 = m->y();
}

void IntroScene::mouseReleaseEvent(QMouseEvent *m)
{
    float delta_ang_x = 4*M_PI*(mouse_x0-m->x())/WINDOW_WIDTH;
    float delta_ang_y = M_PI*(mouse_y0-m->y())/WINDOW_HEIGHT;
    ang_x += delta_ang_x;
    ang_y += delta_ang_y;
}

void IntroScene::mouseMoveEvent(QMouseEvent *m)
{
    float delta_ang_x = 4*M_PI*(mouse_x0-m->x())/WINDOW_WIDTH;
    float delta_ang_y = M_PI*(mouse_y0-m->y())/WINDOW_HEIGHT;
    ax = ang_x + delta_ang_x;
    ay = ang_y + delta_ang_y;
    QWidget::repaint();
}

void IntroScene::keyPressEvent(QKeyEvent *e)
{
    switch(e->key())
    {
    case Qt::Key_Escape: exit(0);
    case '+': dist /= 1.1; break;
    case '-': dist *= 1.1; break;
    default:
        QWidget::keyPressEvent(e);
    }
    QWidget::repaint();
}

