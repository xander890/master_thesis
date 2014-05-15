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

#define BUNNIES
#define POINT_DIST 0 // 0 random, 1 exponential, 2 uniform

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
const Vec4f light_diffuse(.5f,.5f,.7f,1.0f);
const Vec4f light_position(-6.f,0.f,0.f,1);
const Vec4f light_diffuse_2(0.7,0.5,0.5,0.0);
const Vec4f light_position_2(6.0,0.0,0.0,1.0);

void draw_screen_aligned_quad(ShaderProgram& shader_prog)
{
    check_gl_error();

    const Vec2f points[] = {Vec2f(-1,-1), Vec2f(1,-1), Vec2f(-1,1), Vec2f(1,1)};
    GLint pos_attrib = shader_prog.get_attrib_location("vertex");


    static GLuint VAO = 0;
    if(VAO == 0)
    {
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        GLuint VBO;
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER,4*sizeof(Vec2f),&points[0],GL_STATIC_DRAW);
        glVertexAttribPointer(pos_attrib,2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(pos_attrib);
    }
    check_gl_error();
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_STRIP,0,4);
}

void TranslucentMaterials::draw_objects(ShaderProgramDraw& shader_prog)
{
    vector<string> objs;
    objs.push_back(default_obj);
    draw_objects(shader_prog,objs);
}

void TranslucentMaterials::draw_objects(ShaderProgramDraw& shader_prog, vector<string>& objectsToDraw)
{

    if(objects.empty())
    {

        int LODSpheres = 20;
        int LODCubes = 10;

        //objects.push_back(ThreeDObject());
        //objects[objects.size()-1].init(objects_path+"cottage_obj/cottage.obj");
        //objects.back().scale(Vec3f(1));
        //objects.back().translate(Vec3f(-14.8718f,-7.91218f,terra.height(-14.8718f,-7.91218f)));

        DipoleCPU dipoleCalculator;
        dipoleCalculator.light = manager[0];
        dipoleCalculator.user = user;

        Mesh::ScatteringMaterial * scattering_mat = getDefaultMaterial(S_Marble);
        Mesh::ScatteringMaterial * scattering_mat2 = getDefaultMaterial(S_Marble);
        Mesh::ScatteringMaterial * scattering_mat3 = getDefaultMaterial(S_Marble);

//        scattering_mat->diffuse = Vec4f(0.83f, 0.79f, 0.75f,1.0f);
        scattering_mat->name = "marble";


        Mesh::Material * default_mat = new Mesh::Material();

//      SingleScattering * s = new SingleScattering(*scattering_mat);

        JensenDipole jensenDipoleModel(*scattering_mat);
        BetterDipole deonDipoleModel(*scattering_mat);
        DirectionalDipole jeppeDipole(*scattering_mat);

        ThreeDObject * t = new ThreeDObject();
        objects.push_back(t);
        t->init(objects_path+"cow.obj", "cow", *scattering_mat);
        t->setScale(Vec3f(.3f));
        t->setRotation(Vec3f(1,0,0), 90);
        t->setTranslation(Vec3f(0,0,-1.5f));

        const int TEXTURE_SIZE = 512;
        vector<Vec3f> texarray(TEXTURE_SIZE*TEXTURE_SIZE);

        ThreeDPlane *plane = new ThreeDPlane();
        objects.push_back(plane);
        plane->init(" ", "plane", *default_mat);
        //plane->setTexture(texarray,TEXTURE_SIZE);
        plane->setScale(Vec3f(20.0f));
        plane->setTranslation(Vec3f(0.0f,0.0f,-6.0f));

        ThreeDSphere *sphere = new ThreeDSphere(20);
        objects.push_back(sphere);
        sphere->init(" ", "sphere", *scattering_mat3);
        sphere->setScale(Vec3f(1.0f));
//        sphere->translate();

        ThreeDCube *cube_light = new ThreeDCube(10);
        objects.push_back(cube_light);
        cube_light->init(" ", "light_sphere", *default_mat);
        cube_light->setScale(Vec3f(.3f));
        cube_light->setTranslation(Vec3f(manager[0].position));


        ThreeDSphere *translucent_sphere = new ThreeDSphere(20);
        objects.push_back(translucent_sphere);
        translucent_sphere->init(" ","translucent", *default_mat);
        translucent_sphere->setScale(Vec3f(1.5f));
        translucent_sphere->setTranslation(Vec3f(0.0f,10.0f,-1.5f));

        vector<Vec3f> luminance;




        ThreeDCube *cube = new ThreeDCube(LODCubes);
        objects.push_back(cube);
        cube->init(" ","cube",*scattering_mat2);
        cube->setScale(Vec3f(2.0f));
        cube->setTranslation(Vec3f(0.0f,0.0f,-2.f));

        ThreeDCube *cubemap = new ThreeDCube(10);
        objects.push_back(cubemap);
        cubemap->init(" ","cubem",*default_mat);
        cubemap->setScale(Vec3f(4.0f));

        ThreeDCube *cube2 = new ThreeDCube(LODCubes);
        objects.push_back(cube2);
        cube2->init(" ","cube2",*scattering_mat);
        cube2->setScale(Vec3f(2.0f));
        cube2->setTranslation(Vec3f(7.5f,0.0f,-2.f));


        ThreeDCube *cube3 = new ThreeDCube(LODCubes);
        objects.push_back(cube3);
        cube3->init(" ","cube3",*scattering_mat);
        cube3->setScale(Vec3f(2.0f));
        cube3->setTranslation(Vec3f(15.0f,0.0f,-2.f));

        ThreeDCube *cube4 = new ThreeDCube(LODCubes);
        objects.push_back(cube4);
        cube4->init(" ","cube4",*scattering_mat);
        cube4->setScale(Vec3f(2.0f));
        cube4->setTranslation(Vec3f(0.f,7.5f,-2.f));

        ThreeDCube *cube5 = new ThreeDCube(LODCubes);
        objects.push_back(cube5);
        cube5->init(" ","cube5",*scattering_mat);
        cube5->setScale(Vec3f(2.0f));
        cube5->setTranslation(Vec3f(7.5f,7.5f,-2.f));

        ThreeDCube *cube6 = new ThreeDCube(LODCubes);
        objects.push_back(cube6);
        cube6->init(" ","cube6",*scattering_mat);
        cube6->setScale(Vec3f(2.0f));
        cube6->setTranslation(Vec3f(15.f,7.5f,-2.f));

        ThreeDCube *cube7 = new ThreeDCube(LODCubes);
        objects.push_back(cube7);
        cube7->init(" ","cube7",*scattering_mat);
        cube7->setScale(Vec3f(2.0f));
        cube7->setTranslation(Vec3f(0.f,15.0f,-2.f));

        ThreeDCube *cube8 = new ThreeDCube(LODCubes);
        objects.push_back(cube8);
        cube8->init(" ","cube8",*scattering_mat);
        cube8->setScale(Vec3f(2.0f));
        cube8->setTranslation(Vec3f(7.5f,15.0f,-2.f));

        ThreeDCube *cube9 = new ThreeDCube(LODCubes);
        objects.push_back(cube9);
        cube9->init(" ","cube9",*scattering_mat);
        cube9->setScale(Vec3f(2.0f));
        cube9->setTranslation(Vec3f(15.f,15.0f,-2.f));


        ThreeDSphere *sphere1 = new ThreeDSphere(LODSpheres);
        objects.push_back(sphere1);
        sphere1->init(" ","sphere1",*scattering_mat);
        sphere1->setScale(Vec3f(2.0f));
        sphere1->setTranslation(Vec3f(0.0f,0.0f,-2.f));


        ThreeDSphere *sphere2 = new ThreeDSphere(LODSpheres);
        objects.push_back(sphere2);
        sphere2->init(" ","sphere2",*scattering_mat);
        sphere2->setScale(Vec3f(2.0f));
        sphere2->setTranslation(Vec3f(7.5f,0.0f,-2.f));


        ThreeDSphere *sphere3 = new ThreeDSphere(LODSpheres);
        objects.push_back(sphere3);
        sphere3->init(" ","sphere3",*scattering_mat);
        sphere3->setScale(Vec3f(2.0f));
        sphere3->setTranslation(Vec3f(15.0f,0.0f,-2.f));

        ThreeDObject * cow1 = new ThreeDObject();
        objects.push_back(cow1);
        cow1->init(objects_path+"cow.obj", "cow1", *scattering_mat);
        cow1->setScale(Vec3f(.5f));
        cow1->setRotation(Vec3f(1,0,0), 90);
        cow1->setTranslation(Vec3f(0,0,-1.5f));

        ThreeDObject * cow2 = new ThreeDObject();
        objects.push_back(cow2);
        cow2->init(objects_path+"cow.obj", "cow2", *scattering_mat);
        cow2->setScale(Vec3f(.5f));
        cow2->setRotation(Vec3f(1,0,0), 90);
        cow2->setTranslation(Vec3f(7.5,0,-1.5f));

        ThreeDObject * cow3 = new ThreeDObject();
        objects.push_back(cow3);
        cow3->init(objects_path+"cow.obj", "cow3", *scattering_mat);
        cow3->setScale(Vec3f(.5f));
        cow3->setRotation(Vec3f(1,0,0), 90);
        cow3->setTranslation(Vec3f(15.0,0,-1.5f));

        ThreeDObject * bunny2 = new ThreeDObject();
        objects.push_back(bunny2);
        bunny2->init(objects_path+"bunny-simplified.obj", "bunny2", *scattering_mat);
        bunny2->setScale(Vec3f(30.f));
        bunny2->setRotation(Vec3f(1,0,0), 90);
        bunny2->setTranslation(Vec3f(7.5,0,-4.5f));

        ThreeDObject * bunny3 = new ThreeDObject();
        objects.push_back(bunny3);
        bunny3->init(objects_path+"bunny-simplified.obj", "bunny3", *scattering_mat);
        bunny3->setScale(Vec3f(30.f));
        bunny3->setRotation(Vec3f(1,0,0), 90);
        bunny3->setTranslation(Vec3f(15.0,0,-4.5f));

        check_gl_error();
        //objects.push_back(ThreeDObject());
        //objects[objects.size()-1].init(objects_path+"portal.obj");
        //objects.back().scale(Vec3f(2));
        //objects.back().translate(Vec3f(0,0,terra.height(0,0)+1.0));

#ifdef CUBES
        dipoleCalculator.calculate(*cube, luminance, jensenDipoleModel);
        dipoleCalculator.calculate(*cube4, luminance, deonDipoleModel);
        dipoleCalculator.calculate(*cube7, luminance, jeppeDipole);
        DipoleGPU gip;
        gip.prepare(*cube2);
        gip.prepare(*cube3);
        gip.prepare(*cube5);
        gip.prepare(*cube6);
        gip.prepare(*cube8);
        gip.prepare(*cube9);
#endif

#ifdef BUNNIES
        //dipoleCalculator.calculate(*bunny1,luminance,jensenDipoleModel);
        //dipoleCalculator.calculate(*bunny2,luminance,deonDipoleModel);
        //dipoleCalculator.calculate(*bunny3,luminance,jeppeDipole);
        DipoleGPU gip;
        gip.prepare(*cube);

        gip.prepare(*sphere);
        //gip.prepare(*bunny2);
        //gip.prepare(*bunny3);
#endif

//        gip.prepare(*cube6);

    }

    for(unsigned int i=0; i<objects.size();++i)
    {
        ThreeDObject * obj = objects[i];
        if(obj->enabled)
            obj->display(shader_prog);
    }
}



void TranslucentMaterials::set_light_and_camera(ShaderProgramDraw& shader_prog)
{

    shader_prog.set_projection_matrix(perspective_Mat4x4f(55, float(window_width)/window_height, 1.0f, 100));
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

/*
    terrain_shader.use();
    set_light_and_camera(terrain_shader);

    //terra.draw(terrain_shader);

    object_shader.use();
    set_light_and_camera(object_shader);
    vector<string> objs;
    //objs.push_back("bunny1");
    //objs.push_back("sphere2");
    //objs.push_back("sphere");
    //objs.push_back("cube");

    draw_objects(object_shader,objs);

    t_shader.use();
    set_light_and_camera(t_shader);
    objs.clear();

#ifdef CUBES
    objs.push_back("cube");
    objs.push_back("cube4");
    objs.push_back("cube7");
    //objs.push_back("cube5");
#endif

#ifdef BUNNIES
    //objs.push_back("bunny1");
#endif


    draw_objects(t_shader,objs);
*/

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
/*
    better_dipole_shader.use();
    set_light_and_camera(better_dipole_shader);
    objs.clear();
    #ifdef CUBES
    objs.push_back("cube6");
    #endif
    draw_objects(better_dipole_shader,objs);

    better_dipole_shader_vertex.use();
    set_light_and_camera(better_dipole_shader_vertex);
    objs.clear();
    #ifdef CUBES
    objs.push_back("cube5");
    #endif
#ifdef BUNNIES
    //objs.push_back("bunny2");
#endif
    draw_objects(better_dipole_shader_vertex,objs);

    directional_dipole_shader.use();
    set_light_and_camera(directional_dipole_shader);
    objs.clear();
    #ifdef CUBES
    objs.push_back("cube9");
    #endif
    draw_objects(directional_dipole_shader,objs);

    directional_dipole_shader_vertex.use();
    set_light_and_camera(directional_dipole_shader_vertex);
    objs.clear();
    #ifdef CUBES
    objs.push_back("cube8");
    #endif
#ifdef BUNNIES
    //objs.push_back("bunny3");
#endif
    draw_objects(directional_dipole_shader_vertex,objs);


    red_shader.use();
    set_light_and_camera(red_shader);
    objs.clear();
    //objs.push_back("light_sphere");

    //objs.push_back("cow1");
    //objs.push_back("cow2");
    //objs.push_back("cow3");

    draw_objects(red_shader,objs);

    plane_shader.use();
    set_light_and_camera(plane_shader);
    objs.clear();
    //objs.push_back("plane");
    draw_objects(plane_shader,objs);
    */
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

    /*
    // Create shader program. Note that this is the only program which has
    // a geometry shader. Also there is only one shader in this function since
    // all geometry is treated in the same way.
    static ShaderProgramDraw wire_shader(shader_path, "wire.vert", "wire.geom", "wire.frag");
    if(reload)
        wire_shader.reload();

    
    glClearColor(0.9f,0.9f,0.9f,0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    wire_shader.use();
    set_light_and_camera(wire_shader);
    Vec2f win_scale = Vec2f(window_width/2.0, window_height/2.0);
    wire_shader.set_uniform("WIN_SCALE", win_scale);
#ifdef SOLUTION_CODE
    Mat4x4f ident = identity_Mat4x4f();
    glUniformMatrix4fv(wire_shader.get_uniform_location("InstanceMatrix"),1,GL_FALSE,
                       (const GLfloat*) &ident);
#endif

    //terra.draw(wire_shader);
    vector<string> objs;
    objs.push_back("bunny1");
    objs.push_back("bunny2");
    objs.push_back("bunny3");
//    objs.push_back("cube4");
//    objs.push_back("cube5");
//    objs.push_back("cube6");

    draw_objects(wire_shader, objs);
    */
}



bool compareVec2fDistanceAscending (Vec2f i,Vec2f j) { return (i.length() < j.length()); }

void TranslucentMaterials::getDiscPoints(vector<Vec3f> * points, const int n, const int m)
{
    vector<vector<Vec2f> > texture;

//#if POINT_DIST == 0
    planeHammersleyCircleMulti(texture, n, m);
//#elif POINT_DIST == 1
//    planeHammersleyCircleMultiExp(texture, n, m,3.0f);
//#else
//    circleUniformPoints(texture, n / 50, m, 50);
//#endif
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

void TranslucentMaterials::render_direct_abuffer(bool reload, ShaderProgramDraw &render_to_array)
{
    //static ShaderProgramDraw obj_shader(shader_path,"object.vert","","object.frag");
    static ShaderProgramDraw clean_array(shader_path,"display_tex.vert","display_tex.geom","ss_render_to_arraymap_imageStore_clear.frag");
    glDisable(GL_BLEND);
#ifdef SINGLE_LIGHT
    static ShaderProgramDraw gbuff_shader(shader_path,"ss_cubemap_gbuffer.vert","","ss_cubemap_gbuffer.frag");
#else
    static ShaderProgramDraw gbuff_shader(shader_path,"ss_cubemap_gbuffer_multilight.vert","ss_cubemap_gbuffer_multilight.geom", "ss_cubemap_gbuffer_multilight.frag");
#endif

    //static ShaderProgramDraw gbuff_quad(shader_path,"ss_cubemap_test_gbuffer.vert","","ss_cubemap_test_gbuffer.frag");
    //static ShaderProgramDraw gbuff_wrap(shader_path,"ss_cubemap_test_wrap_gbuffer.vert","","ss_cubemap_test_wrap_gbuffer.frag");

   // static ShaderProgramDraw render_to_cubemap_test(shader_path,"ss_cubemap_render_to_cubemap.vert","","ss_cubemap_render_to_cubemap.frag");
   // static ShaderProgramDraw render_to_cubemap_test_screen(shader_path,"ss_cubemap_test_render_to_cubemap_screen.vert","","ss_cubemap_test_render_to_cubemap_screen.frag");
   // static ShaderProgramDraw render_to_cubemap_test_cube(shader_path,"ss_cubemap_test_render_to_cubemap_cube.vert","","ss_cubemap_test_render_to_cubemap_cube.frag");

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

    static ArrayImageBuffer imageTexMap(ARRAY_TEXTURE_SIZE,LAYERS);
    Mesh::Texture * colorTexture = imageTexMap.getColorTexture();


    /*
    static ArrayTextureBuffer arraytexmap(ARRAY_TEXTURE_SIZE,LAYERS,1);
    static ArrayTextureBuffer arraytexmap_back(ARRAY_TEXTURE_SIZE,LAYERS,1);
    ArrayTextureBuffer * front;
    ArrayTextureBuffer * back;
    */

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
        test2.reload();
        clean_array.reload();
        //obj_shader.reload();
        gbuff_shader.reload();
        //gbuff_quad.reload();
        //gbuff_wrap.reload();
        render_to_array.reload();
        //render_to_cubemap_test_screen.reload();
        //render_to_cubemap_test_cube.reload();
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

    if(currentFrame < CONVERGENCE_FRAMES)
    {
        static bool initialized = false;

        // Need to disable mipmap afterwards, otherwise the memory space is not reserved.
        imageTexMap.disableMipMaps();
        static GLuint emptyFBO = 0;
        if(!initialized)
        {
            glGenFramebuffers(1, &emptyFBO);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, emptyFBO);
            glFramebufferParameteri(GL_DRAW_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_WIDTH, ARRAY_TEXTURE_SIZE);
            glFramebufferParameteri(GL_DRAW_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_HEIGHT, ARRAY_TEXTURE_SIZE);
            glFramebufferParameteri(GL_DRAW_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_LAYERS, LAYERS);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

            screen_quad->init("","plane",*test_mat);
            vector<Vec3f> * discpoint_data = new vector<Vec3f>();
            getDiscPoints(discpoint_data,discPoints,DISCS);

            Mesh::Texture * tex = new Mesh::Texture("discpoints",GL_TEXTURE_2D, discPoints, DISCS, *discpoint_data);
            tex->init();
            scattering_material->addTexture(tex);

            //scattering_material->addTexture(colorTexture);

            //preparing first color buffer from which to read (just for avoiding errors,
            // the shader already avoids that)
            //scattering_material->addTexture(arraytexmap_back.getColorTexture());
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

        // We need to reset the viewport, since the shadow buffer does not have
        // the same size as the screen window.
        glViewport(0, 0, window_width, window_height);



        Mesh::Texture * vtex = light_buffer.getVertexTexture();
        Mesh::Texture * ntex = light_buffer.getNormalTexture();

        scattering_material->addTexture(vtex);
        scattering_material->addTexture(ntex);

        // TODO RENDER TO DEPTH


        // Rendering to array



        glViewport(0,0,ARRAY_TEXTURE_SIZE,ARRAY_TEXTURE_SIZE);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, emptyFBO);
        colorTexture->setIfImage(true);

        Vec3f radius = Vec3f(29.909f,23.316f, 18.906f); //radius for marble - red 29.909 green 23.316 blue 18.906
        //float trueRadius = clamp01(length(mat * Vec4f(radius[0],0,0,0)));

        float trueRadius = params->circleradius;
        glClearColor(0,0,0,0);

        if(currentFrame == 0)
        {
            clean_array.use();
            clean_array.set_uniform("colorMapSize",ARRAY_TEXTURE_SIZE);
            clean_array.set_uniform("layers", LAYERS);
            GLuint progc = clean_array.prog;
            glProgramUniform1i(progc, glGetUniformLocation(progc, "colorMap"), 0);

            screen_quad->display(clean_array);


        }
        glMemoryBarrier(GL_ALL_BARRIER_BITS);
        render_to_array.use();
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

        render_to_array.set_uniform("viewMatrices", viewMatricesvector, LAYERS);
        render_to_array.set_uniform("layers", LAYERS);
        render_to_array.set_model_matrix(model_identity);
        render_to_array.set_projection_matrix(projection_array);

        GLuint prog = render_to_array.prog;
        glProgramUniform1i(prog, glGetUniformLocation(prog, "colorMap"), 0);
        obj->display(render_to_array);

        glMemoryBarrier(GL_ALL_BARRIER_BITS);

        //glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

        //test_mat->addTexture(imageTexMap.getColorTexture());

        colorTexture->setIfImage(false);

        render_mipmaps.use();
        render_mipmaps.set_uniform("viewMatrices", viewMatricesvector, LAYERS);
        render_mipmaps.set_uniform("layers", LAYERS);
        //front->generateMipMaps();

#ifdef MIPMAPS_USE
        for(int level = 0; level < MIPMAPS; level++)
        {
            glViewport(0, 0, ARRAY_TEXTURE_SIZE / SCALING[level], ARRAY_TEXTURE_SIZE / SCALING[level]);

            /* image processing of mipmaps */
            screen_quad->mesh.getMaterial()->removeTexture(string("colorMap"));
            GLuint sourceTex = 0;
            if(level == 0)
            {
                sourceTex = imageTexMap.getColorTexture()->get_id();
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

        glViewport(0, 0, ARRAY_TEXTURE_SIZE , ARRAY_TEXTURE_SIZE );
        imageTexMap.enableMipMaps();

        /* copying texture in mipmaps */
        for(int level = 0; level < MIPMAPS; level++)
        {
            GLuint target = imageTexMap.getColorTexture()->get_id();
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
#endif
        //test_mat->removeTexture(string("colorMap"));
        test_mat->addTexture(colorTexture);
        //test_mat->addTexture(front->getDepthTexture());

        // Adding the new calculated stuff.
        /*
        Mesh::Texture * color = front->getColorTexture();
        string colname = color->get_name();
        scattering_material->removeTexture(colname); //switching the old color TODO : replace
        scattering_material->addTexture(color);

        Mesh::Texture * depth = front->getDepthTexture();
        scattering_material->addTexture(depth);
        */

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
        glViewport(0,0,window_width,window_height);

        glBindTexture(GL_TEXTURE_2D_ARRAY, colorTexture->get_id());
        float converg = currentFrame / float(CONVERGENCE_FRAMES);
        glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_LOD_BIAS, 3.0f * (1 - converg * converg * converg));
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
        initialized = true;
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
        glBindTexture(GL_TEXTURE_2D_ARRAY, colorTexture->get_id());
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

    test2.use();
    test2.set_uniform("mipmap_LOD",params->LOD);
    if(params->cubemapVisible)
        screen_quad->display(test2);
}

void TranslucentMaterials::render_direct_array(bool reload, ShaderProgramDraw & render_to_array)
{


    static ShaderProgramDraw obj_shader(shader_path,"object.vert","","object.frag");
#ifdef SINGLE_LIGHT
    static ShaderProgramDraw gbuff_shader(shader_path,"ss_cubemap_gbuffer.vert","","ss_cubemap_gbuffer.frag");
#else
    static ShaderProgramDraw gbuff_shader(shader_path,"ss_cubemap_gbuffer_multilight.vert","ss_cubemap_gbuffer_multilight.geom", "ss_cubemap_gbuffer_multilight.frag");
#endif    static ShaderProgramDraw gbuff_quad(shader_path,"ss_cubemap_test_gbuffer.vert","","ss_cubemap_test_gbuffer.frag");
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
    if(params->cubemapVisible)
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
        test2.reload();
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

        render_mipmaps.use();
        render_mipmaps.set_uniform("viewMatrices", viewMatricesvector, LAYERS);
        render_mipmaps.set_uniform("layers", LAYERS);
        //front->generateMipMaps();

        for(int level = 0; level < MIPMAPS; level++)
        {
            glViewport(0, 0, ARRAY_TEXTURE_SIZE / SCALING[level], ARRAY_TEXTURE_SIZE / SCALING[level]);

            /* image processing of mipmaps */
            screen_quad->mesh.getMaterial()->removeTexture(string("colorMap"));
            GLuint sourceTex = 0;
            if(level == 0)
            {
                sourceTex = front->getColorTexture()->get_id();
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

        glViewport(0, 0, ARRAY_TEXTURE_SIZE , ARRAY_TEXTURE_SIZE );
        front->enableMipMaps();

        /* copying texture in mipmaps */
        for(int level = 0; level < MIPMAPS; level++)
        {
            GLuint target = front->getColorTexture()->get_id();
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

    currentFrame++;

    test2.use();
    test2.set_uniform("mipmap_LOD",params->LOD);
    if(params->cubemapVisible)
        screen_quad->display(test2);


}

void TranslucentMaterials::render_direct_test(bool reload, ShaderProgramDraw & render_to_cubemap)
{
    static ShaderProgramDraw obj_shader(shader_path,"object.vert","","object.frag");
    static ShaderProgramDraw gbuff_shader(shader_path,"ss_cubemap_gbuffer.vert","","ss_cubemap_gbuffer.frag");
    static ShaderProgramDraw gbuff_quad(shader_path,"ss_cubemap_test_gbuffer.vert","","ss_cubemap_test_gbuffer.frag");
    static ShaderProgramDraw gbuff_wrap(shader_path,"ss_cubemap_test_wrap_gbuffer.vert","","ss_cubemap_test_wrap_gbuffer.frag");


    static ShaderProgramDraw render_to_cubemap_test(shader_path,"ss_cubemap_render_to_cubemap.vert","","ss_cubemap_render_to_cubemap.frag");
    static ShaderProgramDraw render_to_cubemap_test_screen(shader_path,"ss_cubemap_test_render_to_cubemap_screen.vert","","ss_cubemap_test_render_to_cubemap_screen.frag");
    static ShaderProgramDraw render_to_cubemap_test_cube(shader_path,"ss_cubemap_test_render_to_cubemap_cube.vert","","ss_cubemap_test_render_to_cubemap_cube.frag");
    static ShaderProgramDraw render_combination(shader_path,"ss_cubemap_combination.vert","","ss_cubemap_combination.frag");

    const int GBUFFER_SIZE = 1024;
    static VertexNormalBuffer buff(GBUFFER_SIZE);



    if(reload)
    {
        obj_shader.reload();
        gbuff_shader.reload();
        gbuff_quad.reload();
        gbuff_wrap.reload();
        render_to_cubemap.reload();
        render_to_cubemap_test_screen.reload();
        render_to_cubemap_test_cube.reload();
        render_combination.reload();
    }

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

    gbuff_shader.use();

    buff.enable();

    const float LIGHT_CAMERA_SIZE = 3.0f;

    // Set up a modelview matrix suitable for shadow: Maps from world coords to
    // shadow buffer coords.
    Vec3f v = Vec3f(manager[0].position);
    gbuff_shader.set_view_matrix(lookat_Mat4x4f(v,-v,Vec3f(0,1,0))); //PARALLEL!
    gbuff_shader.set_model_matrix(identity_Mat4x4f());
    gbuff_shader.set_projection_matrix(ortho_Mat4x4f(Vec3f(-LIGHT_CAMERA_SIZE,-LIGHT_CAMERA_SIZE,1),Vec3f(LIGHT_CAMERA_SIZE,LIGHT_CAMERA_SIZE,10)));

    // Switch viewport size to that of shadow buffer.

    glViewport(0, 0, GBUFFER_SIZE, GBUFFER_SIZE);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    // Draw to shadow buffer.
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

    Mesh::Texture * vtex = buff.getVertexTexture();
    Mesh::Texture * ntex = buff.getNormalTexture();

    obj->mesh.getMaterial()->addTexture(vtex);
    obj->mesh.getMaterial()->addTexture(ntex);
    obj->mesh.getMaterial()->addUniform("lightMatrix",mat);

//#define TEST_ONSCREEN_QUAD_2
#ifdef TEST_ONSCREEN_QUAD

    static vector<Vec2f> discpoints2;
        static bool mark2 = false;
        if(!mark2)
        {
            mark2 = true;
            planeHammersley(discpoints2, 150);
        }

    material->addTexture(*vtex);
    material->addTexture(*ntex);

    gbuff_quad.use();
    //gbuff_quad.set_uniform("lightMatrix",mat);
    set_light_and_camera(gbuff_quad);
    material->loadUniforms(gbuff_quad);

    gbuff_quad.set_uniform("discpoints", discpoints2, 150);
    gbuff_quad.set_uniform("samples",params->samples);
    gbuff_quad.set_uniform("discradius",params->circleradius);

    draw_screen_aligned_quad(gbuff_quad);
    return;
#endif

#ifdef GBUFFER_USE_TEST
    gbuff_wrap.use();
    //gbuff_wrap.set_uniform("lightMatrix",mat);
    set_light_and_camera(gbuff_wrap);

    obj->mesh.getMaterial()->addTexture(*vtex);
    obj->mesh.getMaterial()->addTexture(*ntex);
    obj->display(gbuff_wrap);

#endif
    check_gl_error();

    const int CUBEMAP_SIDE_SIZE = 1024;
    const float CAMERA_DISTANCE = 6.0f;
    const float CAMERA_NEAR = 1.0f;
    const float CAMERA_FAR = 21.0f;
    const float CAMERA_SIZE = 6.0f;

    static CubemapBuffer cubemap(CUBEMAP_SIDE_SIZE);

    render_to_cubemap.use();
    check_gl_error();


    Vec3f center = obj->getCenter();
    check_gl_error();
    static Vec3f cameraPositions[6] = {
        center + Vec3f(1,0,0) * CAMERA_DISTANCE, //+X
        center - Vec3f(1,0,0) * CAMERA_DISTANCE, //-X
        center + Vec3f(0,1,0) * CAMERA_DISTANCE, //+Y
        center - Vec3f(0,1,0) * CAMERA_DISTANCE, //-Y
        center + Vec3f(0,0,1) * CAMERA_DISTANCE, //+Z
        center - Vec3f(0,0,1) * CAMERA_DISTANCE  //-Z
    };

    static Mat4x4f viewMatrices[6]  = {
        scaling_Mat4x4f(Vec3f(1,-1,1)) * lookat_Mat4x4f_target(cameraPositions[0], center, Vec3f(0,1,0)), //+X
        scaling_Mat4x4f(Vec3f(1,-1,1)) * lookat_Mat4x4f_target(cameraPositions[1], center, Vec3f(0,1,0)), //-X
        scaling_Mat4x4f(Vec3f(-1,1,1)) * lookat_Mat4x4f_target(cameraPositions[2], center, Vec3f(0,0,1)), //+Y
        scaling_Mat4x4f(Vec3f(-1,1,1)) * lookat_Mat4x4f_target(cameraPositions[3], center, Vec3f(0,0,-1)), //-Y
        scaling_Mat4x4f(Vec3f(1,-1,1)) * lookat_Mat4x4f_target(cameraPositions[4], center, Vec3f(0,1,0)), //+Z
        scaling_Mat4x4f(Vec3f(1,-1,1)) * lookat_Mat4x4f_target(cameraPositions[5], center, Vec3f(0,1,0))  //-Z
    };

    Mat4x4f model = identity_Mat4x4f();
    Mat4x4f projection = ortho_Mat4x4f(Vec3f(-CAMERA_SIZE,-CAMERA_SIZE,CAMERA_NEAR),Vec3f(CAMERA_SIZE,CAMERA_SIZE,CAMERA_FAR));

    glViewport(0,0,CUBEMAP_SIDE_SIZE,CUBEMAP_SIDE_SIZE);


    static float area;
    int DISC_POINTS = 1000;
    const int DISCS = 6;

    static bool mark = false;
    if(!mark)
    {
        vector<Vec3f> * discpoint_data = new vector<Vec3f>();
        mark = true;

        vector<vector<Vec2f> > texture;

#if POINT_DIST == 0
        planeHammersleyCircleMulti(texture, DISC_POINTS, DISCS);
#elif POINT_DIST == 1
        planeHammersleyCircleMultiExp(texture, DISC_POINTS, DISCS,3.0f);
#else
        circleUniformPoints(texture, DISC_POINTS / 50, DISCS, 50);
#endif
        for(int k = 0; k < DISCS; k++)
        {
            cout << "Vector " << k <<endl;
            vector<Vec2f> discpoints = texture[k];
            std::sort(discpoints.begin(),discpoints.end(),compareVec2fDistanceAscending);
            for(int i = 0; i < discpoints.size(); i++)
            {
                //if (k == 0)
                //    cout <<discpoints[i][0] << " " << discpoints[i][1] << endl;
                discpoint_data->push_back(Vec3f(discpoints[i][0],discpoints[i][1],0.0f));

            }
        }
        Mesh::Texture * tex = new Mesh::Texture("discpoints",GL_TEXTURE_2D, DISC_POINTS, DISCS, *discpoint_data);
        tex->init();
        obj->mesh.getMaterial()->addTexture(tex);
        totalArea(*obj,area);
    }

    Vec3f radius = Vec3f(29.909f,23.316f, 18.906f); //radius for marble - red 29.909 green 23.316 blue 18.906
    //float trueRadius = clamp01(length(mat * Vec4f(radius[0],0,0,0)));

    float trueRadius = params->circleradius;
    //render_to_cubemap.set_uniform("discpoints", discpoints, DISC_POINTS);
    render_to_cubemap.set_uniform("one_over_max_samples",1.0f/params->samples);
    render_to_cubemap.set_uniform("one_over_discs",1.0f/DISCS);
    render_to_cubemap.set_uniform("samples",params->samples);
    render_to_cubemap.set_uniform("discradius",trueRadius);
    render_to_cubemap.set_uniform("epsilon_gbuffer", params->epsilon_gbuffer);
    set_light_and_camera(render_to_cubemap);

#ifdef TEST_ONSCREEN_QUAD_2

    //testmaterial->addTexture(*vtex);
    //testmaterial->addTexture(*ntex);

    gbuff_quad.use();
    //gbuff_quad.set_uniform("lightMatrix",mat);
    set_light_and_camera(gbuff_quad);
    obj->mesh.getMaterial()->loadUniforms(gbuff_quad);
    draw_screen_aligned_quad(gbuff_quad);
    return;
#endif

    for(int i = 0; i < 6; i++)
    {
        render_to_cubemap.set_uniform("currentDisc",i);
        render_to_cubemap.set_view_matrix(viewMatrices[i]);
        render_to_cubemap.set_model_matrix(model);
        render_to_cubemap.set_projection_matrix(projection);
        cubemap.enable(i);
        obj->display(render_to_cubemap);
    }

    cubemap.generateMipMaps();
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
    glViewport(0,0,window_width,window_height);

    Mesh::Texture * cube = cubemap.getCubemapTexture();
    Mesh::Texture * depth = cubemap.getCubemapDepthTexture();
    obj->mesh.getMaterial()->addTexture(cube);
    obj->mesh.getMaterial()->addTexture(depth);


//#define CUBEMAP_CUBE_TEST

#ifdef CUBEMAP_TEST_SCREEN_2
//    user.set(cameraPositions[0],-cameraPositions[0]);
    obj_shader.use();
    set_light_and_camera(obj_shader);
    obj->display(obj_shader);
    obj_shader.set_projection_matrix(projection);
    obj_shader.set_view_matrix(viewMatrices[2]);
    obj_shader.set_model_matrix(identity_Mat4x4f());
    obj->display(obj_shader);
#endif


#ifdef CUBEMAP_TEST_SCREEN

    render_to_cubemap_test.use();
    set_light_and_camera(render_to_cubemap_test);
    material->loadUniforms(render_to_cubemap_test);
    draw_screen_aligned_quad(render_to_cubemap_test);
#endif

//#define CUBEMAP_CUBE_TEST
if(params->cubemapVisible)
{
    static ThreeDCube *cubemapplaceholder = new ThreeDCube(10);
    static bool was_here = false;
    if(!was_here)
    {
        was_here = true;
        objects.push_back(cubemapplaceholder);
        cubemapplaceholder->init(" ","cubem",*(obj->mesh.getMaterial()));
        cubemapplaceholder->setScale(Vec3f(CAMERA_SIZE * 2));
        cubemapplaceholder->setRotation(Vec3f(1,0,0), 90);
    }

    render_to_cubemap_test_cube.use();
    set_light_and_camera(render_to_cubemap_test_cube);

    render_to_cubemap_test_cube.set_uniform("areacircle", (float)(trueRadius * trueRadius * M_PI));
    render_to_cubemap_test_cube.set_uniform("one_over_max_samples", 1.0f/params->samples);
    render_to_cubemap_test_cube.set_uniform("total_area", CAMERA_SIZE * CAMERA_SIZE);
    render_to_cubemap_test_cube.set_uniform("mipmap_LOD",params->LOD);
    cubemapplaceholder->setTranslation(center);
    cubemapplaceholder->display(render_to_cubemap_test_cube);
}

    render_combination.use();
    render_combination.set_uniform("centerWorldCoordinates",center);
    render_combination.set_uniform("cameraSize",CAMERA_SIZE);
    render_combination.set_uniform("zFar",CAMERA_FAR);
    render_combination.set_uniform("zNear",CAMERA_NEAR);
    render_combination.set_uniform("shadow_bias", params->shadow_bias);
    render_combination.set_uniform("epsilon_combination", params->epsilon_combination);
    render_combination.set_uniform("one_over_max_samples", 1.0f/params->samples);
    render_combination.set_uniform("total_area", CAMERA_SIZE * CAMERA_SIZE);
    render_combination.set_uniform("mipmap_LOD",params->LOD);
    float worldCircleRadius = params->circleradius * 2 * LIGHT_CAMERA_SIZE;

    render_combination.set_uniform("disc_area", (float)(worldCircleRadius * worldCircleRadius * M_PI));


    render_combination.set_uniform("face_plus_x",params->plusX);
    render_combination.set_uniform("face_minus_x", params->minusX);
    render_combination.set_uniform("face_plus_y", params->plusY);
    render_combination.set_uniform("face_minus_y", params->minusY);
    render_combination.set_uniform("face_plus_z", params->plusZ);
    render_combination.set_uniform("face_minus_z", params->minusZ);
    render_combination.set_uniform("step_tex", 1.0f/CUBEMAP_SIDE_SIZE);
    set_light_and_camera(render_combination);
    obj->display(render_combination);
}

void TranslucentMaterials::render_to_gbuffer(GBuffer& gbuffer, bool reload)
{
    static ShaderProgramDraw objects_render_to_gbuffer(shader_path, "objects_gbuffer.vert", "", "objects_gbuffer.frag");
    static ShaderProgramDraw terrain_render_to_gbuffer(shader_path, "terrain_gbuffer.vert", "", "terrain_gbuffer.frag");
#ifdef SOLUTION_CODE
    static ShaderProgramDraw instanced_objects_render_to_gbuffer(shader_path, "instanced_objects_gbuffer.vert", "", "instanced_objects_gbuffer.frag");
#endif

    if(reload)
    {
        objects_render_to_gbuffer.reload();
        terrain_render_to_gbuffer.reload();
#ifdef SOLUTION_CODE
        instanced_objects_render_to_gbuffer.reload();
#endif
    }
    gbuffer.enable();

    terrain_render_to_gbuffer.use();
    set_light_and_camera(terrain_render_to_gbuffer);

    terra.draw(terrain_render_to_gbuffer);

    objects_render_to_gbuffer.use();
    set_light_and_camera(objects_render_to_gbuffer);
    draw_objects(objects_render_to_gbuffer);

#ifdef SOLUTION_CODE
    instanced_objects_render_to_gbuffer.use();
    set_light_and_camera(instanced_objects_render_to_gbuffer);
    //draw_trees(instanced_objects_render_to_gbuffer);
#else
    //draw_trees(objects_render_to_gbuffer);
#endif
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


TranslucentMaterials::TranslucentMaterials( QWidget* parent)
    : QGLWidget( new Core4_3_context(), (QWidget*) parent),
      ax(0), ay(0), dist(12),ang_x(0),ang_y(0),mouse_x0(0),mouse_y0(0)
    , clearColor(Vec4f(0.0f,0.0f,0.0f,1.0f)), isVertexMode(true), isShadow(true), isGridVisible(true), areAxesVisible(true), params(new TranslucentParameters())
    , render_mode(DRAW_JENSEN), render_method(CUBEMAP_BASE), currentFrame(0)
{
    Light mainLight (light_position, light_diffuse, 1.0f, light_specular, true);
    Light secondaryLight(light_position_2, light_diffuse_2, 15.0f, Vec4f(0.0f), true);
    manager.addLight(mainLight);
    //manager.addLight(secondaryLight);
    setFocusPolicy(Qt::ClickFocus);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(animate()));
    timer->start(16);
}

QImage* TranslucentMaterials::takeScreenshot()
{
    float *pixels;
    // the thingy we use to write files

    uint w = window_width;
    uint h = window_height;
    pixels = new float[w*h*3]; //3 floats = RGB
    // read in the pixel data, TGA's pixels are BGR aligned
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

void TranslucentMaterials::paintGL()
{
#ifdef SOLUTION_CODE
    static QTime stopwatch;
    static int frames=0;
    static vector<int> msecs_total(50);
    glFinish();
    stopwatch.start();
#endif

    setup_shadow(reload_shaders);

    glClearColor(clearColor[0],clearColor[1],clearColor[2],clearColor[3]);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);


    if(isGridVisible) draw_grid(reload_shaders);
    if(areAxesVisible) draw_axes(reload_shaders);

    //draw_bounding_boxes(reload_shaders);

#ifdef SINGLE_LIGHT
    static ShaderProgramDraw render_to_cubemap_jensen(shader_path,"ss_cubemap_render_to_cubemap_jensen.vert","ss_cubemap_render_to_cubemap_array.geom","ss_cubemap_render_to_cubemap_jensen.frag");
    static ShaderProgramDraw render_to_cubemap_jeppe(shader_path,"ss_cubemap_render_to_cubemap_jeppe.vert","ss_cubemap_render_to_cubemap_array.geom","ss_cubemap_render_to_cubemap_jeppe.frag");
#else
    static ShaderProgramDraw render_to_cubemap_jensen(shader_path,"ss_cubemap_render_to_cubemap_jensen.vert","ss_cubemap_render_to_cubemap_array.geom","ss_cubemap_render_to_arraymap_multilight_jensen.frag");
    //static ShaderProgramDraw render_to_cubemap_jensen(shader_path,"ss_cubemap_render_to_cubemap_jensen.vert","ss_cubemap_render_to_cubemap_array.geom","ss_render_to_arraymap_imageStore_multilight_jensen.frag");
    static ShaderProgramDraw render_to_cubemap_jeppe(shader_path,"ss_cubemap_render_to_cubemap_jensen.vert","ss_cubemap_render_to_cubemap_array.geom","ss_cubemap_render_to_arraymap_multilight_jeppe.frag");
#endif

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
            render_direct_test(reload_shaders, render_to_cubemap_jensen);
            break;
        case DRAW_DIRECTIONAL:
            render_direct_array_time(reload_shaders, render_to_cubemap_jeppe);
            break;
        }

    }
    reload_shaders = false;
    check_gl_error();
#ifdef SOLUTION_CODE
    glFinish();
    int msecs = stopwatch.elapsed();
    msecs_total[frames++] = msecs;
    if (frames == 50)
    {
        nth_element(msecs_total.begin(), msecs_total.begin()+25, msecs_total.end());
        qDebug() << "median frame time (msecs)" << msecs_total[25];
        frames = 0;
        emit timeMeasurement(msecs_total[25]);
    }
#endif
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
    QWidget::repaint();
}

void TranslucentMaterials::keyReleaseEvent(QKeyEvent *)
{
    user.stop();
}

ThreeDObject *TranslucentMaterials::getDefaultObject()
{
    Mesh::ScatteringMaterial * scattering_mat = getDefaultMaterial(S_Marble);
    ThreeDObject * bunny1 = new ThreeDObject();
    bunny1->init(objects_path+"dragon.obj", "bunny1", *scattering_mat);
    bunny1->setScale(Vec3f(1.5f));
    //bunny1->setRotation(Vec3f(1,0,0), 90);
    bunny1->setTranslation(Vec3f(0,0,2.0f));
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
