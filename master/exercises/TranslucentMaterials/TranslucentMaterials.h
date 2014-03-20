#ifndef __TERRAIN_SCENE_H
#define __TERRAIN_SCENE_H

#include <GLGraphics/GLHeader.h>
#include <GLGraphics/ThreeDObject.h>
#include <GLGraphics/ShaderProgram.h>
#include "GBuffer.h"
#include <QGLFormat>

class TranslucentMaterials : public QGLWidget
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
    CGLA::Vec4f clearColor;

public:

    TranslucentMaterials(QWidget* parent = 0);
    QImage* takeScreenshot();
    CGLA::Vec4f getClearColor() const;

public slots:
    void animate();
    void setClearColor(CGLA::Vec4f & color);
    void setUserPosition(CGLA::Vec3f & position);
    void setUserDirection(CGLA::Vec3f & direction);
    void setLightIntensity(float intensity);

signals:
    void userPositionChanged(CGLA::Vec3f & newPosition);
    void userDirectionChanged(CGLA::Vec3f & newDirection);

protected:
    void set_light_and_camera(GLGraphics::ShaderProgramDraw& shader_prog);
    void render_direct(bool reload);
    void render_direct_wireframe(bool reload);
    void render_direct_fur(bool reload);
    void render_to_gbuffer(GBuffer& gbuffer, bool reload);
    void render_deferred_toon(bool reload);
    void render_deferred_ssao(bool reload);
    void draw_objects(GLGraphics::ShaderProgramDraw& shader_prog);
    void draw_objects(GLGraphics::ShaderProgramDraw& shader_prog,std::vector<std::string> & str);

#ifdef SOLUTION_CODE
    void render_indirect();
#endif
     void initializeGL();
     void resizeGL( int w, int h );
     void paintGL();
     void mousePressEvent(QMouseEvent *);
     void mouseReleaseEvent(QMouseEvent *);
     void mouseMoveEvent(QMouseEvent *);
     void keyPressEvent(QKeyEvent *);
     void keyReleaseEvent(QKeyEvent *);
     std::vector<GLGraphics::ThreeDObject*> objects;
};

struct CompareThreeD {
    CompareThreeD( std::string s ) : toFind(s) { }

    bool operator() (GLGraphics::ThreeDObject *n)
        { return strcmp(n->name.c_str(),toFind.c_str()) == 0; }

    std::string toFind;
};

#endif // INTROSCENE_H
