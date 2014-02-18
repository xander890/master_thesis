#ifndef __TERRAIN_SCENE_H
#define __TERRAIN_SCENE_H

#include <GLGraphics/GLHeader.h>
#include <GLGraphics/ThreeDObject.h>
#include <GLGraphics/ShaderProgram.h>
#include "detector.h"

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 400;

class TerrainScene : public QGLWidget
{
    Q_OBJECT

    QTimer *timer;

    float ax, ay, dist;
    float ang_x;
    float ang_y;
    int mouse_x0,mouse_y0;

    GLGraphics::ShaderProgramDraw shader;
    GLGraphics::ThreeDObject cow;
    GLuint m_vertexBuffer;

    void set_light_and_camera(GLGraphics::ShaderProgramDraw&);

public:
    TerrainScene( const QGLFormat& format, QWidget* parent = 0);

public slots:
    void animate();

    void step(int value);
    void activityChanged(int value);
    void accelerationChanged(QVec3f f);
protected:
     void initializeGL();
     void resizeGL( int w, int h );
     void paintGL();
     void mousePressEvent(QMouseEvent *);
     void mouseReleaseEvent(QMouseEvent *);
     void mouseMoveEvent(QMouseEvent *);
     void keyPressEvent(QKeyEvent *);
     void keyReleaseEvent(QKeyEvent *);
private:
     QVec3f accel;
     Detector detector;
     int activity;
};


#endif // INTROSCENE_H
