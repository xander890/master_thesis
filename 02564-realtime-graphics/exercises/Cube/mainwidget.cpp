#include "mainwidget.h"
#include "geometryengine.h"
#include <QtOpenGL/QGLShaderProgram>
#include <QBasicTimer>
#include <QMouseEvent>
#include <math.h>
#include <QDebug>

MainWidget::MainWidget(QWidget *parent) :
    QGLWidget(parent),
    timer(new QBasicTimer),
    geometries(new GeometryEngine)

{
    sensors = new Sensors();
    connect(sensors,SIGNAL(update()),this,SLOT(updateSensors()));
    updateSensors();
}

void MainWidget::updateSensors()
{

    //Those are just simple examples of handling reading from the sensors.

    //Light sensor
    lightValue = std::min(sensors->light(), 255.0);


    //Accelerometer
    x = sensors->accX();
    y = sensors->accY();
    z = sensors->accZ();

    QVector3D n = QVector3D(x, y, 0.0).normalized();
    qreal acc = sqrt(pow(x,2)+(y,2))/100;
    rotationAxis = (rotationAxis * angularSpeed + n*acc).normalized();
    angularSpeed += acc;

    //Compass
    azimuth = sensors->azimuth();

}

MainWidget::~MainWidget()
{
    delete timer; timer = 0;
    delete program; program = 0;
    delete geometries; geometries = 0;

    deleteTexture(texture);
}

//! [0]
void MainWidget::mousePressEvent(QMouseEvent *e)
{
    // Saving mouse press position
    mousePressPosition = QVector2D(e->posF());
}

void MainWidget::mouseReleaseEvent(QMouseEvent *e)
{
    // Mouse release position - mouse press position
    QVector2D diff = QVector2D(e->posF()) - mousePressPosition;

    // Rotation axis is perpendicular to the mouse position difference 
    // vector
    QVector3D n = QVector3D(diff.y(), diff.x(), 0.0).normalized();

    touch_acc = QVector3D(diff.x(), -diff.y(), 0.0);
    // Accelerate angular speed relative to the length of the mouse sweep
    qreal acc = diff.length() / 100.0;

    // Calculate new rotation axis as weighted sum
    rotationAxis = (rotationAxis * angularSpeed + n * acc).normalized();

    // Increase angular speed
    angularSpeed += acc;
}
//! [0]

//! [1]
void MainWidget::timerEvent(QTimerEvent *e)
{
    Q_UNUSED(e);
    QVector3D acc(sensors->accY(), -sensors->accX(), -sensors->accZ());

    static QVector3D pos(0,0,0);
    static QVector3D pos_old(0,0,0);

    QVector3D pos_new = 2.0 * pos - pos_old + 0.0025 * (acc + touch_acc);
    touch_acc = QVector3D(0,0,0);

    pos_old = pos;
    pos = pos_new;

    float WALL = 9;
    float WALLX = 15;
    if(pos.x() < - WALLX) pos.setX(-WALLX);
    if(pos.x() > WALLX) pos.setX(WALLX);
    if(pos.y() < - WALL) pos.setY(-WALL);
    if(pos.y() > WALL) pos.setY(WALL);
    if(pos.z() < - WALL) pos.setZ(-WALL);
    if(pos.z() > WALL) pos.setZ(WALL);

    xform.setToIdentity();
    xform.translate(pos.x(), pos.y(), pos.z());

    float speed = (pos-pos_old).length();

    QVector3D rot_axis = QVector3D::crossProduct(pos-pos_old,QVector3D(0,0,-1)).normalized();

    QMatrix4x4 new_mat;
    new_mat.setToIdentity();
    new_mat.rotate(speed*10, rot_axis);
    rot_mat = new_mat * rot_mat;
    xform *= rot_mat;

    program->setUniformValue("light_intensity", GLfloat(lightValue/255.0f));

        // Update scene
        updateGL();
}
//! [1]

void MainWidget::initializeGL()
{
    qglClearColor(Qt::black);

    qDebug() << "Initializing shaders...";
    initShaders();

    qDebug() << "Initializing textures...";
    initTextures();

//! [2]
    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // Enable back face culling
    glEnable(GL_CULL_FACE);
//! [2]

    rot_mat.setToIdentity();

    qDebug() << "Initializing geometries...";
    geometries->init();

    // using QBasicTimer because its faster that QTimer
    timer->start(12, this);
}

//! [3]
void MainWidget::initShaders()
{

    program = new QGLShaderProgram(QGLContext::currentContext());
    setlocale(LC_NUMERIC, "C");

    QGLShader* shaderF = new QGLShader(QGLShader::Fragment,QGLContext::currentContext());
    if(!shaderF->compileSourceFile(":/fshader.glsl"))
    {
	qDebug() << "Could not load shader file " << program->log();
    }
    else
    {
	qDebug() << "Loaded fshader successfully";
	program->addShader(shaderF);

    }

    QGLShader* shaderV = new QGLShader(QGLShader::Vertex, QGLContext::currentContext());

    if(!shaderV->compileSourceFile(":/vshader.glsl"))
    {
	qDebug() << "Could not load shader file "<< program->log();

    }
    else
    {
	qDebug() << "Loaded vsahder successfully";
	program->addShader(shaderV);

    }

    // Linking shader pipeline
    if (!program->link())
	close();

    // Binding shader pipeline for use
    if (!program->bind())
	close();

    // Restore system locale
    setlocale(LC_ALL, "");
}
//! [3]

//! [4]
void MainWidget::initTextures()
{
    // Loading cube.png to texture unit 0
    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    texture = bindTexture(QImage(":/cube.png"));

    // Set nearest filtering mode for texture minification
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // Set bilinear filtering mode for texture magnification
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Wrap texture coordinates by repeating
    // f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}
//! [4]

//! [5]
void MainWidget::resizeGL(int w, int h)
{
    // Set OpenGL viewport to cover whole widget
    glViewport(0, 0, w, h);

    // Calculate aspect ratio
    qreal aspect = (qreal)w / ((qreal)h?h:1);

    // Set near plane to 3.0, far plane to 7.0, field of view 45 degrees
    const qreal zNear = 5.0, zFar = 30.0, fov = 45.0;

    // Reset projection
    projection.setToIdentity();

    // Set perspective projection
    projection.perspective(fov, aspect, zNear, zFar);
}
//! [5]

void MainWidget::paintGL()
{
    // Clear color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    qglClearColor(QColor(50,50,50, 0 ));

//! [6]
    // Calculate model view transformation
    QMatrix4x4 matrix;
    matrix.translate(0.0, 0.0, -15.0);
 //   matrix.rotate(rotation);

    // Set modelview-projection matrix
        program->setUniformValue("mvp_matrix", projection * matrix * xform);
        program->setUniformValue("n_matrix", (matrix*xform).normalMatrix());
//! [6]

    // Using texture unit 0 which contains cube.png
    program->setUniformValue("texture", 0);

    // Draw cube geometry
    geometries->drawCubeGeometry(program);
}
