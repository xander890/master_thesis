#include "mainwidget.h"
#include <QtOpenGL/QGLShaderProgram>
#include <QBasicTimer>
#include <QMouseEvent>
#include <math.h>
#include <QDebug>
#include <iostream>
#include <string>
#include <GLGraphics/ShaderProgram.h>
#include <Mesh/Texmap.h>

#define SOLUTION_CODE 1

using namespace std;
using namespace GLGraphics;
using namespace CGLA;

MainWidget::MainWidget(QWidget *parent) :
    QGLWidget(parent),
    timer(new QBasicTimer),
    mousePressPosition(0.0,0.0),
    rotationAxis(0,1,0),
    angularSpeed(0),
    rotation(0,0,0,1),
    modelMatrix(identity_Mat4x4f()),
    rot_mat(identity_Mat4x4f()),
    touch_acc(0.0,0.0,0.0)
{
    sensors = new Sensors();
    connect(sensors,SIGNAL(update()),this,SLOT(updateSensors()));
    updateSensors();
}

void MainWidget::updateSensors()
{
    //Light sensor
    lightValue = std::min(sensors->light(), 255.0);

    //Accelerometer
    x = sensors->accX();
    y = sensors->accY();
    z = sensors->accZ();

#ifdef SOLUTION_CODE
#else
    Vec3f n = normalize(Vec3f(x,y,0.0));
    float acc = sqrt(pow(x,2)+pow(y,2))/100;
    rotationAxis = normalize(rotationAxis * angularSpeed + n*acc);
    angularSpeed += acc;
#endif

    //Compass
    azimuth = sensors->azimuth();
}

MainWidget::~MainWidget()
{
    delete timer; timer = 0;


    deleteTexture(texture);
}

void MainWidget::mousePressEvent(QMouseEvent *e)
{
    // Saving mouse press position
    mousePressPosition = Vec2f(e->posF().x(), e->posF().y());
}

void MainWidget::mouseReleaseEvent(QMouseEvent *e)
{

    // Mouse release position - mouse press position

    Vec2f diff =  Vec2f(e->posF().x(), e->posF().y()) - mousePressPosition;

#ifdef SOLUTION_CODE
    // Rotation axis is perpendicular to the mouse position difference
    // vector
    Vec3f n = normalize(Vec3f(diff[0], diff[1], 0.0));


    touch_acc = Vec3f(diff[0], -diff[1], 0.0);
    // Accelerate angular speed relative to the length of the mouse sweep

    float acc = diff.length() / 100.0f;

    // Calculate new rotation axis as weighted sum

    rotationAxis = normalize(rotationAxis * angularSpeed + n * acc);

    // Increase angular speed
    angularSpeed += acc;
#else
   // todo touch_acc, rotationAxis and angularSpeed here
#endif
}

void MainWidget::timerEvent(QTimerEvent *e)
{
    Q_UNUSED(e);
    Vec3f acc_ = Vec3f(sensors->accY(), -sensors->accX(), -sensors->accZ());

#ifdef SOLUTION_CODE
    static Vec3f pos_(0,0,0);
    static Vec3f pos_old_(0,0,0);

    Vec3f pos_new_ = 2.0 * pos_ - pos_old_ + 0.0025 * (acc_ + touch_acc);

    touch_acc = Vec3f(0,0,0);

    pos_old_ = pos_;
    pos_ = pos_new_;

    float WALL = 9;
    float WALLX = 15;

    if (pos_[0] < - WALLX) pos_[0] = -WALLX;
    if (pos_[0] > WALLX) pos_[0] = WALLX;
    if (pos_[1] < - WALL) pos_[1] = -WALL;
    if (pos_[1] > WALL) pos_[1] = WALL;
    if (pos_[2] < - WALL) pos_[2] = -WALL;
    if (pos_[2] > WALL) pos_[2] = WALL;

    modelMatrix = translation_Mat4x4f(pos_);

    Vec3f velocity = pos_-pos_old_;
    float speed_ = velocity.length();

    if (velocity[0] != 0.0 || velocity[1] != 0.0){

        Vec3f rot_axis_ = normalize(cross(velocity, Vec3f(0,0,-1)));
        Quatf rotationQuat;
        const float DEGREE_TO_RADIAN = (2*M_PI)/360;
        rotationQuat.make_rot( speed_*10*DEGREE_TO_RADIAN, rot_axis_);
        rot_mat = rotationQuat.get_Mat4x4f() * rot_mat;
     }

    modelMatrix *=rot_mat;
#else
    // todo compute model matrix here
#endif

    // Update scene
    updateGL();
}

void MainWidget::initializeGL()
{
    string filename = "./data/cube/cube.obj";
    cube.load(filename, false);

    qDebug() << "Initializing textures...";
    initTextures();

    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // Enable back face culling
    glEnable(GL_CULL_FACE);

    rot_mat = identity_Mat4x4f();

    // using QBasicTimer because its faster that QTimer
    timer->start(12, this);
}

void MainWidget::initTextures()
{
    // Loading cube.png to texture unit 0
    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    static Mesh::Texmap tex;
    tex.load(":/data/cube/cubees64.png");
    texture = tex.get_id();
    glBindTexture(GL_TEXTURE_2D, texture);
}

void MainWidget::resizeGL(int w, int h)
{
    // Set OpenGL viewport to cover whole widget
    glViewport(0, 0, w, h);

    // Calculate aspect ratio
    float aspect = (float)w / ((float)h?h:1);

    // Set near plane to 3.0, far plane to 7.0, field of view 45 degrees
    const float zNear = 5.0, zFar = 30.0, fov = 45.0;

    // Set perspective projection
    projection = perspective_Mat4x4f(fov, aspect, zNear, zFar);
}

void MainWidget::paintGL()
{
    static ShaderProgramDraw cubeShader("./shaders/cubees/", "cube.vert", "", "cube.frag");

    // Clear color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(50.0f/255,50.0f/255,50.0f/255, 0);

    // Calculate model view transformation
    Mat4x4f viewMatrix = translation_Mat4x4f(Vec3f(0.0, 0.0, -15.0));

    // Draw cube geometry
    cubeShader.use();
    cubeShader.set_uniform("light_intensity", GLfloat(lightValue/255.0f));
    cubeShader.set_model_matrix(modelMatrix);
    cubeShader.set_view_matrix(viewMatrix);
    cubeShader.set_projection_matrix(projection);

    cubeShader.set_uniform("texture", 0);
    cube.renderDirect(cubeShader);
}
