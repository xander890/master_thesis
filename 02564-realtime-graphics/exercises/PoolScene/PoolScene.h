#ifndef __TERRAIN_SCENE_H
#define __TERRAIN_SCENE_H

#include <GLGraphics/GLHeader.h>
#include <GLGraphics/ThreeDObject.h>
#include <GLGraphics/ShaderProgram.h>
#include <GLGraphics/User.h>


extern bool reload_shaders;
extern GLGraphics::User user;

class PoolScene : public QGLWidget
{
    Q_OBJECT

    QTimer *timer;

    float ax, ay, dist;
    float ang_x;
    float ang_y;
    int mouse_x0,mouse_y0;
    int window_width;
    int window_height;

    GLGraphics::ShaderProgramDraw shader;
    GLGraphics::ThreeDObject cow;
    GLuint m_vertexBuffer;

    //void set_light_and_camera(GLGraphics::ShaderProgramDraw&);

public:
    PoolScene( const QGLFormat& format, QWidget* parent = 0);

public slots:
    void animate();

protected:
     void set_light_and_camera(GLGraphics::ShaderProgramDraw& shader_prog, bool is_reflected=false);
     void draw_objects();
     void draw_room(bool is_reflected=false);
     void draw_reflection();
     void draw_particles();
     void initializeGL();
     void resizeGL( int w, int h );
     void paintGL();
     void mousePressEvent(QMouseEvent *);
     void mouseReleaseEvent(QMouseEvent *);
     void mouseMoveEvent(QMouseEvent *);
     void keyPressEvent(QKeyEvent *);
     void keyReleaseEvent(QKeyEvent *);
};


#endif // INTROSCENE_H
