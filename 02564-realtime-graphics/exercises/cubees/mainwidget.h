#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QtOpenGL/QGLWidget>

#include <QMatrix4x4>
#include <QQuaternion>
#include <QVector2D>
#include <CGLA/Vec3f.h>
#include <CGLA/Vec2f.h>
#include <CGLA/Quatf.h>
#include <CGLA/Mat4x4f.h>
#include "sensors.h"
#include <Mesh/TriangleMesh.h>

class QBasicTimer;
class QGLShaderProgram;

class GeometryEngine;

class MainWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit MainWidget(QWidget *parent = 0);
    virtual ~MainWidget();

signals:

public slots:
    void updateSensors();

protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void timerEvent(QTimerEvent *e);

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

    void initTextures();

private:
    QBasicTimer *timer;
    Mesh::TriangleMesh cube;

    GLuint texture;

    CGLA::Mat4x4f projection;


    CGLA::Vec2f mousePressPosition;
    CGLA::Vec3f rotationAxis;
    float angularSpeed;
    CGLA::Quatf rotation;
    CGLA::Mat4x4f modelMatrix;
    CGLA::Mat4x4f rot_mat;
    CGLA::Vec3f touch_acc;

    Sensors* sensors;

    double lightValue;
    double x;
    double y;
    double z;
    double azimuth;

};

#endif // MAINWIDGET_H
