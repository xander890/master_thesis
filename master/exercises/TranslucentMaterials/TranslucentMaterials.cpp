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
#include "arrayimagebuffer.h"
#include "GLGraphics/computeshader.h"
#include "shaderpreprocessor.h"
#include "GLGraphics/ResourceLoader.h"
#include "mipmapgeneratorview.h"

#define BUNNIES
#define POINT_DIST 0 // 0 random, 1 exponential, 2 uniform
#define TIMER

#define GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX 0x9047
#define GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX 0x9048
#define GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX 0x9049
#define GPU_MEMORY_INFO_EVICTION_COUNT_NVX 0x904A
#define GPU_MEMORY_INFO_EVICTED_MEMORY_NVX 0x904B

//#define SINGLE_LIGHT

using namespace std;
using namespace CGLA;
using namespace GLGraphics;

const string shader_path = "./shaders/TranslucentMaterials/";
const string objects_path = "./data/";
const string default_obj = "bunny1";
static const Vec4f light_ambient(0.3f,0.4f,0.6f,0.4f);

Terrain terra(30,0.025f);

User user (&terra);
bool reload_shaders = true;

LightManager manager;

const Vec4f light_specular(0.6f,0.6f,0.3f,0.6f);
const Vec4f light_diffuse(.5f,.5f,.5f,1.0f);
const Vec4f light_position(6.f,0.f,0.f,1);
const Vec4f light_diffuse_2(0.7,0.5,0.5,0.0);
const Vec4f light_position_2(6.0,0.0,0.0,1.0);


TranslucentMaterials::TranslucentMaterials( QWidget* parent)
    : QGLWidget( new Core4_3_context(), (QWidget*) parent),
      ax(0), ay(0), dist(12),ang_x(0),ang_y(0),mouse_x0(0),mouse_y0(0)
    , clearColor(Vec4f(0.0f,0.0f,0.0f,1.0f)), frame(0),
      isVertexMode(true),
      isShadow(true),
      isGridVisible(true),
      areAxesVisible(true),
      params(new TranslucentParameters()),
      render_mode(DRAW_JENSEN),
      render_method(CUBEMAP_BASE),
      currentFrame(0),
      performanceTimer(PerformanceTimer(20)),
      skybox_cube(new ThreeDSphere(30))
    {

        #ifndef TIMER
            performanceTimer.setIntermediateEnabled(false);
        #endif
        Light mainLight (light_position, light_diffuse, 1.0f, light_specular, true);
        Light secondaryLight(light_position_2, light_diffuse_2, 15.0f, Vec4f(0.0f), true);
        manager.addLight(mainLight);
        //manager.addLight(secondaryLight);
        setFocusPolicy(Qt::ClickFocus);
        timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(animate()));
        timer->start(16);
    }


void TranslucentMaterials::draw_objects(ShaderProgramDraw& shader_prog)
{
    vector<string> objs;
    objs.push_back(default_obj);
    draw_objects(shader_prog,objs);
}

void TranslucentMaterials::draw_objects(ShaderProgramDraw& shader_prog, vector<string>& objectsToDraw)
{
    for(unsigned int i=0; i<objects.size();++i)
    {
        ThreeDObject * obj = objects[i];
        if(obj->enabled)
            obj->display(shader_prog);
    }
}



void TranslucentMaterials::set_light_and_camera(ShaderProgramDraw& shader_prog)
{

    shader_prog.set_projection_matrix(perspective_Mat4x4f(53.1301, float(window_width)/window_height, 0.1f, 10));


    shader_prog.set_view_matrix(user.get_view_matrix());

    shader_prog.set_uniform("light_amb", light_ambient);
    shader_prog.set_uniform("user_pos",user.get_pos());
    shader_prog.set_uniform("gamma",params->gamma);
    manager.loadLights(shader_prog);
}


void TranslucentMaterials::render_jensen(bool reload)
{
    // Load shaders for terrain and general objects. The terrain color is computed
    // procedurally, so we need a different shader.

    static ShaderProgramDraw terrain_shader(shader_path, "terrain.vert", "", "terrain.frag");
    static ShaderProgramDraw object_shader(shader_path, "object.vert", "", "object.frag");
    static ShaderProgramDraw plane_shader(shader_path, "plane.vert", "", "plane.frag");

    static ShaderProgramDraw jensen_shader(shader_path, "jensen_dipole_gpu.vert", "", "jensen_dipole_gpu.frag");
    static ShaderProgramDraw jensen_shader_vertex(shader_path, "jensen_dipole_gpu_vertex.vert", "", "jensen_dipole_gpu_vertex.frag");


    if(reload)
    {
        jensen_shader.reload();
        jensen_shader_vertex.reload();
    }


    if(isVertexMode)
    {
        jensen_shader_vertex.use();
        set_light_and_camera(jensen_shader_vertex);
        draw_objects(jensen_shader_vertex);

    }
    else
    {
        jensen_shader.use();
        set_light_and_camera(jensen_shader);
        draw_objects(jensen_shader);

    }
}


void TranslucentMaterials::setup_shadow(bool reload)
{
    const int SHADOW_SIZE = 4096;
    static ShadowBuffer shadow_buffer(SHADOW_SIZE);
    static ShaderProgramDraw render_to_shadow_map(shader_path, "shadow.vert", "", "shadow.frag");

    if(reload)
    {
        shadow_buffer.initialize();
        render_to_shadow_map.reload();
    }

    render_to_shadow_map.use();
    shadow_buffer.enable();

    // Set up a modelview matrix suitable for shadow: Maps from world coords to
    // shadow buffer coords.
    Vec3f v = Vec3f(manager[0].position);
    render_to_shadow_map.set_view_matrix(lookat_Mat4x4f(v,-v,Vec3f(0,1,0))); //PARALLEL!
    render_to_shadow_map.set_model_matrix(identity_Mat4x4f());
    render_to_shadow_map.set_projection_matrix(ortho_Mat4x4f(Vec3f(-10,-10,1),Vec3f(10,10,10)));

    // Switch viewport size to that of shadow buffer.

    glViewport(0, 0, SHADOW_SIZE, SHADOW_SIZE);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    // Draw to shadow buffer.
    for(uint i = 0; i < objects.size(); i++)
    {
        ThreeDObject * o = objects[i];
        if(o->enabled && o->mesh.getMaterial()->cast_shadows)
        {
            o->display(render_to_shadow_map);
        }
    }

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glDrawBuffer(GL_BACK);

    // We need to reset the viewport, since the shadow buffer does not have
    // the same size as the screen window.
    glViewport(0, 0, window_width, window_height);

    Mat4x4f mat = translation_Mat4x4f(Vec3f(0.5));
    mat *= scaling_Mat4x4f(Vec3f(0.5));
    mat *= render_to_shadow_map.get_projection_matrix();
    mat *= render_to_shadow_map.get_view_matrix();

    shadow_buffer.setLightWorldTransformationMatrix(mat);

    for(uint i = 0; i < objects.size(); i++)
    {
        ThreeDObject * o = objects[i];
        Mesh::Material * mat = o->mesh.getMaterial();
        if(o->enabled && mat->receives_shadows)
        {
            mat->setShadowBuffer(&shadow_buffer);
        }
    }
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

    for(int i = 0; i < objects.size(); i++)
    {
        ThreeDObject * o = objects[i];
        if(o->boundingBoxEnabled && o->enabled)
        {
            Mesh::BoundingBox * boundingBox = o->getBoundingBox();
            boundingBoxCubes[i]->setModelView(o->getPosition(),o->getRotation(),o->getScale());

            boundingBoxCubes[i]->setColor(Vec4f(0.0,1.0,0.0,1.0));
            boundingBoxCubes[i]->display(color_shader);
            delete boundingBox;
        }
    }
}

void TranslucentMaterials::render_better_dipole(bool reload)
{
    static ShaderProgramDraw better_dipole_shader(shader_path, "better_dipole_gpu.vert", "", "better_dipole_gpu.frag");
    static ShaderProgramDraw better_dipole_shader_vertex(shader_path, "better_dipole_gpu_vertex.vert", "", "better_dipole_gpu_vertex.frag");


    if(reload)
    {

        better_dipole_shader.reload();
        better_dipole_shader_vertex.reload();
    }

    if(isVertexMode)
    {
        better_dipole_shader_vertex.use();
        set_light_and_camera(better_dipole_shader_vertex);

        draw_objects(better_dipole_shader_vertex);

    }
    else
    {
        better_dipole_shader.use();
        set_light_and_camera(better_dipole_shader);

        draw_objects(better_dipole_shader);
    }
}



bool compareVec2fDistanceAscending (Vec2f i,Vec2f j) { return (i.length() < j.length()); }

void TranslucentMaterials::getDiscPoints(vector<Vec3f> * points, const int n, const int m)
{
    getDiscPoints(points,n,m,0.0f);
}

void TranslucentMaterials::getDiscPoints(vector<Vec3f> * points, const int n, const int m, float sigma_tr)
{
    vector<vector<Vec2f> > texture;

//#if POINT_DIST == 0
    planeHaltonCircleRejectionExponentialMulti(texture, n, m, sigma_tr);
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
    for(int i = 0 ; i < n; i++) cout << texture[0][i][0] << " " << texture[0][i][1] << endl;

    for(int k = 0; k < m; k++)
    {
        //cout << "Vector " << k <<endl;
        if( k == 0)
        {
            //cout << "Vector, " << n << " points" << endl;
        }

        vector<Vec2f> discpoints = texture[k];
        std::sort(discpoints.begin(),discpoints.end(),compareVec2fDistanceAscending);
        for(int i = 0; i < discpoints.size(); i++)
        {
            if (k == 0)
            {
                //cout <<discpoints[i][0] << " " << discpoints[i][1] << endl;
            }
            points->push_back(Vec3f(discpoints[i][0],discpoints[i][1],0.0f));

        }
    }
}

void print_memory_info_nvidia()
{
    GLint data[5];
    const char * messages[5] = {"Dedicated video memory: %1 Mb", "Total available memory: %1 Mb", "Current available video memory: %1 Mb", "Eviction count: %1 ", "Evicted memory: %1 Mb"};
    const int divisors[5] = {1024,1024,1024,1,1024};
    glGetIntegerv(GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX, &data[0]);
    glGetIntegerv(GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &data[1]);
    glGetIntegerv(GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &data[2]);
    glGetIntegerv(GPU_MEMORY_INFO_EVICTION_COUNT_NVX, &data[3]);
    glGetIntegerv(GPU_MEMORY_INFO_EVICTED_MEMORY_NVX, &data[4]);
    for(int i = 0; i < 5; i++)
        cout << QString(messages[i]).arg(data[i] / divisors[i]).toStdString() << endl;
    check_gl_error();
}

void TranslucentMaterials::render_direct_compute_time(bool reload, ShaderProgramDraw &render_to_array)
{
    performanceTimer.registerEvent("-1: Start");
#ifdef SINGLE_LIGHT
    static ShaderProgramDraw gbuff_shader(shader_path,"ss_cubemap_gbuffer.vert","","ss_cubemap_gbuffer.frag");
#else
    static ShaderProgramDraw gbuff_shader(shader_path,"ss_cubemap_gbuffer_multilight.vert","ss_cubemap_gbuffer_multilight.geom", "ss_cubemap_gbuffer_multilight.frag");
#endif

    static ComputeShader compute_mipmaps(shader_path, "ss_array_generate_mips_single.compute");

    static ShaderProgramDraw gbuff_quad(shader_path,"ss_cubemap_test_gbuffer.vert","","ss_cubemap_test_gbuffer.frag");
    static ShaderProgramDraw gbuff_wrap(shader_path,"ss_cubemap_test_wrap_gbuffer.vert","","ss_cubemap_test_wrap_gbuffer.frag");

    static ShaderProgramDraw render_to_cubemap_test(shader_path,"ss_cubemap_render_to_cubemap.vert","","ss_cubemap_render_to_cubemap.frag");
    static ShaderProgramDraw render_to_cubemap_test_screen(shader_path,"ss_cubemap_test_render_to_cubemap_screen.vert","","ss_cubemap_test_render_to_cubemap_screen.frag");
    static ShaderProgramDraw render_to_cubemap_test_cube(shader_path,"ss_cubemap_test_render_to_cubemap_cube.vert","","ss_cubemap_test_render_to_cubemap_cube.frag");
    static ShaderProgramDraw render_combination(shader_path,"ss_array_combination.vert","","ss_array_combination.frag");

    static ShaderProgramDraw render_mipmaps(shader_path,"display_tex.vert","","display_tex.frag");
    static ThreeDPlane * screen_quad = new ThreeDPlane();
    static Mesh::Material * test_mat = new Mesh::Material();
    static ShaderProgramDraw test2(shader_path,"display_tex_2.vert","","display_tex_2.frag");

    const int GBUFFER_SIZE = 1024;
    const float LIGHT_CAMERA_SIZE = 3.0f;



    const int ARRAY_TEXTURE_SIZE = 1024;
    const int LAYERS = 10;

    const int MIPMAPS = 3;
    const int SCALING [MIPMAPS] = {2, 4, 8};
    static MipMapGenerator mipmaps [MIPMAPS] = {MipMapGenerator(ARRAY_TEXTURE_SIZE/SCALING[0], LAYERS, 1), MipMapGenerator(ARRAY_TEXTURE_SIZE/SCALING[1], LAYERS, 1), MipMapGenerator(ARRAY_TEXTURE_SIZE/SCALING[2], LAYERS, 1)};

    static ArrayTextureBuffer arraytexmap(ARRAY_TEXTURE_SIZE,LAYERS,1);
    static ArrayTextureBuffer arraytexmap_back(ARRAY_TEXTURE_SIZE,LAYERS,1);
    ArrayTextureBuffer * front;
    ArrayTextureBuffer * back;

#ifdef SINGLE_LIGHT
    static VertexNormalBuffer light_buffer(GBUFFER_SIZE);
#else
    static ArrayVertexNormalBuffer light_buffer(GBUFFER_SIZE, 4);
#endif

    const float CAMERA_DISTANCE = 6.0f; //This should not matter (can be DIST = max bounding box + camera near + epsilon
    const float CAMERA_NEAR = 1.0f;
    const float CAMERA_FAR = 21.0f;
    const float CAMERA_SIZE = 6.0f;
    const int CONVERGENCE_FRAMES = 100;

    int discPoints = params->samples / manager.size();
    const int DISCS = LAYERS;

    //TODO more objs
    ThreeDObject * obj = objects[0];
    for(int i = 0; i < objects.size(); i++)
    {
        ThreeDObject * o = objects[i];
        if(o->enabled)
        {
            obj = o;
            break;
        }
    }
    Mesh::Material * scattering_material = obj->mesh.getMaterial();


    if(reload)
    {
        //test.reload();
        compute_mipmaps.reload();
        test2.reload();
        gbuff_shader.reload();
        gbuff_quad.reload();
        gbuff_wrap.reload();
        render_to_array.reload();
        render_to_cubemap_test_screen.reload();
        render_to_cubemap_test_cube.reload();
        render_combination.reload();
        render_mipmaps.reload();
        currentFrame = 0;
    }

    Vec3f center = obj->getCenter();

    static Vec3f cameraPositions[LAYERS] = {
        center + Vec3f(1,0,0) * CAMERA_DISTANCE, //+X
        center - Vec3f(1,0,0) * CAMERA_DISTANCE, //-X
        center + Vec3f(0,1,0) * CAMERA_DISTANCE, //+Y
        center - Vec3f(0,1,0) * CAMERA_DISTANCE, //-Y
        center + Vec3f(0,0,1) * CAMERA_DISTANCE, //+Z
        center - Vec3f(0,0,1) * CAMERA_DISTANCE,  //-Z
        center + Vec3f(1.0f/sqrt(2.0f),1.0f/sqrt(2.0f),0) * CAMERA_DISTANCE,
        center + Vec3f(-1.0f/sqrt(2.0f),-1.0f/sqrt(2.0f),0) * CAMERA_DISTANCE,
        center + Vec3f(-1.0f/sqrt(2.0f),1.0f/sqrt(2.0f),0) * CAMERA_DISTANCE,
        center + Vec3f(1.0f/sqrt(2.0f),-1.0f/sqrt(2.0f),0) * CAMERA_DISTANCE
    };

    static Mat4x4f viewMatrices[LAYERS]  = {
        scaling_Mat4x4f(Vec3f(1,-1,1)) * lookat_Mat4x4f_target(cameraPositions[0], center, Vec3f(0,1,0)), //+X
        scaling_Mat4x4f(Vec3f(1,-1,1)) * lookat_Mat4x4f_target(cameraPositions[1], center, Vec3f(0,1,0)), //-X
        scaling_Mat4x4f(Vec3f(-1,1,1)) * lookat_Mat4x4f_target(cameraPositions[2], center, Vec3f(0,0,1)), //+Y
        scaling_Mat4x4f(Vec3f(-1,1,1)) * lookat_Mat4x4f_target(cameraPositions[3], center, Vec3f(0,0,-1)), //-Y
        scaling_Mat4x4f(Vec3f(1,-1,1)) * lookat_Mat4x4f_target(cameraPositions[4], center, Vec3f(0,1,0)), //+Z
        scaling_Mat4x4f(Vec3f(1,-1,1)) * lookat_Mat4x4f_target(cameraPositions[5], center, Vec3f(0,1,0)),  //-Z
        scaling_Mat4x4f(Vec3f(1,-1,1)) * lookat_Mat4x4f_target(cameraPositions[6], center, Vec3f(0,1,0)),
        scaling_Mat4x4f(Vec3f(1,-1,1)) * lookat_Mat4x4f_target(cameraPositions[7], center, Vec3f(0,1,0)),
        scaling_Mat4x4f(Vec3f(1,-1,1)) * lookat_Mat4x4f_target(cameraPositions[8], center, Vec3f(0,1,0)),
        scaling_Mat4x4f(Vec3f(1,-1,1)) * lookat_Mat4x4f_target(cameraPositions[9], center, Vec3f(0,1,0))
    };

    vector<Mat4x4f> planeTransformMatrices(LAYERS);
    vector<Mat4x4f> viewMatricesvector(LAYERS);

    Mat4x4f model_identity = identity_Mat4x4f();
    Mat4x4f projection_array = ortho_Mat4x4f(Vec3f(-CAMERA_SIZE,-CAMERA_SIZE,CAMERA_NEAR),Vec3f(CAMERA_SIZE,CAMERA_SIZE,CAMERA_FAR));
    Mat4x4f projection_light = ortho_Mat4x4f(Vec3f(-LIGHT_CAMERA_SIZE,-LIGHT_CAMERA_SIZE,1),Vec3f(LIGHT_CAMERA_SIZE,LIGHT_CAMERA_SIZE,10));


    Mat4x4f mat2 = translation_Mat4x4f(Vec3f(0.5));
    mat2 *= scaling_Mat4x4f(Vec3f(0.5));
    mat2 *= projection_array;

    for(int i = 0; i < LAYERS; i++)
    {
        planeTransformMatrices[i] = mat2 * viewMatrices[i];
        viewMatricesvector[i] = viewMatrices[i];
    }

    performanceTimer.registerEvent("0: Initialization");

    if(currentFrame < CONVERGENCE_FRAMES)
    {
        static bool initialized = false;

        // Need to disable mipmap afterwards, otherwise the memory space is not reserved.
        arraytexmap.disableMipMaps();
        arraytexmap_back.disableMipMaps();

        if(!initialized)
        {
            initialized = true;
            screen_quad->init("","plane",*test_mat);
            vector<Vec3f> * discpoint_data = new vector<Vec3f>();
            getDiscPoints(discpoint_data,discPoints,DISCS);

            Mesh::Texture * tex = new Mesh::Texture("discpoints",GL_TEXTURE_2D, discPoints, DISCS, *discpoint_data);
            tex->init();
            scattering_material->addTexture(tex);

            //preparing first color buffer from which to read (just for avoiding errors,
            // the shader already avoids that)
            scattering_material->addTexture(arraytexmap_back.getColorTexture());
        }

        if(params->currentFlags & TranslucentParameters::SAMPLES_CHANGED)
        {
            vector<Vec3f> * discpoint_data = new vector<Vec3f>();
            getDiscPoints(discpoint_data,discPoints,DISCS);
            Mesh::Texture * tex = scattering_material->getTexture(string("discpoints"));
            tex->reloadData(*discpoint_data,discPoints,DISCS);
            params->currentFlags &= ~(TranslucentParameters::SAMPLES_CHANGED);
        }


        gbuff_shader.use();


        // Set up a modelview matrix suitable for shadow: Maps from world coords to
        // shadow buffer coords.

        gbuff_shader.set_model_matrix(model_identity);
        gbuff_shader.set_projection_matrix(projection_light);

#ifdef SINGLE_LIGHT
        Vec3f v = Vec3f(manager[0].position);
        gbuff_shader.set_view_matrix(lookat_Mat4x4f(v,-v,Vec3f(0,1,0))); //PARALLEL!
        Mat4x4f mat = translation_Mat4x4f(Vec3f(0.5));
        mat *= scaling_Mat4x4f(Vec3f(0.5));
        mat *= gbuff_shader.get_projection_matrix();
        mat *= gbuff_shader.get_view_matrix();
        scattering_material->addUniform("lightMatrix",mat);
#else

        vector<Mat4x4f> lightMatrices;
        vector<Mat4x4f> inverseLightMatrices;
        for(int i = 0; i < manager.size(); i++)
        {
            Vec3f light_dir = Vec3f(manager[i].position);
            Mat4x4f V = lookat_Mat4x4f(light_dir,-light_dir,Vec3f(0,1,0));
            lightMatrices.push_back(V); //PARALLEL!

            Mat4x4f mat = translation_Mat4x4f(Vec3f(0.5));
            mat *= scaling_Mat4x4f(Vec3f(0.5));
            mat *= projection_light;
            mat *= V;
            inverseLightMatrices.push_back(mat);

        }
        gbuff_shader.set_uniform("lightMatrices",lightMatrices,manager.size());
        gbuff_shader.set_uniform("layers", manager.size());
#endif
        // Switch viewport size to that of shadow buffer.

        glViewport(0, 0, GBUFFER_SIZE, GBUFFER_SIZE);

        // Draw to shadow buffer.
        light_buffer.enable();
        obj->display(gbuff_shader);

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glDrawBuffer(GL_BACK);
        performanceTimer.registerEvent("1: Render to lightmap");
        // We need to reset the viewport, since the shadow buffer does not have
        // the same size as the screen window.
        glViewport(0, 0, window_width, window_height);



        Mesh::Texture * vtex = light_buffer.getVertexTexture();
        Mesh::Texture * ntex = light_buffer.getNormalTexture();

        scattering_material->addTexture(vtex);
        scattering_material->addTexture(ntex);

        render_to_array.use();


        glViewport(0,0,ARRAY_TEXTURE_SIZE,ARRAY_TEXTURE_SIZE);


        Vec3f radius = Vec3f(29.909f,23.316f, 18.906f); //radius for marble - red 29.909 green 23.316 blue 18.906
        //float trueRadius = clamp01(length(mat * Vec4f(radius[0],0,0,0)));

        float trueRadius = params->circleradius;
        glClearColor(0,0,0,0);

        //render_to_cubemap.set_uniform("discpoints", discpoints, DISC_POINTS);
        render_to_array.set_uniform("one_over_max_samples",1.0f/params->samples);
        render_to_array.set_uniform("one_over_discs",1.0f/DISCS);
        render_to_array.set_uniform("samples",params->samples);
        render_to_array.set_uniform("discradius",trueRadius);
        render_to_array.set_uniform("epsilon_gbuffer", params->epsilon_gbuffer);

#ifndef SINGLE_LIGHT
        render_to_array.set_uniform("lightMatrices",inverseLightMatrices, manager.size());
#endif
        render_to_array.set_uniform("epsilon_combination", params->epsilon_combination);
        render_to_array.set_uniform("cameraMatrices", planeTransformMatrices,LAYERS);
        render_to_array.set_uniform("current_frame", currentFrame);
        render_to_array.set_uniform("convergence_frames", CONVERGENCE_FRAMES);

        set_light_and_camera(render_to_array);

        bool isFrontArrayMap = ((currentFrame % 2) == 0)? true : false;


        // ping-pong between buffers

        if(isFrontArrayMap)
        {
            front = &arraytexmap;
            back = &arraytexmap_back;
        }
        else
        {
            front = &arraytexmap_back;
            back = &arraytexmap;
        }

        //Render to front from back

        render_to_array.set_uniform("viewMatrices", viewMatricesvector, LAYERS);
        render_to_array.set_uniform("layers", LAYERS);
        render_to_array.set_model_matrix(model_identity);
        render_to_array.set_projection_matrix(projection_array);
        front->enable();
        obj->display(render_to_array);

        performanceTimer.registerEvent("2: Render to array");

        front->enableMipMaps();
        compute_mipmaps.use();
        GLuint prog = compute_mipmaps.id();
        GLuint colTex = front->getColorTexture()->get_id();

        GLint location_src = compute_mipmaps.get_uniform_location("source");
        GLint location_dest = compute_mipmaps.get_uniform_location("dest");

        const int KERNEL_XY = 8;
        const int KERNEL_Z = 1;
#define MULTIMIPMAP
#ifdef MULTIMIPMAP
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D_ARRAY, colTex);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D_ARRAY, colTex);

        for(int i = 0; i < MIPMAPS + 1; i++)
            glBindImageTexture(	i, colTex, i, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA32F);


        for(int i = 0; i < MIPMAPS; i++)
        {
            int pow = 2 << i;

            glUniform1i(location_src, i);
            glUniform1i(location_dest, i+1);
            glDispatchCompute(ARRAY_TEXTURE_SIZE / (pow * KERNEL_XY), ARRAY_TEXTURE_SIZE / (pow * KERNEL_XY), LAYERS / KERNEL_Z);
            glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        }
#else

        const char * names[10] = {"source","mipmap0","mipmap1","mipmap2","mipmap3","mipmap4","mipmap5","mipmap6","mipmap7","mipmap8"};
        glMemoryBarrier(GL_ALL_BARRIER_BITS);
        for(int i = 0; i < MIPMAPS + 1; i++)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D_ARRAY, colTex);
            glBindImageTexture(	i, colTex, i, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA32F);
            glUniform1i(compute_mipmaps.get_uniform_location(names[i]), i);
        }

        glDispatchCompute(ARRAY_TEXTURE_SIZE / (2 * KERNEL_XY), ARRAY_TEXTURE_SIZE / (2 * KERNEL_XY), LAYERS / KERNEL_Z);
        glMemoryBarrier(GL_ALL_BARRIER_BITS);

#endif

        performanceTimer.registerEvent("3: Compute mipmaps");

        test_mat->removeTexture(string("colorMap"));
        test_mat->addTexture(front->getColorTexture());
        test_mat->addTexture(front->getDepthTexture());

        // Adding the new calculated stuff.
        Mesh::Texture * color = front->getColorTexture();
        string colname = color->get_name();
        scattering_material->removeTexture(colname); //switching the old color TODO : replace
        scattering_material->addTexture(color);

        Mesh::Texture * depth = front->getDepthTexture();
        scattering_material->addTexture(depth);

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
        glViewport(0,0,window_width,window_height);

        glBindTexture(GL_TEXTURE_2D_ARRAY, front->getColorTexture()->get_id());
        float converg = currentFrame / float(CONVERGENCE_FRAMES);
        glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_LOD_BIAS, 3.0f * (1 - converg * converg * converg));
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    }


    render_combination.use();
    render_combination.set_uniform("shadow_bias", params->shadow_bias);
    render_combination.set_uniform("epsilon_combination", params->epsilon_combination);
    render_combination.set_uniform("one_over_max_samples", 1.0f/params->samples);
    render_combination.set_uniform("mipmap_LOD",params->LOD);
    render_combination.set_uniform("current_frame_rev", 1.0f/min(currentFrame + 1,CONVERGENCE_FRAMES));

    render_combination.set_uniform("cameraMatrices", planeTransformMatrices,LAYERS);
    float worldCircleRadius = params->circleradius * 2 * LIGHT_CAMERA_SIZE;
    render_combination.set_uniform("disc_area", (float)(worldCircleRadius * worldCircleRadius * M_PI));
    render_combination.set_uniform("step_tex", 1.0f/ARRAY_TEXTURE_SIZE);
    set_light_and_camera(render_combination);
    obj->display(render_combination);

    if(currentFrame < CONVERGENCE_FRAMES)
    {
        glBindTexture(GL_TEXTURE_2D_ARRAY, front->getColorTexture()->get_id());
        glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_LOD_BIAS, 0.0f);
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    }


    if((currentFrame - 1) % 5 == 0 && currentFrame < 0)
    {
        QImage * screen = takeScreenshot();
        QString name = QString("C:/Users/alessandro/Desktop/test/test_jensen_100_%1.png").arg(currentFrame - 1);
        screen->save(name);
        delete screen;
    }

    performanceTimer.registerEvent("4: Combination");
    currentFrame++;

    test2.use();
    test2.set_uniform("mipmap_LOD",params->LOD);
    if(params->debugOverlayVisible)
        screen_quad->display(test2);


}

void TranslucentMaterials::render_direct_array(bool reload, ShaderProgramDraw & render_to_array)
{


    static ShaderProgramDraw obj_shader(shader_path,"object.vert","","object.frag");
#ifdef SINGLE_LIGHT
    static ShaderProgramDraw gbuff_shader(shader_path,"ss_cubemap_gbuffer.vert","","ss_cubemap_gbuffer.frag");
#else
    static ShaderProgramDraw gbuff_shader(shader_path,"ss_cubemap_gbuffer_multilight.vert","ss_cubemap_gbuffer_multilight.geom", "ss_cubemap_gbuffer_multilight.frag");
#endif
    static ShaderProgramDraw gbuff_quad(shader_path,"ss_cubemap_test_gbuffer.vert","","ss_cubemap_test_gbuffer.frag");
    static ShaderProgramDraw gbuff_wrap(shader_path,"ss_cubemap_test_wrap_gbuffer.vert","","ss_cubemap_test_wrap_gbuffer.frag");

    static ShaderProgramDraw render_to_cubemap_test(shader_path,"ss_cubemap_render_to_cubemap.vert","","ss_cubemap_render_to_cubemap.frag");
    static ShaderProgramDraw render_to_cubemap_test_screen(shader_path,"ss_cubemap_test_render_to_cubemap_screen.vert","","ss_cubemap_test_render_to_cubemap_screen.frag");
    static ShaderProgramDraw render_to_cubemap_test_cube(shader_path,"ss_cubemap_test_render_to_cubemap_cube.vert","","ss_cubemap_test_render_to_cubemap_cube.frag");
    static ShaderProgramDraw render_combination(shader_path,"ss_array_combination.vert","","ss_array_combination.frag");

    static ShaderProgramDraw render_mipmaps(shader_path,"display_tex.vert","","display_tex.frag");
    static ThreeDPlane * screen_quad = new ThreeDPlane();
    static Mesh::Material * test_mat = new Mesh::Material();
    static ShaderProgramDraw test2(shader_path,"display_tex_2.vert","","display_tex_2.frag");

    const int GBUFFER_SIZE = 1024;
    const float LIGHT_CAMERA_SIZE = 3.0f;

#ifdef SINGLE_LIGHT
    static VertexNormalBuffer light_buffer(GBUFFER_SIZE);
#else
    static ArrayVertexNormalBuffer light_buffer(GBUFFER_SIZE, 4);
#endif

    const int ARRAY_TEXTURE_SIZE = 1024;
    const int LAYERS = 10;

    const int MIPMAPS = 3;
    const int SCALING [MIPMAPS] = {2, 4, 8};
    static ArrayTextureBuffer arraytexmap(ARRAY_TEXTURE_SIZE,LAYERS,1);
    static MipMapGenerator mipmaps [MIPMAPS] = {MipMapGenerator(ARRAY_TEXTURE_SIZE/SCALING[0], LAYERS, 1), MipMapGenerator(ARRAY_TEXTURE_SIZE/SCALING[1], LAYERS, 1), MipMapGenerator(ARRAY_TEXTURE_SIZE/SCALING[2], LAYERS, 1)};

    arraytexmap.disableMipMaps();

    const float CAMERA_DISTANCE = 6.0f; //This should not matter (can be DIST = max bounding box + camera near + epsilon
    const float CAMERA_NEAR = 1.0f;
    const float CAMERA_FAR = 21.0f;
    const float CAMERA_SIZE = 6.0f;

    int discPoints = params->samples / manager.size();
    const int DISCS = LAYERS;

    //TODO more objs
    ThreeDObject * obj = objects[0];
    for(int i = 0; i < objects.size(); i++)
    {
        ThreeDObject * o = objects[i];
        if(o->enabled)
        {
            obj = o;
            break;
        }
    }
    Mesh::Material * scattering_material = obj->mesh.getMaterial();

    check_gl_error();
    if(reload)
    {
        obj_shader.reload();
        gbuff_shader.reload();
        gbuff_wrap.reload();
        render_to_array.reload();
        render_to_cubemap_test_screen.reload();
        render_to_cubemap_test_cube.reload();
        render_combination.reload();
        test2.reload();
        render_mipmaps.reload();

    }

    static bool initialized = false;

    if(!initialized)
    {
        screen_quad->init("","plane",*test_mat);
        initialized = true;
        vector<Vec3f> * discpoint_data = new vector<Vec3f>();
        getDiscPoints(discpoint_data,discPoints,DISCS);

        Mesh::Texture * tex = new Mesh::Texture("discpoints",GL_TEXTURE_2D, discPoints, DISCS, *discpoint_data);
        tex->init();
        scattering_material->addTexture(tex);
    }

    if(params->currentFlags & TranslucentParameters::SAMPLES_CHANGED)
    {
        vector<Vec3f> * discpoint_data = new vector<Vec3f>();
        getDiscPoints(discpoint_data,discPoints,DISCS);
        Mesh::Texture * tex = scattering_material->getTexture(string("discpoints"));
        tex->reloadData(*discpoint_data,discPoints,DISCS);
        params->currentFlags &= ~(TranslucentParameters::SAMPLES_CHANGED);
    }

    gbuff_shader.use();


    // Set up a modelview matrix suitable for shadow: Maps from world coords to
    // shadow buffer coords.
#ifdef SINGLE_LIGHT
        Vec3f v = Vec3f(manager[0].position);
        gbuff_shader.set_view_matrix(lookat_Mat4x4f(v,-v,Vec3f(0,1,0))); //PARALLEL!
        Mat4x4f mat = translation_Mat4x4f(Vec3f(0.5));
        mat *= scaling_Mat4x4f(Vec3f(0.5));
        mat *= gbuff_shader.get_projection_matrix();
        mat *= gbuff_shader.get_view_matrix();
        scattering_material->addUniform("lightMatrix",mat);
#else
        Mat4x4f projection_light = ortho_Mat4x4f(Vec3f(-LIGHT_CAMERA_SIZE,-LIGHT_CAMERA_SIZE,1),Vec3f(LIGHT_CAMERA_SIZE,LIGHT_CAMERA_SIZE,10));

        vector<Mat4x4f> lightMatrices;
        vector<Mat4x4f> inverseLightMatrices;
        for(int i = 0; i < manager.size(); i++)
        {
            Vec3f light_dir = Vec3f(manager[i].position);
            Mat4x4f V = lookat_Mat4x4f(light_dir,-light_dir,Vec3f(0,1,0));
            lightMatrices.push_back(V); //PARALLEL!

            Mat4x4f mat = translation_Mat4x4f(Vec3f(0.5));
            mat *= scaling_Mat4x4f(Vec3f(0.5));
            mat *= projection_light;
            mat *= V;
            inverseLightMatrices.push_back(mat);

        }
        gbuff_shader.set_uniform("lightMatrices",lightMatrices,manager.size());
        gbuff_shader.set_uniform("layers", manager.size());
#endif

    // Switch viewport size to that of shadow buffer.

    glViewport(0, 0, GBUFFER_SIZE, GBUFFER_SIZE);

    // Draw to shadow buffer.
    light_buffer.enable();
    obj->display(gbuff_shader);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glDrawBuffer(GL_BACK);

    // We need to reset the viewport, since the shadow buffer does not have
    // the same size as the screen window.
    glViewport(0, 0, window_width, window_height);

    Mat4x4f mat = translation_Mat4x4f(Vec3f(0.5));
    mat *= scaling_Mat4x4f(Vec3f(0.5));
    mat *= gbuff_shader.get_projection_matrix();
    mat *= gbuff_shader.get_view_matrix();

    Mesh::Texture * vtex = light_buffer.getVertexTexture();
    Mesh::Texture * ntex = light_buffer.getNormalTexture();

    scattering_material->addTexture(vtex);
    scattering_material->addTexture(ntex);
    scattering_material->addUniform("lightMatrix",mat);
    check_gl_error();

    render_to_array.use();

    Vec3f center = obj->getCenter();

    static Vec3f cameraPositions[LAYERS] = {
        center + Vec3f(1,0,0) * CAMERA_DISTANCE, //+X
        center - Vec3f(1,0,0) * CAMERA_DISTANCE, //-X
        center + Vec3f(0,1,0) * CAMERA_DISTANCE, //+Y
        center - Vec3f(0,1,0) * CAMERA_DISTANCE, //-Y
        center + Vec3f(0,0,1) * CAMERA_DISTANCE, //+Z
        center - Vec3f(0,0,1) * CAMERA_DISTANCE,  //-Z
        center + Vec3f(1.0f/sqrt(2.0f),1.0f/sqrt(2.0f),0) * CAMERA_DISTANCE,
        center + Vec3f(-1.0f/sqrt(2.0f),-1.0f/sqrt(2.0f),0) * CAMERA_DISTANCE,
        center + Vec3f(-1.0f/sqrt(2.0f),1.0f/sqrt(2.0f),0) * CAMERA_DISTANCE,
        center + Vec3f(1.0f/sqrt(2.0f),-1.0f/sqrt(2.0f),0) * CAMERA_DISTANCE
    };

    static Mat4x4f viewMatrices[LAYERS]  = {
        scaling_Mat4x4f(Vec3f(1,-1,1)) * lookat_Mat4x4f_target(cameraPositions[0], center, Vec3f(0,1,0)), //+X
        scaling_Mat4x4f(Vec3f(1,-1,1)) * lookat_Mat4x4f_target(cameraPositions[1], center, Vec3f(0,1,0)), //-X
        scaling_Mat4x4f(Vec3f(-1,1,1)) * lookat_Mat4x4f_target(cameraPositions[2], center, Vec3f(0,0,1)), //+Y
        scaling_Mat4x4f(Vec3f(-1,1,1)) * lookat_Mat4x4f_target(cameraPositions[3], center, Vec3f(0,0,-1)), //-Y
        scaling_Mat4x4f(Vec3f(1,-1,1)) * lookat_Mat4x4f_target(cameraPositions[4], center, Vec3f(0,1,0)), //+Z
        scaling_Mat4x4f(Vec3f(1,-1,1)) * lookat_Mat4x4f_target(cameraPositions[5], center, Vec3f(0,1,0)),  //-Z
        scaling_Mat4x4f(Vec3f(1,-1,1)) * lookat_Mat4x4f_target(cameraPositions[6], center, Vec3f(0,1,0)),
        scaling_Mat4x4f(Vec3f(1,-1,1)) * lookat_Mat4x4f_target(cameraPositions[7], center, Vec3f(0,1,0)),
        scaling_Mat4x4f(Vec3f(1,-1,1)) * lookat_Mat4x4f_target(cameraPositions[8], center, Vec3f(0,1,0)),
        scaling_Mat4x4f(Vec3f(1,-1,1)) * lookat_Mat4x4f_target(cameraPositions[9], center, Vec3f(0,1,0))
    };

    vector<Mat4x4f> planeTransformMatrices(LAYERS);
    vector<Mat4x4f> viewMatricesvector(LAYERS);

    Mat4x4f model = identity_Mat4x4f();
    Mat4x4f projection = ortho_Mat4x4f(Vec3f(-CAMERA_SIZE,-CAMERA_SIZE,CAMERA_NEAR),Vec3f(CAMERA_SIZE,CAMERA_SIZE,CAMERA_FAR));

    Mat4x4f mat2 = translation_Mat4x4f(Vec3f(0.5));
    mat2 *= scaling_Mat4x4f(Vec3f(0.5));
    mat2 *= projection;

    for(int i = 0; i < LAYERS; i++)
    {
        planeTransformMatrices[i] = mat2 * viewMatrices[i];
        viewMatricesvector[i] = viewMatrices[i];
    }

    glViewport(0,0,ARRAY_TEXTURE_SIZE,ARRAY_TEXTURE_SIZE);




    Vec3f radius = Vec3f(29.909f,23.316f, 18.906f); //radius for marble - red 29.909 green 23.316 blue 18.906
    //float trueRadius = clamp01(length(mat * Vec4f(radius[0],0,0,0)));

    float trueRadius = params->circleradius;
    //render_to_cubemap.set_uniform("discpoints", discpoints, DISC_POINTS);
    render_to_array.set_uniform("one_over_max_samples",1.0f/params->samples);
    render_to_array.set_uniform("one_over_discs",1.0f/DISCS);
    render_to_array.set_uniform("samples",params->samples);
    render_to_array.set_uniform("discradius",trueRadius);
    render_to_array.set_uniform("epsilon_gbuffer", params->epsilon_gbuffer);
    set_light_and_camera(render_to_array);

#ifndef SINGLE_LIGHT
        render_to_array.set_uniform("lightMatrices",inverseLightMatrices, manager.size());
#endif

    render_to_array.set_uniform("viewMatrices", viewMatricesvector, LAYERS);
    render_to_array.set_uniform("layers", LAYERS);
    render_to_array.set_model_matrix(model);
    render_to_array.set_projection_matrix(projection);
    arraytexmap.enable();
    obj->display(render_to_array);

    //arraytexmap.generateMipMaps();

    //glDisable(GL_DEPTH_TEST);

    render_mipmaps.use();
    render_mipmaps.set_uniform("viewMatrices", viewMatricesvector, LAYERS);
    render_mipmaps.set_uniform("layers", LAYERS);


check_gl_error();
    for(int level = 0; level < MIPMAPS; level++)
    {
        glViewport(0, 0, ARRAY_TEXTURE_SIZE / SCALING[level], ARRAY_TEXTURE_SIZE / SCALING[level]);

        /* image processing of mipmaps */
        screen_quad->mesh.getMaterial()->removeTexture(string("colorMap"));
        GLuint sourceTex = 0;
        if(level == 0)
        {
            sourceTex = arraytexmap.getColorTexture()->get_id();
        }
        else
        {
            sourceTex = mipmaps[level - 1].getColorTexture()->get_id();
        }
        screen_quad->mesh.getMaterial()->addTexture(new Mesh::Texture("colorMap", sourceTex, GL_TEXTURE_2D_ARRAY));

        render_mipmaps.set_uniform("scaling", SCALING[level]);

        render_mipmaps.set_uniform("texStep", 1.0f / ARRAY_TEXTURE_SIZE);

        for(int i = 0; i < LAYERS; i++)
        {
            mipmaps[level].enable(i);
            render_mipmaps.set_uniform("currentLayer", i);
            screen_quad->display(render_mipmaps);
        }
        check_gl_error();
    }
check_gl_error();
    glViewport(0, 0, ARRAY_TEXTURE_SIZE , ARRAY_TEXTURE_SIZE );
    arraytexmap.enableMipMaps();

    /* copying texture in mipmaps */
    for(int level = 0; level < MIPMAPS; level++)
    {
        GLuint target = arraytexmap.getColorTexture()->get_id();
        GLuint source = mipmaps[level].getColorTexture()->get_id();
        GLuint source_d = mipmaps[level].getDepthTexture()->get_id();

        glBindFramebuffer(GL_READ_FRAMEBUFFER, mipmaps[level].getFBO());
        glBindTexture(GL_TEXTURE_2D_ARRAY, target);

        for(int i = 0; i < LAYERS; i++)
        {
            glFramebufferTextureLayer(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, source, 0, i);
            glFramebufferTextureLayer(GL_READ_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, source_d, 0, i);
            glReadBuffer(GL_COLOR_ATTACHMENT0);
            glCopyTexSubImage3D(GL_TEXTURE_2D_ARRAY, level + 1, 0, 0, i, 0, 0, ARRAY_TEXTURE_SIZE / SCALING[level], ARRAY_TEXTURE_SIZE / SCALING[level]);
        }

        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

    }
    //glUniform1i(test.get_uniform_location("colorMap"), 0);
    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D_ARRAY, front->getColorTexture()->get_id());

    //draw_screen_aligned_quad(test);
    //glEnable(GL_DEPTH_TEST);
    check_gl_error();

    test_mat->removeTexture(string("colorMap"));
    test_mat->addTexture(arraytexmap.getColorTexture());
    test_mat->addTexture(arraytexmap.getDepthTexture());
    test_mat->addTexture(mipmaps[1].getColorTexture());

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
    glViewport(0,0,window_width,window_height);

    test2.use();
    test2.set_uniform("mipmap_LOD",params->LOD);
    if(params->debugOverlayVisible)
        screen_quad->display(test2);

    Mesh::Texture * color = arraytexmap.getColorTexture();
    Mesh::Texture * depth = arraytexmap.getDepthTexture();
    scattering_material->addTexture(color);
    scattering_material->addTexture(depth);




    render_combination.use();
    render_combination.set_uniform("shadow_bias", params->shadow_bias);
    render_combination.set_uniform("epsilon_combination", params->epsilon_combination);
    render_combination.set_uniform("one_over_max_samples", 1.0f/params->samples);
    render_combination.set_uniform("mipmap_LOD",params->LOD);

    render_combination.set_uniform("cameraMatrices", planeTransformMatrices,LAYERS);
    float worldCircleRadius = params->circleradius * 2 * LIGHT_CAMERA_SIZE;
    render_combination.set_uniform("disc_area", (float)(worldCircleRadius * worldCircleRadius * M_PI));
    render_combination.set_uniform("step_tex", 1.0f/ARRAY_TEXTURE_SIZE);
    set_light_and_camera(render_combination);
    obj->display(render_combination);


    //scattering_material->loadUniforms(test2);



}

void TranslucentMaterials::render_direct_array_time(bool reload, ShaderProgramDraw & render_to_array)
{
    performanceTimer.registerEvent("-1: Start");
    static ShaderProgramDraw obj_shader(shader_path,"object.vert","","object.frag");

    static ShaderProgramDraw gbuff_shader(shader_path,"ss_cubemap_gbuffer_multilight.vert","ss_cubemap_gbuffer_multilight.geom", "ss_cubemap_gbuffer_multilight.frag");

    static ShaderProgramDraw gbuff_quad(shader_path,"ss_cubemap_test_gbuffer.vert","","ss_cubemap_test_gbuffer.frag");
    static ShaderProgramDraw gbuff_wrap(shader_path,"ss_cubemap_test_wrap_gbuffer.vert","","ss_cubemap_test_wrap_gbuffer.frag");

    static ShaderProgramDraw render_to_cubemap_test(shader_path,"ss_cubemap_render_to_cubemap.vert","","ss_cubemap_render_to_cubemap.frag");
    static ShaderProgramDraw render_to_cubemap_test_screen(shader_path,"ss_cubemap_test_render_to_cubemap_screen.vert","","ss_cubemap_test_render_to_cubemap_screen.frag");
    static ShaderProgramDraw render_to_cubemap_test_cube(shader_path,"ss_cubemap_test_render_to_cubemap_cube.vert","","ss_cubemap_test_render_to_cubemap_cube.frag");
    static ShaderProgramDraw render_combination(shader_path,"ss_array_combination.vert","","ss_array_combination.frag");

    static ShaderProgramDraw render_mipmaps(shader_path,"display_tex.vert","display_tex.geom","display_tex.frag");
    static ThreeDPlane * screen_quad = new ThreeDPlane();
    static Mesh::Material * screen_quad_material = new Mesh::Material();
    static ShaderProgramDraw screen_quad_display_shader(shader_path,"display_tex_2.vert","","display_tex_2.frag");

    const int GBUFFER_SIZE = 1024;
    const float LIGHT_CAMERA_SIZE = 4.0f;

    const int ARRAY_TEXTURE_SIZE = 1024;
    const int LAYERS = 10;
    int samples_per_texel = params->samples;
    int maximum_samples = params->maxsamples;

    const int MIPMAPS = 3;

    static ArrayTextureBuffer arraytexmap(ARRAY_TEXTURE_SIZE,LAYERS,MIPMAPS + 1);
    static ArrayTextureBuffer arraytexmap_back(ARRAY_TEXTURE_SIZE,LAYERS,MIPMAPS + 1);
    ArrayTextureBuffer * front;

    static MipMapGeneratorView * mipmaps = new MipMapGeneratorView(arraytexmap.getColorTexture()->get_id(), arraytexmap.getDepthTexture()->get_id(), ARRAY_TEXTURE_SIZE, LAYERS, MIPMAPS);
    static MipMapGeneratorView * mipmaps_back = new MipMapGeneratorView(arraytexmap_back.getColorTexture()->get_id(), arraytexmap_back.getDepthTexture()->get_id(), ARRAY_TEXTURE_SIZE, LAYERS, MIPMAPS);
    MipMapGeneratorView * front_mipmaps;


    static ArrayVertexNormalBuffer light_buffer(GBUFFER_SIZE, 4);


    const float CAMERA_DISTANCE = 3.0f; //This should not matter (can be DIST = max bounding box + camera near + epsilon
    const float CAMERA_NEAR = 0.1f;
    const float CAMERA_FAR = 10.0f;
    const float CAMERA_SIZE = 1.0f;
    const int CONVERGENCE_FRAMES = 100;
    const float CAMERA_RATIO = CAMERA_SIZE / LIGHT_CAMERA_SIZE;

    int discPoints = maximum_samples; //TODO more LIGHTS!
    const int DISCS = LAYERS;

    //TODO more objs
    ThreeDObject * obj = objects[0];
    for(int i = 0; i < objects.size(); i++)
    {
        ThreeDObject * o = objects[i];
        if(o->enabled)
        {
            obj = o;
            break;
        }
    }
    Mesh::ScatteringMaterial * scattering_material = (Mesh::ScatteringMaterial*)obj->mesh.getMaterial();
    Vec3f tr = scattering_material->transmissionCoefficient;
    float minimumTransmission = min(tr[0],min(tr[1],tr[2]));

    if(reload)
    {
        //test.reload();
        screen_quad_display_shader.reload();
        obj_shader.reload();
        gbuff_shader.reload();
        gbuff_quad.reload();
        gbuff_wrap.reload();
        render_to_array.reload();
        render_to_cubemap_test_screen.reload();
        render_to_cubemap_test_cube.reload();
        render_combination.reload();
        render_mipmaps.reload();
        currentFrame = 0;
    }

//    Vec3f center = obj->getCenter();

    static vector<Vec3f> cameraPositions(LAYERS);

            /*= {
        center + Vec3f(1,0,0) * CAMERA_DISTANCE, //+X
        center - Vec3f(1,0,0) * CAMERA_DISTANCE, //-X
        center + Vec3f(0,1,0) * CAMERA_DISTANCE, //+Y
        center - Vec3f(0,1,0) * CAMERA_DISTANCE, //-Y
        center + Vec3f(0,0,1) * CAMERA_DISTANCE, //+Z
        center - Vec3f(0,0,1) * CAMERA_DISTANCE,  //-Z
        center + Vec3f(1.0f/sqrt(2.0f),1.0f/sqrt(2.0f),1.0f/sqrt(2.0f)) * CAMERA_DISTANCE,
        center + Vec3f(-1.0f/sqrt(2.0f),-1.0f/sqrt(2.0f),-1.0f/sqrt(2.0f)) * CAMERA_DISTANCE,
        center + Vec3f(-1.0f/sqrt(2.0f),1.0f/sqrt(2.0f),0) * CAMERA_DISTANCE,
        center + Vec3f(1.0f/sqrt(2.0f),-1.0f/sqrt(2.0f),0) * CAMERA_DISTANCE
    };
*/
    static vector<Mat4x4f> viewMatrices(LAYERS);
/*            = {
        scaling_Mat4x4f(Vec3f(1,-1,1)) * lookat_Mat4x4f_target(cameraPositions[0], center, Vec3f(0,1,0)), //+X
        scaling_Mat4x4f(Vec3f(1,-1,1)) * lookat_Mat4x4f_target(cameraPositions[1], center, Vec3f(0,1,0)), //-X
        scaling_Mat4x4f(Vec3f(-1,1,1)) * lookat_Mat4x4f_target(cameraPositions[2], center, Vec3f(0,0,1)), //+Y
        scaling_Mat4x4f(Vec3f(-1,1,1)) * lookat_Mat4x4f_target(cameraPositions[3], center, Vec3f(0,0,-1)), //-Y
        scaling_Mat4x4f(Vec3f(1,-1,1)) * lookat_Mat4x4f_target(cameraPositions[4], center, Vec3f(0,1,0)), //+Z
        scaling_Mat4x4f(Vec3f(1,-1,1)) * lookat_Mat4x4f_target(cameraPositions[5], center, Vec3f(0,1,0)),  //-Z
        scaling_Mat4x4f(Vec3f(1,-1,1)) * lookat_Mat4x4f_target(cameraPositions[6], center, Vec3f(0,1,0)),
        scaling_Mat4x4f(Vec3f(1,-1,1)) * lookat_Mat4x4f_target(cameraPositions[7], center, Vec3f(0,1,0)),
        scaling_Mat4x4f(Vec3f(1,-1,1)) * lookat_Mat4x4f_target(cameraPositions[8], center, Vec3f(0,1,0)),
        scaling_Mat4x4f(Vec3f(1,-1,1)) * lookat_Mat4x4f_target(cameraPositions[9], center, Vec3f(0,1,0))
    };
*/
    static vector<Mat4x4f> planeTransformMatrices(LAYERS);

    static Mat4x4f model_identity = identity_Mat4x4f();
    static Mat4x4f projection_array = ortho_Mat4x4f(Vec3f(-CAMERA_SIZE,-CAMERA_SIZE,CAMERA_NEAR),Vec3f(CAMERA_SIZE,CAMERA_SIZE,CAMERA_FAR));
    static Mat4x4f projection_light = ortho_Mat4x4f(Vec3f(-LIGHT_CAMERA_SIZE,-LIGHT_CAMERA_SIZE,1),Vec3f(LIGHT_CAMERA_SIZE,LIGHT_CAMERA_SIZE,10));

    static bool initialized = false;
    if(!initialized)
    {
        initialized = true;
        screen_quad->init("","plane",*screen_quad_material);
        vector<Vec3f> * discpoint_data = new vector<Vec3f>();

        getDiscPoints(discpoint_data,discPoints,DISCS, minimumTransmission);

        Mesh::Texture * tex = new Mesh::Texture("discpoints",GL_TEXTURE_2D, discPoints, DISCS, *discpoint_data);
        tex->init();
        scattering_material->addTexture(tex);

        // preparing first color buffer from which to read (just for avoiding errors,
        // the shader already avoids that)
        scattering_material->addTexture(arraytexmap_back.getColorTexture());
        scattering_material->addTexture(skybox);

        screen_quad_material->addTexture(tex);
        vector<Vec3f> spherePoints;
        sphereHalton(spherePoints, LAYERS);

        for(int i = 0; i < LAYERS; i++) cout << spherePoints[i] <<endl;

        Mat4x4f mat2 = translation_Mat4x4f(Vec3f(0.5));
        mat2 *= scaling_Mat4x4f(Vec3f(0.5));
        mat2 *= projection_array;

        Vec3f up = Vec3f(0,0,1);
        Vec3f center = obj->getCenter();
        for(int i = 0; i < LAYERS; i++)
        {
            Vec3f point = spherePoints[i];
            Vec3f camera_pos = center + point * CAMERA_DISTANCE;
            cameraPositions[i] = camera_pos;
            viewMatrices[i] = scaling_Mat4x4f(Vec3f(1,-1,1)) * lookat_Mat4x4f_target(camera_pos, center, up);
            planeTransformMatrices[i] = mat2 * viewMatrices[i];
        }
    }

    if(params->currentFlags & TranslucentParameters::SAMPLES_CHANGED)
    {
        vector<Vec3f> * discpoint_data = new vector<Vec3f>();
        getDiscPoints(discpoint_data,discPoints,DISCS, minimumTransmission);
        Mesh::Texture * tex = scattering_material->getTexture(string("discpoints"));
        tex->reloadData(*discpoint_data,discPoints,DISCS);
        params->currentFlags &= ~(TranslucentParameters::SAMPLES_CHANGED);
    }

    performanceTimer.registerEvent("0: Initialization");

    if(currentFrame < CONVERGENCE_FRAMES)
    {
        gbuff_shader.use();

        // Set up a modelview matrix suitable for shadow: Maps from world coords to
        // shadow buffer coords.

        gbuff_shader.set_model_matrix(model_identity);
        gbuff_shader.set_projection_matrix(projection_light);

        vector<Mat4x4f> lightMatrices;
        vector<Mat4x4f> inverseLightMatrices;

        for(int i = 0; i < manager.size(); i++)
        {
            Vec3f light_dir = Vec3f(manager[i].position);
            Mat4x4f V = lookat_Mat4x4f(light_dir,-light_dir,Vec3f(0,0,1));
            lightMatrices.push_back(V); //PARALLEL!

            Mat4x4f mat = translation_Mat4x4f(Vec3f(0.5));
            mat *= scaling_Mat4x4f(Vec3f(0.5));
            mat *= projection_light;
            mat *= V;
            inverseLightMatrices.push_back(mat);
        }

        gbuff_shader.set_uniform("lightMatrices",lightMatrices,manager.size());
        gbuff_shader.set_uniform("layers", manager.size());

        // Switch viewport size to that of shadow buffer.

        glViewport(0, 0, GBUFFER_SIZE, GBUFFER_SIZE);

        // Draw to shadow buffer.
        light_buffer.enable();
        obj->display(gbuff_shader);

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glDrawBuffer(GL_BACK);

        performanceTimer.registerEvent("1: Render to lightmap");

        // We need to reset the viewport, since the shadow buffer does not have
        // the same size as the screen window.
        glViewport(0, 0, window_width, window_height);



        Mesh::Texture * vtex = light_buffer.getVertexTexture();
        Mesh::Texture * ntex = light_buffer.getNormalTexture();

        scattering_material->addTexture(vtex);
        scattering_material->addTexture(ntex);
        screen_quad_material->addTexture(ntex);

        render_to_array.use();


        glViewport(0,0,ARRAY_TEXTURE_SIZE,ARRAY_TEXTURE_SIZE);


        Vec3f radius = Vec3f(29.909f,23.316f, 18.906f); //radius for marble - red 29.909 green 23.316 blue 18.906
        //float trueRadius = clamp01(length(mat * Vec4f(radius[0],0,0,0)));

        float trueRadius = params->circleradius;
        glClearColor(0,0,0,0);

        //render_to_cubemap.set_uniform("discpoints", discpoints, DISC_POINTS);
        render_to_array.set_uniform("one_over_max_samples",1.0f/maximum_samples);
        render_to_array.set_uniform("max_samples",(float)maximum_samples);
        render_to_array.set_uniform("one_over_discs",1.0f/DISCS);
        render_to_array.set_uniform("samples",samples_per_texel);
        render_to_array.set_uniform("discradius",trueRadius);
        render_to_array.set_uniform("epsilon_gbuffer", params->epsilon_gbuffer);
        render_to_array.set_uniform("min_tr", minimumTransmission);
        render_to_array.set_uniform("lightMatrices",inverseLightMatrices, manager.size());
        render_to_array.set_uniform("epsilon_combination", params->epsilon_combination);
        render_to_array.set_uniform("cameraMatrices", planeTransformMatrices,LAYERS);
        render_to_array.set_uniform("current_frame_percentage", currentFrame / float(CONVERGENCE_FRAMES));
        render_to_array.set_uniform("current_frame", currentFrame);
        render_to_array.set_uniform("convergence_frames", CONVERGENCE_FRAMES);
        render_to_array.set_uniform("global_frame",frame);

        set_light_and_camera(render_to_array);

        bool isFrontArrayMap = ((frame % 2) == 0)? true : false;


        // ping-pong between buffers

        if(isFrontArrayMap)
        {
            front = &arraytexmap;
            front_mipmaps = mipmaps;
        }
        else
        {
            front = &arraytexmap_back;
            front_mipmaps = mipmaps_back;
        }

        //cout << render_to_array.processed_shader.c_str() << endl;
        //Render to front from back

        render_to_array.set_uniform("viewMatrices", viewMatrices, LAYERS);
        render_to_array.set_uniform("layers", LAYERS);
        render_to_array.set_model_matrix(model_identity);
        render_to_array.set_projection_matrix(projection_array);
        front->enable();
        performanceTimer.registerEvent("2.1: Render to array prepare");

        obj->display(render_to_array);

        performanceTimer.registerEvent("2: Render to array");

        // Need to disable mipmaps, otherwise I cannot copy from level 0 to another (weird opengl stuff)
        front->disableMipMaps();

        render_mipmaps.use();
        render_mipmaps.set_uniform("viewMatrices", viewMatrices, LAYERS);
        render_mipmaps.set_uniform("layers", LAYERS);
        //front->generateMipMaps();

        //disabling depth test so we do not need a special renderbuffer for depth.
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);

        for(int level = 0; level < MIPMAPS; level++)
        {
            int viewport_size = ARRAY_TEXTURE_SIZE >> (level + 1);
            glViewport(0, 0, viewport_size, viewport_size);

            /* image processing of mipmaps */
            screen_quad->mesh.getMaterial()->removeTexture(string("colorMap"));
            GLuint sourceTex = 0;
            if(level == 0)
            {
                sourceTex = front->getColorTexture()->get_id();
            }
            else
            {
                sourceTex = front_mipmaps->getColorTexture(level - 1)->get_id();
            }

            screen_quad->mesh.getMaterial()->addTexture(new Mesh::Texture("colorMap", sourceTex, GL_TEXTURE_2D_ARRAY));

            render_mipmaps.set_uniform("scaling", 2 << (level + 1));
            render_mipmaps.set_uniform("texStep", 1.0f / ARRAY_TEXTURE_SIZE);

            front_mipmaps->enableUniqueTarget(level);
            screen_quad->display(render_mipmaps);

            performanceTimer.registerEvent(QString("3.0.%1: Compute mipmaps - shader").arg(QString::number(level)).toStdString());
        }

        // re-enabling depth test for final rendering
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);

        front->enableMipMaps();
        performanceTimer.registerEvent("3: Compute mipmaps");

        screen_quad_material->removeTexture(string("colorMap"));
        screen_quad_material->addTexture(front->getColorTexture());
        screen_quad_material->addTexture(front->getDepthTexture());

        // Adding the new calculated stuff.
        Mesh::Texture * color = front->getColorTexture();
        string colname = color->get_name();
        scattering_material->removeTexture(colname); //switching the old color TODO : replace
        scattering_material->addTexture(color);

        Mesh::Texture * depth = front->getDepthTexture();
        scattering_material->addTexture(depth);

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
        glViewport(0,0,window_width,window_height);

        glBindTexture(GL_TEXTURE_2D_ARRAY, front->getColorTexture()->get_id());
        float converg = currentFrame / float(CONVERGENCE_FRAMES);
        //glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_LOD_BIAS, 3.0f * (1 - converg * converg * converg));
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

    }


    render_combination.use();
    render_combination.set_uniform("shadow_bias", params->shadow_bias);
    render_combination.set_uniform("epsilon_combination", params->epsilon_combination);
    render_combination.set_uniform("one_over_max_samples", 1.0f/samples_per_texel);
    render_combination.set_uniform("mipmap_LOD",params->LOD);
    render_combination.set_uniform("current_frame_rev", 1.0f/min(currentFrame + 1,CONVERGENCE_FRAMES));
    render_combination.set_uniform("cameraMatrices", planeTransformMatrices,LAYERS);
    render_combination.set_uniform("camera_dirs", cameraPositions, LAYERS);
    render_combination.set_uniform("has_environment", params->environment);
    float worldCircleRadius = params->circleradius * LIGHT_CAMERA_SIZE;
    render_combination.set_uniform("disc_area", (float)(worldCircleRadius * worldCircleRadius * M_PI) * CAMERA_RATIO);
    render_combination.set_uniform("step_tex", 1.0f/ARRAY_TEXTURE_SIZE);
    render_combination.set_uniform("skybox_dim", Vec2f(skybox->width,skybox->height));

    set_light_and_camera(render_combination);
    obj->display(render_combination);

    if(currentFrame < CONVERGENCE_FRAMES)
    {
        glBindTexture(GL_TEXTURE_2D_ARRAY, front->getColorTexture()->get_id());
        glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_LOD_BIAS, 0.0f);
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    }


    if((currentFrame - 1) % 5 == 0 && currentFrame < 0)
    {
        QImage * screen = takeScreenshot();
        QString name = QString("C:/Users/alessandro/Desktop/test/test_jensen_100_%1.png").arg(currentFrame - 1);
        screen->save(name);
        delete screen;
    }

    currentFrame++;
    performanceTimer.registerEvent("4: Combination");


//#define MEMORY_INFO
#ifdef MEMORY_INFO
    print_memory_info_nvidia();
#endif

    screen_quad_display_shader.use();
    screen_quad_display_shader.set_uniform("mipmap_LOD",params->LOD);
    if(params->debugOverlayVisible)
        screen_quad->display(screen_quad_display_shader);


}

void TranslucentMaterials::render_directional_dipole(bool reload)
{


    static ShaderProgramDraw directional_dipole_shader(shader_path, "directional_dipole_gpu.vert", "", "directional_dipole_gpu.frag");
    static ShaderProgramDraw directional_dipole_shader_vertex(shader_path, "directional_dipole_gpu_vertex.vert", "", "directional_dipole_gpu_vertex.frag");

    if(reload)
    {

        directional_dipole_shader.reload();
        directional_dipole_shader_vertex.reload();
    }


    if(isVertexMode)
    {
        directional_dipole_shader_vertex.use();
        set_light_and_camera(directional_dipole_shader_vertex);

        draw_objects(directional_dipole_shader_vertex);

    }
    else
    {
        directional_dipole_shader.use();
        set_light_and_camera(directional_dipole_shader);

        draw_objects(directional_dipole_shader);

    }

    /*
    // Initialize resources. Note that we have a G-Buffer here. This buffer captures eye
    // space position, normal, and color for each pixel. We then do deferred shading based
    // on that.
    static GBuffer gbuffer(window_width, window_height);

    static ShaderProgramDraw deferred_shading(shader_path, "deferred.vert", "", "deferred_toon.frag");

    gbuffer.rebuild_on_resize(window_width, window_height);
    if(reload)
    {
        deferred_shading.reload();
    }


    render_to_gbuffer(gbuffer, reload);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glDrawBuffer(GL_BACK);

    glDisable(GL_DEPTH_TEST);

    glClear(GL_COLOR_BUFFER_BIT);
    deferred_shading.use();
    set_light_and_camera(deferred_shading);

    gbuffer.bind_textures(0, 1, 2);
    deferred_shading.set_uniform("gtex", 0);
    deferred_shading.set_uniform("ntex", 1);
    deferred_shading.set_uniform("ctex", 2);
    draw_screen_aligned_quad(deferred_shading);
    glEnable(GL_DEPTH_TEST);
    */
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

void initRectangle(GLuint * tex, GLenum * type, int * width, int * height)
{
    GLuint cubetex;
    check_gl_error();
    glGenTextures(1, &cubetex);
    glBindTexture(GL_TEXTURE_RECTANGLE, cubetex);
    glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    string name = string("doge2.png");

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
    *width = res.width();
    *height = res.height();
}

void TranslucentMaterials::paintGL()
{

    static GLGraphics::ShaderProgramDraw skybox_shader(shader_path, "ss_skybox.vert", "", "ss_skybox.frag");
    static bool mark = false;
    if(reload_shaders)
        skybox_shader.reload();

    if(!mark)
    {
        GLuint cubetex;
        GLenum type;
        int width, height;
        initRectangle(&cubetex,&type,&width,&height);
        //initCubemap(&cubetex,&type);
        skybox = new Mesh::Texture("skybox", cubetex, type);
        skybox->width = width;
        skybox->height = height;

        Mesh::Material * e = new Mesh::Material();
        e->addTexture(skybox);
        e->addUniform("cubemap_size", Vec2f(width,height));
        skybox_cube->init(" ", "light_sphere", *e);
        skybox_cube->setScale(Vec3f(20.f));
        skybox_cube->setTranslation(Vec3f(0.0f,0.0f,0.0f));
        skybox_cube->enabled = true;
        mark = true;
    }



    //manager[0].position = Vec4f(6.0f * sin(float(frame) / 100.0f * 2 * M_PI), 0.0f, 6.0f * cos(float(frame) / 100.0f * 2 * M_PI), 1.0f);
    //manager.reloadLights();
    performanceTimer.start();


    //setup_shadow(reload_shaders);

    glClearColor(clearColor[0],clearColor[1],clearColor[2],clearColor[3]);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);



    if(isGridVisible) draw_grid(reload_shaders);
    if(areAxesVisible) draw_axes(reload_shaders);

    //draw_bounding_boxes(reload_shaders);

    static ShaderProgramDraw render_to_cubemap_jensen(shader_path,"ss_cubemap_render_to_cubemap_jensen.vert","ss_cubemap_render_to_cubemap_array.geom","ss_cubemap_render_to_arraymap_multilight_jensen.frag");
    static ShaderProgramDraw render_to_cubemap_jeppe(shader_path,"ss_cubemap_render_to_cubemap_jensen.vert","ss_cubemap_render_to_cubemap_array.geom","ss_cubemap_render_to_arraymap_multilight_jeppe.frag");

    if(render_method == RenderMethod::BRUTE_FORCE)
    {
        switch(render_mode)
        {
        case DRAW_JENSEN:
            render_jensen(reload_shaders);
            break;
        case DRAW_BETTER:
            render_better_dipole(reload_shaders);
            break;
        case DRAW_DIRECTIONAL:
            render_directional_dipole(reload_shaders);
            break;
        }
    }
    else
    {
        switch(render_mode)
        {
        case DRAW_JENSEN:
            render_direct_array_time(reload_shaders,render_to_cubemap_jensen);
            break;
        case DRAW_BETTER:
            //render_direct_test(reload_shaders, render_to_cubemap_jensen);
            break;
        case DRAW_DIRECTIONAL:
            render_direct_array_time(reload_shaders, render_to_cubemap_jeppe);
            break;
        }

    }

    reload_shaders = false;

    if(params->environment)
    {
        skybox_shader.use();
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
    manager[0].intensity = intensity;
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

void TranslucentMaterials::addObject(ThreeDObject *obj)
{
    objects.push_back(obj);
}

void TranslucentMaterials::initializeGL()
{
    setup_gl();
    glClearColor( 0.7f, 0.7f, 0.7f, 0.0f );
    glEnable(GL_DEPTH_TEST);
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
    Mesh::ScatteringMaterial * scattering_mat = getDefaultMaterial(S_Potato);
    ThreeDObject * bunny1 = new ThreeDObject();
    bunny1->init(objects_path+"buddha.obj", "bunny1", *scattering_mat);
    bunny1->setScale(Vec3f(1.f));
    bunny1->setRotation(Vec3f(90,0,0));
    bunny1->setTranslation(Vec3f(0,0,0.f));
    bunny1->enabled = true;
    bunny1->boundingBoxEnabled = true;
    return bunny1;
}

ThreeDObject * TranslucentMaterials::getObject(string name)
{
    vector<ThreeDObject*>::iterator it;
    it = std::find_if(objects.begin(),objects.end(), CompareThreeD(name));
    if(it != objects.end())
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
