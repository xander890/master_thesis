#ifndef __TERRAIN_SCENE_H
#define __TERRAIN_SCENE_H

#include <GLGraphics/GLHeader.h>
#include <GLGraphics/ThreeDObject.h>
#include <GLGraphics/ShaderProgram.h>
#include "GBuffer.h"
#include <QGLFormat>

class TranslucentParameters
{

public:
    TranslucentParameters::TranslucentParameters() : circleradius(0.05f), samples(150) {}
    float circleradius;
    int samples;
};

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

    bool isVertexMode;
    bool isShadow;
    bool isGridVisible;
    bool areAxesVisible;

    GLGraphics::ShaderProgramDraw shader;
    GLGraphics::ThreeDObject cow;
    GLuint m_vertexBuffer;
    CGLA::Vec4f clearColor;
    TranslucentParameters * params;

public:

    TranslucentMaterials(QWidget* parent = 0);
    QImage* takeScreenshot();
    CGLA::Vec4f getClearColor() const;
    void keyPressEvent(QKeyEvent *);
    void keyReleaseEvent(QKeyEvent *);
    GLGraphics::ThreeDObject * getDefaultObject();
    GLGraphics::ThreeDObject * getObject(std::string name);
    TranslucentParameters * getParameters();

public slots:
    void animate();
    void setClearColor(CGLA::Vec4f & color);
    void setUserPosition(CGLA::Vec3f & position);
    void setUserDirection(CGLA::Vec3f & direction);
    void setLightIntensity(float intensity);
    void setShadows(bool shadows);
    void setVertexPixelMode(bool isVertex);
    void setGridVisible(bool isVisible);
    void setAxesVisible(bool areVisible);
    void addObject(GLGraphics::ThreeDObject * obj);

signals:
    void userPositionChanged(CGLA::Vec3f & newPosition);
    void userDirectionChanged(CGLA::Vec3f & newDirection);
    void timeMeasurement(int millis);

protected:
    void set_light_and_camera(GLGraphics::ShaderProgramDraw& shader_prog);
    void render_jensen(bool reload);
    void render_better_dipole(bool reload);
    void render_direct_test(bool reload);
    void render_to_gbuffer(GBuffer& gbuffer, bool reload);
    void render_directional_dipole(bool reload);
    void render_deferred_ssao(bool reload);
    void draw_objects(GLGraphics::ShaderProgramDraw& shader_prog);
    void draw_objects(GLGraphics::ShaderProgramDraw& shader_prog,std::vector<std::string> & str);
    void setup_shadow(bool reload);
    void draw_axes(bool reload);
    void draw_grid(bool reload);
    void draw_bounding_boxes(bool reload);

#ifdef SOLUTION_CODE
    void render_indirect();
#endif
     void initializeGL();
     void resizeGL( int w, int h );
     void paintGL();
     void mousePressEvent(QMouseEvent *);
     void mouseReleaseEvent(QMouseEvent *);
     void mouseMoveEvent(QMouseEvent *);

     std::vector<GLGraphics::ThreeDObject*> objects;
};

struct CompareThreeD {
    CompareThreeD( std::string s ) : toFind(s) { }

    bool operator() (GLGraphics::ThreeDObject *n)
        { return strcmp(n->name.c_str(),toFind.c_str()) == 0; }

    std::string toFind;
};

#endif // INTROSCENE_H
