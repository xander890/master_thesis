/*
 *  main.cpp - Terrain Scene 02564_Framework
 *
 *  Created by J. Andreas Bærentzen on 01/02/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#define SOLUTION_CODE

#include <iostream>

#include <QApplication>
#include <QTimer>
#include <QTime>
#include <QDebug>
#include <QMouseEvent>

#include <GLGraphics/GLHeader.h>

#include <CGLA/Mat4x4f.h>
#include <CGLA/Vec3f.h>
#include <CGLA/Vec2f.h>
#include <GLGraphics/ThreeDObject.h>
#include <GLGraphics/User.h>

#include "ShadowBuffer.h"
#include "Terrain.h"
#include "translucentmaterials.h"
#include <GLGraphics/ShaderProgram.h>
#include <GLGraphics/lightmanager.h>
#include <GLGraphics/light.h>
#include <Objects/threedplane.h>
#include <Objects/threedsphere.h>
#include <Objects/threedcube.h>
#include <Dipoles/bssrdf.h>
#include <Dipoles/directionaldipole.h>
#include <Dipoles/jensendipole.h>
#include <Dipoles/betterdipole.h>
#include <Utils/miscellaneous.h>
#include "Dipoles/dipolecpu.h"
#include <Dipoles/singlescattering.h>
#include <Dipoles/dipolegpu.h>
#include <Utils/defaultmaterials.h>
#include <Objects/threedline.h>
#include <Objects/threedgrid.h>
#include <Objects/threedwirecube.h>
#include "vertexnormalbuffer.h"
#include "cubemapbuffer.h"
#include "Utils/areaestimator.h"
#include "arraytexturebuffer.h"
#include "mipmapgenerator.h"
#include "arrayvertexnormalbuffer.h"
#include "GLGraphics/computeshader.h"
#include "shaderpreprocessor.h"
#include "GLGraphics/ResourceLoader.h"
#include "mipmapgeneratorview.h"
#include <GLGraphics/infinitearealight.h>
#include "depthonlybuffer.h"
#include <Mesh/texturebuffer.h>
#include "Utils/cglautils.h"
#define BUNNY 0
#define BUDDHA 1
#define DRAGON 2

#define MODEL 2

#define POINT_DIST 0 // 0 random, 1 exponential, 2 uniform
#define TIMER
#define UNIFORM_SAMPLING 0
#define RUSSIAN_ROULETTE
//#define DIR
#define DEBUG_SCREEN
//#define MEMORY_INFO
//#define SINGLE_LIGHT

using namespace std;
using namespace CGLA;
using namespace GLGraphics;

const string shader_path = "./shaders/TranslucentMaterials/";
const string objects_path = "./data/";
const string default_obj = "bunny1";
static const Vec4f light_ambient(0.3f,0.4f,0.6f,0.4f);

Terrain terra(30,0.025f);

const int GBUFFER_SIZE = 512;
const float LIGHT_CAMERA_SIZE = 1.0f;
const float LIGHT_CAMERA_DISTANCE = 6.0f;
const int ARRAY_TEXTURE_SIZE = 1024;
const int LIGHTS = 1;
const int LAYERS = 16;
const int MIPMAPS = 3;
const float CAMERA_DISTANCE = 3.0f; //This should not matter (can be DIST = max bounding box + camera near + epsilon
const float CAMERA_NEAR = 0.1f;
const float CAMERA_FAR = 10.0f;
const float RADIANCE_CAMERA_SIZE = 1.0f;
const int CONVERGENCE_FRAMES = 1000;

User user (&terra);
bool reload_shaders = true;

LightManager manager;

const Vec4f light_specular(0.6f,0.6f,0.3f,0.6f);
const Vec4f light_diffuse(1.f,1.f,1.f,1.0f);
//const Vec4f light_diffuse(0.5f,0.5f,1.3f,1.0f);
//const Vec4f light_position(-1.f,-0.5f,1.f,1);
const Vec4f light_position(0.f,0.f,1.f,0);
const Vec4f light_diffuse_2(1.3,0.5,0.5,0.0);
const Vec4f light_position_2(2.0,0.0,0.0,1.0);

bool compareVec2fDistanceAscending (CGLA::Vec2f i,CGLA::Vec2f j) { return (i.length() < j.length()); }

void APIENTRY openglCallbackFunction(GLenum source,
                                           GLenum type,
                                           GLuint id,
                                           GLenum severity,
                                           GLsizei length,
                                           const GLchar* message,
                                           void* userParam){

    cout << "---------------------opengl-callback-start------------" << endl;
    cout << "message: "<< message << endl;
    cout << "type: ";
    switch (type) {
    case GL_DEBUG_TYPE_ERROR:
        cout << "ERROR";
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        cout << "DEPRECATED_BEHAVIOR";
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        cout << "UNDEFINED_BEHAVIOR";
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        cout << "PORTABILITY";
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        cout << "PERFORMANCE";
        break;
    case GL_DEBUG_TYPE_OTHER:
        cout << "OTHER";
        break;
    }
    cout << endl;

    cout << "id: "<    cout << "severity: ";
    switch (severity){
    case GL_DEBUG_SEVERITY_LOW:
        cout << "LOW";
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        cout << "MEDIUM";
        break;
    case GL_DEBUG_SEVERITY_HIGH:
        cout << "HIGH";
        break;
    }
    cout << endl;
    cout << "---------------------opengl-callback-end--------------" << endl;
}


TranslucentMaterials::TranslucentMaterials( QWidget* parent)
    : QGLWidget( new Core4_3_context(), (QWidget*) parent),
      ax(0), ay(0), dist(12),ang_x(0),ang_y(0),mouse_x0(0),mouse_y0(0)
    , clearColor(Vec4f(0.0f,0.0f,0.0f,1.0f)), frame(0),
      isVertexMode(true),
      isShadow(true),
      isGridVisible(false),
      areAxesVisible(false),
      params(new TranslucentParameters()),
      render_mode(DRAW_DIRECTIONAL),
      render_method(CUBEMAP_BASE),
      currentFrame(0),
      performanceTimer(PerformanceTimer(20)),
      skybox_cube(new ThreeDSphere(30)),
      objectPool((*new vector<ThreeDObject*>()))
    {

        #ifndef TIMER
            performanceTimer.setIntermediateEnabled(false);
        #endif
        initialize();
        Light mainLight (light_position, light_diffuse, 1.0f, light_specular, true);
        Light secondaryLight(light_position_2, light_diffuse_2, 15.0f, Vec4f(0.0f), false);
#ifndef DIR
        manager.addLight(mainLight);
        //manager.addLight(secondaryLight);
#endif
        manager.addLight(secondaryLight);
        setFocusPolicy(Qt::ClickFocus);
        timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(animate()));
        timer->start(16);
    }


void TranslucentMaterials::initialize()
{
    Mesh::ScatteringMaterial * scattering_mat_bunny = getDefaultMaterial(S_Marble,1000);
   // Mesh::ScatteringMaterial * scattering_mat_bunny = getDefaultMaterial(S_Whitegrapefruit,1000);
   // Mesh::ScatteringMaterial * scattering_mat_bunny = getDefaultMaterial(S_Whitegrapefruit,1000);
    Mesh::ScatteringMaterial * scattering_mat_buddha = getDefaultMaterial(S_Potato);
    Mesh::ScatteringMaterial * scattering_mat_dragon = getDefaultMaterial(S_Marble,1000);

    ThreeDObject * bunny = new ThreeDObject();
    ThreeDObject * buddha = new ThreeDObject();
    ThreeDObject * dragon = new ThreeDObject();
    ThreeDObject * sphere = new ThreeDSphere(40);
    ThreeDObject * plane = new ThreeDPlane();

    plane->init("", "plane", *scattering_mat_dragon);
    plane->setScale(Vec3f(1.f));
    plane->setRotation(Vec3f(90,0,0));
    plane->setTranslation(Vec3f(0,0,0.f));
    plane->enabled = true;
    plane->boundingBoxEnabled = true;

    bunny->init(objects_path+"closed_bunny.obj", "bunny", *scattering_mat_bunny);
    bunny->setScale(Vec3f(1.f));
    bunny->setRotation(Vec3f(90,0,0));
    bunny->setTranslation(Vec3f(0,0,0.f));
    bunny->enabled = true;
    bunny->boundingBoxEnabled = true;

    //buddha->init(objects_path+"buddha_redone.obj", "buddha", *scattering_mat_buddha);
    buddha->setScale(Vec3f(1.f));
    buddha->setRotation(Vec3f(90,0,0));
    buddha->setTranslation(Vec3f(0,0,0.f));
    buddha->enabled = true;
    buddha->boundingBoxEnabled = true;

    dragon->init(objects_path+"dragon.obj", "dragon", *scattering_mat_dragon);
    dragon->setScale(Vec3f(1.f));
    dragon->setRotation(Vec3f(0,0,0));
    dragon->setTranslation(Vec3f(0,0,0.f));
    dragon->enabled = true;
    dragon->boundingBoxEnabled = true;

    sphere->init("","sphere", *scattering_mat_bunny);
    sphere->setScale(Vec3f(.25f));
    sphere->setTranslation(Vec3f(0,0,0.f));
    sphere->enabled = true;
    sphere->boundingBoxEnabled = true;

    objectPool.push_back(bunny);
    objectPool.push_back(buddha);
    objectPool.push_back(dragon);
    currentObject = dragon;
}




void TranslucentMaterials::set_light_and_camera(ShaderProgramDraw& shader_prog)
{
    check_gl_error();
    shader_prog.set_projection_matrix(perspective_Mat4x4f(53.1301, float(window_width)/window_height, 0.1f, 100));
    shader_prog.set_view_matrix(user.get_view_matrix());
    shader_prog.set_uniform("light_amb", light_ambient);
    shader_prog.set_uniform("user_pos",user.get_pos());
    shader_prog.set_uniform("gamma",params->gamma);
    manager.loadLights(shader_prog);
}


void TranslucentMaterials::draw_axes(bool reload)
{
    static ShaderProgramDraw color_shader(shader_path, "color.vert", "", "color.frag");
    if(reload)
    {
        color_shader.reload();
    }

    static bool was_here = false;
    static vector<ThreeDObject*> axes = *(new vector<ThreeDObject*>());
    if(!was_here)
    {
        was_here = true;
        Mesh::Material * m = new Mesh::Material();
        ThreeDLine * x = new ThreeDLine();
        x->init("","x",*m);
        axes.push_back(x);
        x->setColor(Vec4f(1.0f,0.0f,0.0f,1.0f));
        x->setScale(Vec3f(1000.0f));
        x->setRotation(Vec3f(0.0f,90.0f,0.0f));

        ThreeDLine * y = new ThreeDLine();
        y->init("","y",*m);
        axes.push_back(y);
        y->setColor(Vec4f(0.0f,1.0f,0.0f,1.0f));
        y->setScale(Vec3f(1000.0f));
        y->setRotation(Vec3f(90.0f,0.0f,0.0f));

        ThreeDLine * z = new ThreeDLine();
        z->init("","z",*m);
        axes.push_back(z);
        z->setScale(Vec3f(1000.0f));

        z->setColor(Vec4f(0.0f,0.0f,1.0f,1.0f));

    }
    color_shader.use();
    set_light_and_camera(color_shader);

    for(int i = 0; i < 3; i++)
    {
        axes[i]->display(color_shader);
    }
}

void TranslucentMaterials::draw_grid(bool reload)
{
    static ShaderProgramDraw color_shader(shader_path, "color.vert", "", "color.frag");
    if(reload)
    {
        color_shader.reload();
    }

    static ThreeDGrid * grid = new ThreeDGrid(1.0f,50);
    static bool was_here = false;
    if(!was_here)
    {
        was_here = true;
        grid->init("","grid",*(new Mesh::Material()));
        grid->setColor(Vec4f(0.5f));

    }
    color_shader.use();
    set_light_and_camera(color_shader);
    grid->display(color_shader);
}

void TranslucentMaterials::draw_bounding_boxes(bool reload)
{
    static ShaderProgramDraw color_shader(shader_path, "color.vert", "", "color.frag");

    if(reload)
        color_shader.reload();

    color_shader.use();
    set_light_and_camera(color_shader);

    const int bboxes = 10;
    static vector<ThreeDWireCube*> boundingBoxCubes(bboxes);
    static bool washere = false;
    if(!washere)
    {
        Mesh::Material * m = new Mesh::Material();
        for(int i = 0; i < bboxes; i++)
        {
            boundingBoxCubes[i] = new ThreeDWireCube();
            boundingBoxCubes[i]->init("","",*m);
        }
        washere = true;
    }


    //TODO REDO

}




void TranslucentMaterials::getDiscPoints(vector<Vec2f> * points, const int n, const int m)
{
    getDiscPoints(points,n,m,0.0f,1.0f);
}

void TranslucentMaterials::getDiscPoints(vector<Vec2f> * points, const int n, const int m, float sigma_tr, float radius)
{
    vector<vector<Vec2f> > texture;

//#if POINT_DIST == 0
#if UNIFORM_SAMPLING == 1
    planeHaltonCircleUniformMulti(texture, n, m, sigma_tr, radius);
#elif UNIFORM_SAMPLING == 0
    planeHaltonCircleRejectionExponentialMulti(texture, n, m, sigma_tr, radius);
#endif
    //#elif POINT_DIST == 1
//    planeHammersleyCircleMultiExp(texture, n, m,3.0f);
//#else
//    circleUniformPoints(texture, n / 50, m, 50);
//#endif

   // vector<Vec2f> test1;
//    planeHammersleyCircleAlternative(test1, 200);
    //planeHaltonCircleRejectionExponential(test1,200,0.11f);
    //cout << "Alternative" << endl;
    //for(int i = 0 ; i < 200; i++) cout << test1[i][0] << " " << test1[i][1] << endl;
    //test1.clear();
    //planeHammersleyCircle(test1,200);
    //cout << "Standard" << endl;
    //for(int i = 0 ; i < n; i++) cout << texture[0][i][0] << " " << texture[0][i][1] << endl;

    for(int k = 0; k < m; k++)
    {

        vector<Vec2f> discpoints = texture[k];
        //std::sort(discpoints.begin(),discpoints.end(),compareVec2fDistanceAscending);
        for(int i = 0; i < discpoints.size(); i++)
        {
            Vec2f p = Vec2f(discpoints[i][0],discpoints[i][1]);
            points->push_back(p);

        }
    }
}


void TranslucentMaterials::getDiscPointsSpectral(vector<Vec3f> * points, const int n, const int m, Vec3f sigma_tr, float radius)
{
    vector<vector<Vec2f> > texture;

//#if POINT_DIST == 0
    planeHaltonCircleRejectionExponentialMulti(texture, n, m, sigma_tr[0], radius);
    planeHaltonCircleRejectionExponentialMulti(texture, n, m, sigma_tr[1], radius);
    planeHaltonCircleRejectionExponentialMulti(texture, n, m, sigma_tr[2], radius);
//#elif POINT_DIST == 1
//    planeHammersleyCircleMultiExp(texture, n, m,3.0f);
//#else
//    circleUniformPoints(texture, n / 50, m, 50);
//#endif

   // vector<Vec2f> test1;
//    planeHammersleyCircleAlternative(test1, 200);
    //planeHaltonCircleRejectionExponential(test1,200,0.11f);
    //cout << "Alternative" << endl;
    //for(int i = 0 ; i < 200; i++) cout << test1[i][0] << " " << test1[i][1] << endl;
    //test1.clear();
    //planeHammersleyCircle(test1,200);
    //cout << "Standard" << endl;
    //for(int i = 0 ; i < n; i++) cout << texture[0][i][0] << " " << texture[0][i][1] << endl;

    for(int k = 0; k < 3 * m; k++)
    {
        //cout << "Vector " << k <<endl;
        if( k == 0)
        {
            //cout << "Vector, " << n << " points" << endl;
        }

        vector<Vec2f> discpoints = texture[k];
        //std::sort(discpoints.begin(),discpoints.end(),compareVec2fDistanceAscending);
        for(int i = 0; i < discpoints.size(); i++)
        {
            if (k == 0)
            {
                //cout <<discpoints[i][0] << " " << discpoints[i][1] << endl;
            }
            Vec2f p = Vec2f(discpoints[i][0],discpoints[i][1]);
            points->push_back(Vec3f(discpoints[i][0],discpoints[i][1],0.0f));

        }
    }
}


void TranslucentMaterials::render_dirss(bool reload, ShaderProgramDraw & render_to_array)
{


    performanceTimer.registerEvent("-1: Start");
    //static ShaderProgramDraw obj_shader(shader_path,"object.vert","","object.frag");
    //static ShaderProgramDraw gbuff_quad(shader_path,"ss_cubemap_test_gbuffer.vert","","ss_cubemap_test_gbuffer.frag");
    //static ShaderProgramDraw gbuff_wrap(shader_path,"ss_cubemap_test_wrap_gbuffer.vert","","ss_cubemap_test_wrap_gbuffer.frag");
    //static ShaderProgramDraw render_to_cubemap_test(shader_path,"ss_cubemap_render_to_cubemap.vert","","ss_cubemap_render_to_cubemap.frag");
    //static ShaderProgramDraw render_to_cubemap_test_screen(shader_path,"ss_cubemap_test_render_to_cubemap_screen.vert","","ss_cubemap_test_render_to_cubemap_screen.frag");
    //static ShaderProgramDraw render_to_cubemap_test_cube(shader_path,"ss_cubemap_test_render_to_cubemap_cube.vert","","ss_cubemap_test_render_to_cubemap_cube.frag");

    static ShaderProgramDraw gbuff_shader(shader_path,"ss_array_gbuffer_multilight.vert","ss_array_gbuffer_multilight.geom", "ss_array_gbuffer_multilight.frag");
    static ShaderProgramDraw render_combination(shader_path,"ss_array_combination.vert","","ss_array_combination.frag");
    static ShaderProgramDraw render_mipmaps(shader_path,"ss_array_generate_mipmaps.vert","ss_array_generate_mipmaps.geom","ss_array_generate_mipmaps.frag");
    static ShaderProgramDraw render_to_array_pass(shader_path,"ss_array_render_to_arraymap.vert","ss_array_render_to_arraymap.geom","ss_array_render_to_arraymap_pass.frag");

#ifdef DEBUG_SCREEN
    static ThreeDPlane * screen_quad = new ThreeDPlane();
    static Mesh::Material * screen_quad_material = new Mesh::Material();
    static ShaderProgramDraw screen_quad_display_shader(shader_path,"ss_array_debug_tex.vert","","ss_array_debug_tex.frag");
#endif

    int samples_per_texel = params->samples / (manager.size());
    int maximum_samples = params->maxsamples;

    static ArrayTextureBuffer radiance_map(ARRAY_TEXTURE_SIZE,LAYERS,MIPMAPS + 1, 0);
    static MipMapGeneratorView * radiance_map_mipmaps = new MipMapGeneratorView(radiance_map.getColorTexture()->get_id(), radiance_map.getDepthTexture()->get_id(), ARRAY_TEXTURE_SIZE, LAYERS, MIPMAPS);
    static ArrayVertexNormalBuffer light_map(GBUFFER_SIZE, LIGHTS);

    int discPoints = maximum_samples; //TODO more LIGHTS!

    ThreeDObject * obj = currentObject;
    Vec3f m = currentObject->getScale() * (currentObject->getBoundingBox()->high - currentObject->getBoundingBox()->low);
    float MAX_RADIUS = 1.0f;//max3(m) / (2 * CAMERA_SIZE);

    Mesh::ScatteringMaterial * scattering_material = (Mesh::ScatteringMaterial*)obj->mesh.getMaterial();
    Vec3f tr = scattering_material->transmissionCoefficient / params->circleradius;
    float selectedTransmission = min(tr[0],min(tr[1],tr[2]));
    //selectedTransmission = tr[0];
    //selectedTransmission = (tr[0] + tr[1] + tr[2]) / 3;

    if(reload)
    {
        //test.reload();
#ifdef DEBUG_SCREEN
        screen_quad_display_shader.reload();
#endif
        //obj_shader.reload();
        gbuff_shader.reload();
        //gbuff_quad.reload();
        //gbuff_wrap.reload();
        render_to_array.reload();
        //render_to_cubemap_test_screen.reload();
        //render_to_cubemap_test_cube.reload();
        render_combination.reload();
        render_mipmaps.reload();
        cout << "Current sigma_tr: " << scattering_material->transmissionCoefficient << endl;
        currentFrame = 0;
    }


    static vector<Vec3f> camera_positions(LAYERS);
    static vector<Mat4x4f> radiance_map_view_matrices(LAYERS);
    static vector<Mat4x4f> radiance_map_project_view_matrices(LAYERS);
    static Mat4x4f model_identity_matrix = identity_Mat4x4f();
    static Mat4x4f projection_array_matrix = ortho_Mat4x4f(Vec3f(-RADIANCE_CAMERA_SIZE,-RADIANCE_CAMERA_SIZE,CAMERA_NEAR),Vec3f(RADIANCE_CAMERA_SIZE,RADIANCE_CAMERA_SIZE,CAMERA_FAR));
    static Mat4x4f projection_light_matrix = ortho_Mat4x4f(Vec3f(-LIGHT_CAMERA_SIZE,-LIGHT_CAMERA_SIZE,1),Vec3f(LIGHT_CAMERA_SIZE,LIGHT_CAMERA_SIZE,10));
    static Mat4x4f texture_projection_array_matrix = translation_Mat4x4f(Vec3f(0.5)) * scaling_Mat4x4f(Vec3f(0.5)) * projection_array_matrix;
    static Vec3f up = Vec3f(0,1,0);
    static vector<Vec3f> spherePoints;
    static vector<Vec2f> discpoint_data;
    static bool initialized = false;


    if(!initialized)
    {
        initialized = true;
        screen_quad->init("","plane",*screen_quad_material);

        getDiscPoints(&discpoint_data,discPoints,1, selectedTransmission, MAX_RADIUS);

        //Mesh::Texture * tex = new Mesh::Texture("discpoints",GL_TEXTURE_1D, discPoints, 1, discpoint_data);
        Mesh::Texture * tex = new Mesh::TextureBuffer<Vec2f>("discpoints",discPoints, GL_RG32F, discpoint_data);
        tex->init();
        scattering_material->addTexture(tex);

        // preparing first color buffer from which to read (just for avoiding errors,
        // the shader already avoids that)

        Mesh::Texture * vtex = light_map.getVertexTexture();
        Mesh::Texture * ntex = light_map.getNormalTexture();

        scattering_material->addTexture(vtex);
        scattering_material->addTexture(ntex);
        scattering_material->addTexture(skybox);
        scattering_material->addTexture(radiance_map.getColorTexture());
        scattering_material->addTexture(radiance_map.getDepthTexture());

        sphereHalton(spherePoints, LAYERS);

        for(int i = 0; i < LAYERS; i++) cout << spherePoints[i] <<endl;

        //blending parameters
        glEnable(GL_BLEND);
        glBlendColor(0.0f,0.0f,0.0f,0.0f);
        glBlendFunc(GL_ONE, GL_ONE);
        glBlendEquation(GL_FUNC_ADD);
        glDisable(GL_BLEND);

#ifdef DEBUG_SCREEN
        screen_quad_material->addTexture(vtex);
        screen_quad_material->addTexture(ntex);
#endif

    }

    if(params->currentFlags & TranslucentParameters::SAMPLES_CHANGED)
    {
        discpoint_data.clear();
        getDiscPoints(&discpoint_data,discPoints,1, selectedTransmission, MAX_RADIUS);
        Mesh::TextureBuffer<Vec2f> * tex = static_cast<Mesh::TextureBuffer<Vec2f> *>(scattering_material->getTexture(string("discpoints")));
        tex->reloadBufferData(discpoint_data);
        params->currentFlags &= ~(TranslucentParameters::SAMPLES_CHANGED);
    }

    performanceTimer.registerEvent("0: Initialization");

    if(currentFrame < CONVERGENCE_FRAMES)
    {

        Vec3f center = obj->getCenter();

        gbuff_shader.use();

        // Set up a modelview matrix suitable for shadow: Maps from world coords to
        // shadow buffer coords.

        gbuff_shader.set_model_matrix(model_identity_matrix);
        gbuff_shader.set_projection_matrix(projection_light_matrix);

        vector<Mat4x4f> lightMatrices;
        vector<Mat4x4f> inverseLightMatrices;

        for(int i = 0; i < manager.size(); i++)
        {
            Vec3f light_pos;
            if(manager[i].isDirectional)
            {
                light_pos = center + LIGHT_CAMERA_DISTANCE * normalize(Vec3f(manager[i].position));
            }
            else
            {
                light_pos = manager[i].position;
            }

            Mat4x4f V = lookat_Mat4x4f_target(light_pos,center,up);
            lightMatrices.push_back(V); //PARALLEL!

            Mat4x4f mat = translation_Mat4x4f(Vec3f(0.5));
            mat *= scaling_Mat4x4f(Vec3f(0.5));
            mat *= projection_light_matrix;
            mat *= V;
            inverseLightMatrices.push_back(mat);
        }

        gbuff_shader.set_uniform("lightMatrices",lightMatrices,manager.size());
        gbuff_shader.set_uniform("layers", manager.size());

        // Switch viewport size to that of shadow buffer.

        glViewport(0, 0, GBUFFER_SIZE, GBUFFER_SIZE);

        // Draw to shadow buffer.
        light_map.enable();
        obj->display(gbuff_shader);

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glDrawBuffer(GL_BACK);

        performanceTimer.registerEvent("1: Render to lightmap");

        for(int i = 0; i < LAYERS; i++)
        {
            Vec3f camera_pos = center + spherePoints[i] * CAMERA_DISTANCE;
            camera_positions[i] = camera_pos;
            radiance_map_view_matrices[i] = scaling_Mat4x4f(Vec3f(1,-1,1)) * lookat_Mat4x4f_target(camera_pos, center, up);
            radiance_map_project_view_matrices[i] = texture_projection_array_matrix * radiance_map_view_matrices[i];
        }

        // pre render depth
        radiance_map.enable();

        glViewport(0,0,ARRAY_TEXTURE_SIZE,ARRAY_TEXTURE_SIZE);
        render_to_array_pass.use();
        set_light_and_camera(render_to_array_pass);
        render_to_array_pass.set_uniform("viewMatrices", radiance_map_view_matrices, LAYERS);
        render_to_array_pass.set_uniform("layers", LAYERS);
        render_to_array_pass.set_model_matrix(model_identity_matrix);
        render_to_array_pass.set_projection_matrix(projection_array_matrix);

        glDepthMask(GL_TRUE);
        glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
        glClear(GL_DEPTH_BUFFER_BIT);
        obj->display(render_to_array_pass);
        glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);

        // actual rendering of SS effects
        render_to_array.use();

        render_to_array.set_uniform("one_over_max_samples",1.0f/maximum_samples);
        render_to_array.set_uniform("max_samples",maximum_samples);
        //render_to_array.set_uniform("one_over_discs",1.0f/DISCS);
        render_to_array.set_uniform("samples",samples_per_texel);
        //render_to_array.set_uniform("discradius",trueRadius);
        render_to_array.set_uniform("epsilon_gbuffer", params->epsilon_gbuffer);
        render_to_array.set_uniform("min_tr", selectedTransmission);
        render_to_array.set_uniform("lightMatrices",inverseLightMatrices, manager.size());
        render_to_array.set_uniform("epsilon_combination", params->epsilon_combination);
        render_to_array.set_uniform("cameraMatrices", radiance_map_project_view_matrices,LAYERS);
        render_to_array.set_uniform("current_frame_percentage", currentFrame / float(CONVERGENCE_FRAMES));
        render_to_array.set_uniform("current_frame", currentFrame);
        render_to_array.set_uniform("convergence_frames", CONVERGENCE_FRAMES);
        render_to_array.set_uniform("global_frame",frame);
        render_to_array.set_uniform("point_radius",MAX_RADIUS);
        set_light_and_camera(render_to_array);
        render_to_array.set_uniform("viewMatrices", radiance_map_view_matrices, LAYERS);
        render_to_array.set_uniform("layers", LAYERS);
        render_to_array.set_model_matrix(model_identity_matrix);
        render_to_array.set_projection_matrix(projection_array_matrix);
        render_to_array.set_uniform("a_m",params->a);
        render_to_array.set_uniform("b_m",params->b);
        render_to_array.set_uniform("light_texture_to_world_factor", 1.0f / (2 * LIGHT_CAMERA_SIZE));

        static float minimum_integration_radius = 0.001f;
        static float exponential_integral = exponential_over_cube_approximation(selectedTransmission,minimum_integration_radius);
        render_to_array.set_uniform("minimum_integration_radius",minimum_integration_radius);
        render_to_array.set_uniform("exponential_integral",exponential_integral);

        if (currentFrame == 0)
        {
            glClearColor(0,0,0,0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            obj->display(render_to_array);
        }
        else
        {
            glDepthMask(GL_FALSE);
            glDepthFunc(GL_LEQUAL);
            glEnable(GL_BLEND);
            obj->display(render_to_array);
            glDisable(GL_BLEND);
            glDepthMask(GL_TRUE);
            glDepthFunc(GL_LESS);
        }



        performanceTimer.registerEvent("2: Render to array");
        radiance_map.generateMipMaps();
        // Need to disable mipmaps, otherwise I cannot copy from level 0 to another (weird opengl stuff)
        /*
        radiance_map.disableMipMaps();

        render_mipmaps.use();
        render_mipmaps.set_uniform("viewMatrices", radiance_map_view_matrices, LAYERS);
        render_mipmaps.set_uniform("layers", LAYERS);
        //arraytexmap.generateMipMaps();

        //disabling depth test so we do not need a special renderbuffer for depth.
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);

        for(int level = 0; level < MIPMAPS; level++)
        {
            int viewport_size = ARRAY_TEXTURE_SIZE >> (level + 1);
            glViewport(0, 0, viewport_size, viewport_size);

            // image processing of mipmaps
            screen_quad->mesh.getMaterial()->removeTexture(string("colorMap"));
            GLuint sourceTex = 0;
            if(level == 0)
            {
                sourceTex = radiance_map.getColorTexture()->get_id();
            }
            else
            {
                sourceTex = radiance_map_mipmaps->getColorTexture(level - 1)->get_id();
            }

            screen_quad->mesh.getMaterial()->addTexture(new Mesh::Texture("colorMap", sourceTex, GL_TEXTURE_2D_ARRAY));

            render_mipmaps.set_uniform("scaling", 2 << (level + 1));
            render_mipmaps.set_uniform("texStep", 1.0f / ARRAY_TEXTURE_SIZE);

            radiance_map_mipmaps->enableUniqueTarget(level);
            check_gl_error();
            screen_quad->display(render_mipmaps);

//            performanceTimer.registerEvent(QString("3.0.%1: Compute mipmaps - shader").arg(QString::number(level)).toStdString());
        }

        // re-enabling depth test for final rendering
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
*/
        radiance_map.enableMipMaps();
        performanceTimer.registerEvent("3: Compute mipmaps");

        // Adding the new calculated stuff.
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
        glViewport(0,0,window_width,window_height);

        //glBindTexture(GL_TEXTURE_2D_ARRAY, arraytexmap.getColorTexture()->get_id());
        //float converg = currentFrame / float(CONVERGENCE_FRAMES);
        //glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_LOD_BIAS, 3.0f * (1 - converg * converg * converg));
        //glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

    }

    render_combination.use();
    render_combination.set_uniform("shadow_bias", params->shadow_bias);
    render_combination.set_uniform("epsilon_combination", params->epsilon_combination);
    render_combination.set_uniform("one_over_max_samples", 1.0f/params->maxsamples);
    render_combination.set_uniform("mipmap_LOD",params->LOD);
    render_combination.set_uniform("current_frame_rev", 1.0f/min(currentFrame + 1,CONVERGENCE_FRAMES));
    render_combination.set_uniform("current_frame", min(currentFrame + 1,CONVERGENCE_FRAMES));
    render_combination.set_uniform("cameraMatrices", radiance_map_project_view_matrices,LAYERS);
    render_combination.set_uniform("has_environment", params->environment);
    float worldCircleRadius = MAX_RADIUS;
    render_combination.set_uniform("disc_area", (float)(worldCircleRadius * worldCircleRadius * M_PI));
    render_combination.set_uniform("step_tex", 1.0f/ARRAY_TEXTURE_SIZE);
    render_combination.set_uniform("skybox_dim", Vec2f(skybox->width,skybox->height));
    render_combination.set_uniform("samples",samples_per_texel);
    render_combination.set_uniform("combined_uniform_coefficient", (float)(scattering_material->global_coeff * worldCircleRadius * worldCircleRadius * M_PI *  (1.0f/(samples_per_texel*min(currentFrame + 1,CONVERGENCE_FRAMES))) ));
    set_light_and_camera(render_combination);

    obj->display(render_combination);

    if(currentFrame < CONVERGENCE_FRAMES)
    {
        //glBindTexture(GL_TEXTURE_2D_ARRAY, arraytexmap.getColorTexture()->get_id());
        //glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_LOD_BIAS, 0.0f);
        //glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    }


    if((currentFrame - 1) % 5 == 0 && currentFrame < 0)
    {
        QImage * screen = takeScreenshot();
        QString name = QString("C:/Users/alessandro/Desktop/test_convergence/test_jeppe_100_%1.png").arg(currentFrame - 1);
        screen->save(name);
        delete screen;
    }

    currentFrame++;
    performanceTimer.registerEvent("4: Combination");

#ifdef DEBUG_SCREEN

#ifdef MEMORY_INFO
    print_memory_info_nvidia();
#endif

    screen_quad_display_shader.use();
    screen_quad_display_shader.set_uniform("mipmap_LOD",params->LOD);
    scattering_material->loadUniforms(screen_quad_display_shader);
    screen_quad_display_shader.set_uniform("current_frame_rev", 1.0f/min(currentFrame,CONVERGENCE_FRAMES));
    screen_quad_display_shader.set_uniform("points",samples_per_texel);
    screen_quad_display_shader.set_uniform("a_m",params->a);
    screen_quad_display_shader.set_uniform("b_m",params->b);
    if(params->debugOverlayVisible)
        screen_quad->display(screen_quad_display_shader);
#endif
}

QImage* TranslucentMaterials::takeScreenshot()
{
    float *pixels;
    // the thingy we use to write files

    uint w = window_width;
    uint h = window_height;
    pixels = new float[w*h*3]; //3 floats = RGB
    // read in the pixel data, TGA's pixels are BGR aligned
    glBindFramebuffer(GL_READ_FRAMEBUFFER,0);
    glReadPixels(0, 0, w, h, GL_RGB, GL_FLOAT, pixels);

    QImage * res = new QImage(w,h,QImage::Format_RGB32);

    for(uint i = 0; i < w; i++)
    {
        for(uint j = 0; j < h; j++)
        {
            int index = (i * h + j) * 3;
            int red = (int)(pixels[index] * 255.0f);
            int green = (int)(pixels[index + 1] * 255.0f);
            int blue = (int)(pixels[index + 2] * 255.0f);
            QRgb color = qRgb(red,green,blue);
            res->setPixel(j,w-i-1,color);
        }
    }
    return res;
}

Vec4f TranslucentMaterials::getClearColor() const
{
    return clearColor;
}

void initCubeMap(GLuint * tex, GLenum * type)
{
    GLuint cubetex;
    check_gl_error();
    glGenTextures(1, &cubetex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubetex);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    check_gl_error();
    string names[6] =
    {
        string("grace_cross_px.png"),
        string("grace_cross_mx.png"),
        string("grace_cross_py.png"),
        string("grace_cross_my.png"),
        string("grace_cross_pz.png"),
        string("grace_cross_mz.png")
    };

    ResourceLoader r;
    string base_path = r.compute_resource_path("./images/");
    check_gl_error();

    for(int i = 0; i < 6; i++)
    {
        string path = base_path + names[i];
        QImage res;
        QImage * image = new QImage();
        if(image->load(QString(path.c_str()))) {
            res = QGLWidget::convertToGLFormat(*image);
        }
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA32F, res.width(), res.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, res.bits());
        check_gl_error();
    }
    check_gl_error();
    *tex = cubetex;
    *type = GL_TEXTURE_CUBE_MAP;
}

void initRectangle(GLuint * tex, GLenum * type, QImage & img)
{
    GLuint cubetex;
    check_gl_error();
    glGenTextures(1, &cubetex);
    glBindTexture(GL_TEXTURE_RECTANGLE, cubetex);
    glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    string name = string("uffizi.png");

    ResourceLoader r;
    string base_path = r.compute_resource_path("./images/");
    string path = base_path + name;

    QImage res;
    QImage * image = new QImage();
    if(image->load(QString(path.c_str()))) {
        res = QGLWidget::convertToGLFormat(*image);
    }
    glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA16F, res.width(), res.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, res.bits());

    check_gl_error();
    *tex = cubetex;
    *type = GL_TEXTURE_RECTANGLE;
    img = res;
}

void TranslucentMaterials::paintGL()
{

    static GLGraphics::ShaderProgramDraw skybox_shader(shader_path, "ss_skybox.vert", "", "ss_skybox.frag");
    static bool mark = false;
    if(reload_shaders)
        skybox_shader.reload();


    static vector<Vec4f> light_dirs;
    if(!mark)
    {
        GLuint cubetex;
        GLenum type;

        QImage img;
        initRectangle(&cubetex,&type,img);
        InfiniteAreaLight i (img);
        vector<Light *> lights;

        i.getLights(img, lights,LIGHTS);

        //initCubemap(&cubetex,&type);
        skybox = new Mesh::Texture("skybox", cubetex, type);
        skybox->width = img.width();
        skybox->height = img.height();

        Mesh::Material * e = new Mesh::Material();
        e->addTexture(skybox);
        e->addUniform("cubemap_size", Vec2f(img.width(),img.height()));


        for(int i = 0; i < LIGHTS; i++)
        {
#ifdef DIR
            manager.addLight(*lights[i]);
#endif
            light_dirs.push_back(lights.at(i)->position);
        }

        skybox_cube->init(" ", "light_sphere", *e);
        skybox_cube->setScale(Vec3f(20.f));
        skybox_cube->setTranslation(Vec3f(0.0f,0.0f,0.0f));
        skybox_cube->enabled = true;
        mark = true;
    }



    //manager[0].position = Vec4f(6.0f * sin(float(frame) / 100.0f * 2 * M_PI), 0.0f, 6.0f * cos(float(frame) / 100.0f * 2 * M_PI), 1.0f);
    //manager.reloadLights();
    //currentFrame = 0;

    performanceTimer.start();


    //setup_shadow(reload_shaders);

    glClearColor(clearColor[0],clearColor[1],clearColor[2],clearColor[3]);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);



    if(isGridVisible) draw_grid(reload_shaders);
    if(areAxesVisible) draw_axes(reload_shaders);

    //draw_bounding_boxes(reload_shaders);

    static ShaderProgramDraw render_to_cubemap_jensen(shader_path,"ss_array_render_to_arraymap.vert","ss_array_render_to_arraymap.geom","ss_array_render_to_arraymap_multilight_standard.frag");
    static ShaderProgramDraw render_to_cubemap_jeppe(shader_path,"ss_array_render_to_arraymap.vert","ss_array_render_to_arraymap.geom","ss_array_render_to_arraymap_multilight_directional_spectral.frag");

    switch(render_mode)
    {
    case DRAW_JENSEN:
        render_dirss(reload_shaders,render_to_cubemap_jensen);
        break;
    case DRAW_BETTER:
        //render_direct_test(reload_shaders, render_to_cubemap_jensen);
        break;
    case DRAW_DIRECTIONAL:
        render_dirss(reload_shaders, render_to_cubemap_jeppe);
        break;
    }

    reload_shaders = false;

    if(params->environment)
    {
        skybox_shader.use();
        skybox_shader.set_uniform("dir_lights", light_dirs, LIGHTS);
        set_light_and_camera(skybox_shader);
        skybox_cube->display(skybox_shader);
    }

    check_gl_error();

    performanceTimer.end();
    frame++;
    // TODO REDO emit timeMeasurement(msecs_total[25]);

}

void TranslucentMaterials::resizeGL(int W, int H)
{
    window_width = W;
    window_height = H;
    glViewport(0, 0, window_width, window_height);
    reload_shaders = true;
}

void TranslucentMaterials::animate()
{
    user.update_position();
    repaint();
}

void TranslucentMaterials::setClearColor(Vec4f & color)
{
    this->clearColor = color;
}

void TranslucentMaterials::setUserPosition(Vec3f &position)
{
    user.stop();
    user.set(position,user.get_dir());
}

void TranslucentMaterials::setUserDirection(Vec3f &direction)
{
    user.stop();
    user.set(user.getPos(),direction);
}

void TranslucentMaterials::setLightIntensity(float intensity)
{
    if(manager.size() == 0)
        return;
    for(int i = 0; i < manager.size(); i++)
        manager[i].intensity = intensity;
    manager.reloadLights();
}

void TranslucentMaterials::setShadows(bool shadows)
{
    isShadow = shadows;
}

void TranslucentMaterials::setVertexPixelMode(bool isVertex)
{
    isVertexMode = isVertex;
}

void TranslucentMaterials::setGridVisible(bool isVisible)
{
    isGridVisible = isVisible;
}

void TranslucentMaterials::setAxesVisible(bool areVisible)
{
    areAxesVisible = areVisible;
}

void TranslucentMaterials::initializeGL()
{
    setup_gl();
    glClearColor( 0.7f, 0.7f, 0.7f, 0.0f );
    glEnable(GL_DEPTH_TEST);

    if(glDebugMessageCallback){
            cout << "Register OpenGL debug callback " << endl;
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(openglCallbackFunction, nullptr);
            GLuint unusedIds = 0;
            glDebugMessageControl(GL_DONT_CARE,
                GL_DONT_CARE,
                GL_DONT_CARE,
                0,
                &unusedIds,
                true);
        }
        else
            cout << "glDebugMessageCallback not available" << endl;
}

void TranslucentMaterials::mousePressEvent(QMouseEvent *m)
{
    mouse_x0 = m->x();
    mouse_y0 = m->y();
}

void TranslucentMaterials::mouseReleaseEvent(QMouseEvent *m)
{
    float delta_ang_x = 4*M_PI*(mouse_x0-m->x())/window_width;
    float delta_ang_y = M_PI*(mouse_y0-m->y())/window_height;
    ang_x += delta_ang_x;
    ang_y += delta_ang_y;
}

void TranslucentMaterials::mouseMoveEvent(QMouseEvent *m)
{
    float delta_ang_x = 4*M_PI*(mouse_x0-m->x())/window_width;
    float delta_ang_y = M_PI*(mouse_y0-m->y())/window_height;
    ax = ang_x + delta_ang_x;
    ay = ang_y + delta_ang_y;

    Vec3f dir(cos(ax)*cos(ay),sin(ax)*cos(ay),sin(ay));
    user.set_dir(dir);
    emit userDirectionChanged(dir);
    QWidget::repaint();
}

void TranslucentMaterials::keyPressEvent(QKeyEvent *e)
{
    static bool is_full_screen = false;
    switch(e->key())
    {
    case Qt::Key_Escape: exit(0);
    case ' ':
        render_method = static_cast<RenderMethod>((static_cast<int>(render_method)+1) % RENDER_METHODS);
        reload_shaders = true;
        break;
    case 'W':
    {
        user.forward();
        emit userPositionChanged(user.getPos());
    }
        break;
    case 'S':
    {
        user.back();
        emit userPositionChanged(user.getPos());

    }
        break;
    case 'A':
    {
        user.strafe_left();
        emit userPositionChanged(user.getPos());

    }
        break;
    case 'D':
    {
        user.strafe_right();
        emit userPositionChanged(user.getPos());

    }
        break;
    case 'F':
        if(is_full_screen) {
            showNormal();
            is_full_screen = false;
        }
        else
        {
            showMaximized();
            is_full_screen = true;
        }
        break;
    case 'R':
        reload_shaders = true;
        //params->currentFlags |= TranslucentParameters::SAMPLES_CHANGED;
        performanceTimer.refresh();
        ShaderProgram::preprocessor.reload();

        break;
    case 'L':
    {
        Vec3f v = normalize(-user.get_dir() + Vec3f(0,0,1));
        manager[0].position = Vec4f(v[0], v[1], v[2], 0);
        manager.reloadLights();
    }
        break;
    case 'O':
        //user.set(Vec3f(-7.60765f, 14.907f, 4.37377f), Vec3f(0.333226f, -0.925571f, 0.179661f));
        break;
    case '+':
        user.set_height(0.2f);
        emit userPositionChanged(user.getPos());

        break;
    case '-':
        user.set_height(-0.2f);
        emit userPositionChanged(user.getPos());

        break;
    case 'I':
        cout << user.get_pos() << " " << user.get_dir() << endl;
    default:
        QWidget::keyPressEvent(e);
        break;
    }
    //QWidget::repaint();
}

void TranslucentMaterials::keyReleaseEvent(QKeyEvent *)
{
    user.stop();
}

ThreeDObject *TranslucentMaterials::getDefaultObject()
{
    return currentObject;
}

ThreeDObject * TranslucentMaterials::getObject(string name)
{
    vector<ThreeDObject*>::iterator it;
    it = std::find_if(objectPool.begin(),objectPool.end(), CompareThreeD(name));
    if(it != objectPool.end())
    {
        return *it;
    }
    return nullptr;
}

TranslucentParameters *TranslucentMaterials::getParameters()
{
    return params;
}

void TranslucentMaterials::setRenderMode(RenderMode mode)
{
    this->render_mode = mode;
}

ThreeDObject* TranslucentMaterials::setObject(QString &object)
{
    glUseProgram(0);
    //currentObject->mesh.disable();
    currentObject = getObject(object.toStdString());
    reload_shaders = true;
    return currentObject;
}
