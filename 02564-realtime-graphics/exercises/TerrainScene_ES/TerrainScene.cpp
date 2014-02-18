/*
 *  main.cpp - Terrain Scene 02564_Framework
 *
 *  Created by J. Andreas Bærentzen on 01/02/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#define SOLUTION 1
#define SOLUTION_CODE

#include <iostream>

#include <QApplication>
#include <QTimer>
#include <QDebug>
#include <QMouseEvent>

#include <GLGraphics/GLHeader.h>

#include <CGLA/Mat4x4f.h>
#include <CGLA/Vec3f.h>
#include <CGLA/Vec2f.h>
#include <GLGraphics/ThreeDObject.h>
#include <GLGraphics/User.h>

#include "GBuffer.h"
#include "ShadowBuffer.h"
#include "auxil.h"
#include "Terrain.h"
#include "TerrainScene.h"
#include <GLGraphics/ShaderProgram.h>

using namespace std;
using namespace CGLA;
using namespace GLGraphics;


const string shader_path = "./shaders/TerrainSceneES/";
const string objects_path = "./data/";


Terrain terra(30,0.025);
Vec4f light_position(.3,.3,1,0);

#define DELTA_TIME 16

User user (&terra);
bool reload_shaders = true;
enum RenderMode {DRAW_NORMAL=0, DRAW_WIRE=1, DRAW_DEFERRED_TOON=2, DRAW_SSAO=3};
RenderMode render_mode = DRAW_NORMAL;

void draw_objects(ShaderProgramDraw& shader_prog)
{
	static vector<ThreeDObject> objects;
    
	if(objects.empty())
	{
        objects.push_back(ThreeDObject());
        objects[objects.size()-1].init(objects_path+"cottage_obj_s/cottage.obj");
		objects.back().scale(Vec3f(1));
        objects.back().translate(Vec3f(-10,1,terra.height(-10,1)+1));
		
        objects.push_back(ThreeDObject());
        objects[objects.size()-1].init(objects_path+"cow.obj");
		objects.back().scale(Vec3f(.5));
		objects.back().rotate(Vec3f(1,0,0), 90);		
        objects.back().translate(Vec3f(5,7,terra.height(5,7)+1.6));
		
        objects.push_back(ThreeDObject());
        objects[objects.size()-1].init(objects_path+"portal.obj");
		objects.back().scale(Vec3f(2));
        objects.back().translate(Vec3f(0,0,terra.height(0,0)+1.0));
	}
    
	for(int i=0;i<objects.size();++i)
        objects[i].display(shader_prog);
}


void set_light_and_camera(ShaderProgramDraw& shader_prog)
{
    static const Vec4f light_specular(0.6,0.6,0.3,0.6);
	static const Vec4f light_diffuse(0.6,0.6,0.3,0.6);
	static const Vec4f light_ambient(0.3,0.4,0.6,0.4);
    
    shader_prog.set_projection_matrix(perspective_Mat4x4f(55, float(WINDOW_WIDTH)/WINDOW_HEIGHT, 0.01, 100));
    shader_prog.set_view_matrix(user.get_view_matrix());
    shader_prog.set_light_position(light_position);
    shader_prog.set_light_intensities(light_diffuse, light_specular, light_ambient);
}

void render_direct(bool reload)
{
	// Load shaders for terrain and general objects. The terrain color is computed
	// procedurally, so we need a different shader.
	static ShaderProgramDraw terrain_shader(shader_path, "terrain.vert", "", "terrain.frag");	
	static ShaderProgramDraw object_shader(shader_path, "object.vert", "", "object.frag");	
	
	if(reload)
	{
		terrain_shader.reload();
		object_shader.reload();
	}

	glClearColor(0.4,0.35,0.95,0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    terrain_shader.use();
    set_light_and_camera(terrain_shader);
    terra.draw(terrain_shader);
    object_shader.use();
    set_light_and_camera(object_shader);
	draw_objects(object_shader);
}

TerrainScene::TerrainScene( const QGLFormat& format, QWidget* parent)
    : QGLWidget( new Core3_2_context(format), (QWidget*) parent),
      ax(0), ay(0), dist(12),ang_x(0),ang_y(0),mouse_x0(0),mouse_y0(0),
      detector(this), activity( Detector::Idle )
{
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(animate()));
    timer->start(16);
    connect(&detector, SIGNAL(activityChanged(int)),
                          this, SLOT(activityChanged(int)));
    connect(&detector, SIGNAL(step(int)),
                          this, SLOT(step(int)));

    connect(&detector, SIGNAL(accelerationChanged(QVec3f)),
                          this, SLOT(accelerationChanged(QVec3f)));

    detector.setRunning(true);
}

#ifdef SOLUTION_CODE

#define FILTER_LENGTH 20

float filter(float value, float *lastFrameValues, int frame){
    lastFrameValues[frame % FILTER_LENGTH] = value;
    float sum = 0;
    for (int i=0;i<FILTER_LENGTH;i++){
        sum += lastFrameValues[i];
    }
    return sum / FILTER_LENGTH;
}

#endif

void TerrainScene::accelerationChanged(QVec3f accel){
    this->accel = accel;
}

void TerrainScene::paintGL()
{
    switch(render_mode)
    {
        case DRAW_NORMAL:
            render_direct(reload_shaders);
            break;
    }
    reload_shaders = false;
    check_gl_error();
}

void TerrainScene::resizeGL(int W, int H)
{
    glViewport(0, 0, W, H);
    reload_shaders = true;
}


void TerrainScene::animate()
{
#ifdef SOLUTION_CODE
    static int step = 0;
    step++;
    static float yValues[FILTER_LENGTH];
    static float xValues[FILTER_LENGTH];
    // threshold values close to 0
    if (abs(accel[1]) < 1.0){
        accel[1] *= abs(accel[1]);
    }
    if (abs(accel[2]) < 1.0){
        accel[2] *= abs(accel[2]);
    }

    accel[1] = filter(accel[1], xValues, step);
    accel[2] = filter(accel[2], yValues, step);

    ay =  -accel[2]/10*M_PI;
    ax += -accel[1]*0.02;
    Vec3f dir(cos(ax)*cos(ay),sin(ax)*cos(ay),sin(ay));
    user.set_dir(dir);
#endif
    user.update_position();
    repaint();
}



void TerrainScene::initializeGL()
{
    setup_gl();
    glClearColor( 0.7f, 0.7f, 0.7f, 0.0f );
    glEnable(GL_DEPTH_TEST);
    check_gl_error();
}

void TerrainScene::set_light_and_camera(ShaderProgramDraw& shader_prog)
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



void TerrainScene::mousePressEvent(QMouseEvent *m)
{
    mouse_x0 = m->x();
    mouse_y0 = m->y();
}

void TerrainScene::mouseReleaseEvent(QMouseEvent *m)
{
    float delta_ang_x = 4*M_PI*(mouse_x0-m->x())/WINDOW_WIDTH;
    float delta_ang_y = M_PI*(mouse_y0-m->y())/WINDOW_HEIGHT;
    ang_x -= delta_ang_x;
    ang_y -= delta_ang_y;
}

void TerrainScene::mouseMoveEvent(QMouseEvent *m)
{
    float delta_ang_x = 4*M_PI*(mouse_x0-m->x())/WINDOW_WIDTH;
    float delta_ang_y = M_PI*(mouse_y0-m->y())/WINDOW_HEIGHT;
    ax = ang_x - delta_ang_x;
    ay = ang_y - delta_ang_y;
    Vec3f dir(cos(ax)*cos(ay),sin(ax)*cos(ay),sin(ay));
    user.set_dir(dir);
    QWidget::repaint();
}

void TerrainScene::keyPressEvent(QKeyEvent *e)
{
    static bool is_full_screen = false;
    switch(e->key())
        {
            case Qt::Key_Escape: exit(0);
            case ' ':
                render_mode = static_cast<RenderMode>((static_cast<int>(render_mode)+1)%4);
                reload_shaders = true;
                break;
            case 'W': user.forward();  break;
            case 'S': user.back();break;
            case 'A': user.strafe_left();break;
            case 'D': user.strafe_right();break;
            case 'F':
                if(is_full_screen) {
                    showNormal();
                    is_full_screen = false;
                }
                else
                {
                    showFullScreen();
                    is_full_screen = true;
                }
                break;
            case 'R':
                reload_shaders = true;
                break;
            case 'L':
            {
                Vec3f v = normalize(-user.get_dir() + Vec3f(0,0,1));
                light_position = Vec4f(v[0], v[1], v[2], 0);
            }
                break;
            case 'O':
                user.set(Vec3f(-7.60765, 14.907, 4.37377), Vec3f(0.333226, -0.925571, 0.179661));
                break;
            default:
                QWidget::keyPressEvent(e);
                break;
        }
    QWidget::repaint();
}

void TerrainScene::keyReleaseEvent(QKeyEvent *)
{
    user.stop();
}

void TerrainScene::step(int value){
    // unused
#ifdef SOLUTION_CODE
    cout << "step "<<activity<<endl;
#endif
}

void TerrainScene::activityChanged(int activityValue){
    Detector::Activity value = (Detector::Activity)activityValue;
    cout << "activityChanged "<<value<<endl;
#ifdef SOLUTION_CODE
    string activity = "?";
    switch(value){
    case Detector::Idle:
        activity = "Idle";
        user.stop();
        break;
    case Detector::Unknown:
        activity = "Unknown";
        user.stop();
        break;
    case Detector::Walking:
        activity = "Walking";
        user.forward();
        break;
    default:
        cout << "activityChanged unknown"<<endl;
    }
    cout << "activityChanged "<<activity<<endl;
 #endif
}


int main(int argc, char *argv[])
{
    qDebug() << argv[0] << " - " << argv[1] << endl;
    QApplication a( argc, argv );

    QGLFormat glFormat;
    TerrainScene w( glFormat);

    w.showMaximized();
    w.setFocusPolicy(Qt::ClickFocus);
    return a.exec();
}
