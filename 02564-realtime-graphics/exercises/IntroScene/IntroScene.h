#ifndef INTROSCENE_H
#define INTROSCENE_H

#include <GLGraphics/GLHeader.h>
#include <GLGraphics/ThreeDObject.h>
#include <GLGraphics/ShaderProgram.h>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;

class IntroScene : public QGLWidget
{
    Q_OBJECT

    float ax, ay, dist;
    float ang_x;
    float ang_y;
    int mouse_x0,mouse_y0;

    GLGraphics::ShaderProgramDraw shader;
    GLGraphics::ThreeDObject cow;
    GLuint m_vertexBuffer;

    void set_light_and_camera(GLGraphics::ShaderProgramDraw&);

    public:
    IntroScene( const QGLFormat& format, QWidget* parent = 0);


protected:
     void initializeGL();
     void resizeGL( int w, int h );
     void paintGL();
     void mousePressEvent(QMouseEvent *);
     void mouseReleaseEvent(QMouseEvent *);
     void mouseMoveEvent(QMouseEvent *);
     void keyPressEvent(QKeyEvent *);
};


#endif // INTROSCENE_H
