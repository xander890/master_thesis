#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QtOpenGL/QGLWidget>
#include <GLGraphics/GLHeader.h>
#include <GLGraphics/ThreeDObject.h>
#include <GLGraphics/ShaderProgram.h>

class IntroScene : public QGLWidget
 {
     Q_OBJECT


    float ax, ay, dist;
    float ang_x;
    float ang_y;
    float widgetWidth, widgetHeight;
    int mouse_x0,mouse_y0;

    GLGraphics::ShaderProgramDraw shader;
    GLGraphics::ThreeDObject cow;
    GLuint m_vertexBuffer;

    void set_light_and_camera(GLGraphics::ShaderProgramDraw&);
 public:
     explicit IntroScene(QWidget *parent = 0);
     virtual ~IntroScene();

protected:
     void initializeGL();
     void resizeGL( int w, int h );
     void paintGL();
     void mousePressEvent(QMouseEvent *);
     void mouseReleaseEvent(QMouseEvent *);
     void mouseMoveEvent(QMouseEvent *);
     void keyPressEvent(QKeyEvent *);

 private:
 };

#endif // MAINWIDGET_H
