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

#define BUNNIES
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
enum RenderMode {DRAW_JENSEN=0, DRAW_BETTER=3, DRAW_DIRECTIONAL=2, DRAW_SSAO=4, DRAW_OBJ=1};
int rendermodes = 2;
RenderMode render_mode = DRAW_OBJ;


LightManager manager;

const Vec4f light_specular(0.6f,0.6f,0.3f,0.6f);
const Vec4f light_diffuse(.5f,.5f,.5f,1.0f);
const Vec4f light_position(0.f,0.f,6.f,1);

void draw_screen_aligned_quad(ShaderProgram& shader_prog)
{
    const Vec2f points[] = {Vec2f(-1,-1), Vec2f(1,-1), Vec2f(-1,1), Vec2f(1,1)};
    GLuint pos_attrib = shader_prog.get_attrib_location("vertex");
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

void TranslucentMaterials::render_direct_test(bool reload)
{
    static ShaderProgramDraw obj_shader(shader_path,"object.vert","","object.frag");
    static ShaderProgramDraw gbuff_shader(shader_path,"ss_cubemap_gbuffer.vert","","ss_cubemap_gbuffer.frag");
    static ShaderProgramDraw gbuff_quad(shader_path,"ss_cubemap_test_gbuffer.vert","","ss_cubemap_test_gbuffer.frag");
    static ShaderProgramDraw gbuff_wrap(shader_path,"ss_cubemap_test_wrap_gbuffer.vert","","ss_cubemap_test_wrap_gbuffer.frag");

    static ShaderProgramDraw render_to_cubemap(shader_path,"ss_cubemap_render_to_cubemap_jensen.vert","","ss_cubemap_render_to_cubemap_jensen.frag");

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

    // Set up a modelview matrix suitable for shadow: Maps from world coords to
    // shadow buffer coords.
    Vec3f v = Vec3f(manager[0].position);
    gbuff_shader.set_view_matrix(lookat_Mat4x4f(v,-v,Vec3f(0,1,0))); //PARALLEL!
    gbuff_shader.set_model_matrix(identity_Mat4x4f());
    gbuff_shader.set_projection_matrix(ortho_Mat4x4f(Vec3f(-3,-3,1),Vec3f(3,3,10)));

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

    obj->mesh.getMaterial()->addTexture(*vtex);
    obj->mesh.getMaterial()->addTexture(*ntex);
    obj->mesh.getMaterial()->addUniform("lightMatrix",mat);
    Mesh::Material * testmaterial = new Mesh::Material();


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
    const int DISC_POINTS = 300;
    const int DISCS = 6;

    static bool mark = false;
    if(!mark)
    {
        vector<Vec3f> * discpoint_data = new vector<Vec3f>();
        mark = true;

        vector<vector<Vec2f> > texture;
        planeHammersleyCircleMulti(texture, DISC_POINTS, DISCS);

        for(int k = 0; k < DISCS; k++)
        {
            cout << "Vector " << k <<endl;
            vector<Vec2f> discpoints = texture[k];
            std::sort(discpoints.begin(),discpoints.end(),compareVec2fDistanceAscending);
            for(int i = 0; i < discpoints.size(); i++)
            {
                //cout <<discpoints[i][0] << " " << discpoints[i][1] << endl;
                discpoint_data->push_back(Vec3f(discpoints[i][0],discpoints[i][1],0.0f));
            }
        }
        Mesh::Texture * tex = new Mesh::Texture("discpoints",GL_TEXTURE_2D, DISC_POINTS, DISCS, *discpoint_data);
        obj->mesh.getMaterial()->addTexture(*tex);
        totalArea(*obj,area);
    }

    Vec3f radius = Vec3f(29.909f,23.316f, 18.906f); //radius for marble - red 29.909 green 23.316 blue 18.906
    //float trueRadius = clamp01(length(mat * Vec4f(radius[0],0,0,0)));

    float trueRadius = params->circleradius;
    //render_to_cubemap.set_uniform("discpoints", discpoints, DISC_POINTS);
    render_to_cubemap.set_uniform("one_over_max_samples",1.0f/DISC_POINTS);
    render_to_cubemap.set_uniform("one_over_discs",1.0f/DISCS);
    render_to_cubemap.set_uniform("samples",params->samples);
    render_to_cubemap.set_uniform("discradius",trueRadius);
    render_to_cubemap.set_uniform("epsilon_gbuffer", params->epsilon_gbuffer);
    set_light_and_camera(render_to_cubemap);

    for(int i = 0; i < 6; i++)
    {
        render_to_cubemap.set_uniform("currentDisc",i);
        render_to_cubemap.set_view_matrix(viewMatrices[i]);
        render_to_cubemap.set_model_matrix(model);
        render_to_cubemap.set_projection_matrix(projection);
        cubemap.enable(i);
        obj->display(render_to_cubemap);
    }

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
    glViewport(0,0,window_width,window_height);

    Mesh::Texture * cube = cubemap.getCubemapTexture();
    Mesh::Texture * depth = cubemap.getCubemapDepthTexture();
    obj->mesh.getMaterial()->addTexture(*cube);
    obj->mesh.getMaterial()->addTexture(*depth);

    testmaterial->addTexture(*cube);
    testmaterial->addTexture(*depth);

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

#ifdef CUBEMAP_CUBE_TEST
    static ThreeDCube *cubemapplaceholder = new ThreeDCube(10);
    static bool was_here = false;
    if(!was_here)
    {
        was_here = true;
        objects.push_back(cubemapplaceholder);
        cubemapplaceholder->init(" ","cubem",*material);
        cubemapplaceholder->setScale(Vec3f(CAMERA_SIZE * 2));
        cubemapplaceholder->setRotation(Vec3f(1,0,0), 90);
    }

    render_to_cubemap_test_cube.use();
    set_light_and_camera(render_to_cubemap_test_cube);

    render_to_cubemap_test_cube.set_uniform("areacircle", (float)(trueRadius * trueRadius * M_PI));
    cubemapplaceholder->mesh.getMaterial()->addTexture(*cube);
    cubemapplaceholder->mesh.getMaterial()->addTexture(*depth);
    cubemapplaceholder->setTranslation(center);
    cubemapplaceholder->display(render_to_cubemap_test_cube);
#endif
    render_combination.use();
    render_combination.set_uniform("centerWorldCoordinates",center);
    render_combination.set_uniform("cameraSize",CAMERA_SIZE);
    render_combination.set_uniform("zFar",CAMERA_FAR);
    render_combination.set_uniform("zNear",CAMERA_NEAR);
    render_combination.set_uniform("shadow_bias", params->shadow_bias);
    render_combination.set_uniform("epsilon_combination", params->epsilon_combination);
    render_combination.set_uniform("one_over_max_samples", 1.0f/DISC_POINTS);
    render_combination.set_uniform("total_area", area);
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

void TranslucentMaterials::render_deferred_ssao(bool reload)
{
    const int NO_DISC_POINTS = 32;
    const int SHADOW_SIZE = 4096;
    // Create all resources.
    static GLuint  ssao_tex, fbo;
    static vector<Vec3f> disc_points;
    static GBuffer gbuffer(window_width, window_height);
    gbuffer.rebuild_on_resize(window_width, window_height);
    static ShaderProgramDraw deferred_ssao(shader_path, "deferred.vert", "", "deferred_ssao.frag");
    static ShaderProgramDraw deferred_combination(shader_path, "deferred.vert", "", "deferred_ssao_combination.frag");
    static ShaderProgramDraw render_to_shadow_map(shader_path, "shadow.vert", "", "shadow.frag");
    static ShadowBuffer shadow_buffer(SHADOW_SIZE);
    const int MMW=window_width/2;
    const int MMH=window_height/2;

    // Reinitialize all shaders and buffers if reload = true
    if(reload)
    {
        gbuffer.initialize(window_width, window_height);
        deferred_ssao.reload();
        deferred_combination.reload();
        render_to_shadow_map.reload();
        shadow_buffer.initialize();
    }
    
    // If reload=true or we are here first time, reinitialize the FBO for SSAO computation.
    static bool was_here = false;
    if(reload || !was_here)
    {
        was_here = true;

        glGenTextures(1, &ssao_tex);
        glBindTexture(GL_TEXTURE_RECTANGLE, ssao_tex);
        glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA16F, MMW, MMH, 0, GL_RGBA, GL_FLOAT, 0);
        
        glGenFramebuffers(1,&fbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, ssao_tex, 0);
        if(glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            cout << "Something wrong with FBO" << endl;
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

        // Also: Create a bunch of vectors for hemisphere sampling.
        gel_srand(0);
        disc_points.clear();
        for(int i=0;i<NO_DISC_POINTS;++i)
        {
            float alpha = 2.0 * M_PI * i / float(NO_DISC_POINTS) - M_PI;
            Vec3f v(cos(alpha),sin(alpha),0);
            v*= gel_rand()/float(GEL_RAND_MAX);
            disc_points.push_back(v);
        }
    }

    // Enable shadow buffer and the program for drawing to shadow buffer
    shadow_buffer.enable();
    render_to_shadow_map.use();
    
    // Set up a modelview matrix suitable for shadow: Maps from world coords to
    // shadow buffer coords.
    Vec3f v = Vec3f(manager[0].position) * 75;
    render_to_shadow_map.set_view_matrix(lookat_Mat4x4f(v,-v,Vec3f(0,0,1)));
    render_to_shadow_map.set_model_matrix(identity_Mat4x4f());
    render_to_shadow_map.set_projection_matrix(ortho_Mat4x4f(Vec3f(-35,-35,50),Vec3f(35,35,150)));

    // Switch viewport size to that of shadow buffer.
    glViewport(0, 0, SHADOW_SIZE, SHADOW_SIZE);
    glClearColor(0,1,0,0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    // Draw to shadow buffer.
#ifdef SOLUTION_CODE
    Mat4x4f ident = identity_Mat4x4f();
    glUniformMatrix4fv(render_to_shadow_map.get_uniform_location("InstanceMatrix"),1,GL_FALSE,
                       (const GLfloat*) &ident);
#endif
    terra.draw(render_to_shadow_map);
    draw_objects(render_to_shadow_map);
    //draw_trees(render_to_shadow_map);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glDrawBuffer(GL_BACK);

    // We need to reset the viewport, since the shadow buffer does not have
    // the same size as the screen window.
    glViewport(0, 0, window_width, window_height);

    // Enable GBuffer and render to GBuffer
    render_to_gbuffer(gbuffer, reload);
    // Entering deferred rendering, so disable depth test
    glDisable(GL_DEPTH_TEST);
    
    // Bind the FBO that we render to in the first pass where we compute the SSAO contribution
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    // We need to change the viewport since we render SSAO to smaller buffer.
    glViewport(0, 0, MMW, MMH);
    glClear(GL_COLOR_BUFFER_BIT);
    deferred_ssao.use();

    // Bind the GBuffer, assign uniforms including the hemisphere direction vectors.
    gbuffer.bind_textures(0, 1, 2);
    deferred_ssao.set_uniform("gtex", 0);
    deferred_ssao.set_uniform("ntex", 1);
    deferred_ssao.set_uniform("ctex", 2);
    glUniform3fv(deferred_ssao.get_uniform_location("disc_points"),NO_DISC_POINTS,
                 reinterpret_cast<float*>(&disc_points[0]));
    deferred_ssao.set_uniform("NO_DISC_POINTS",NO_DISC_POINTS);
    Vec2f windowDimension(window_width, window_height);
    deferred_ssao.set_uniform("win_dim", windowDimension);

    //Bind shadow buffer texture
    shadow_buffer.bind_textures(3);
    deferred_ssao.set_uniform("shadow_map", 3);

    // Tricky stuff: Create an inverse of the modelview matrix to (later) be able
    // to go from the GBuffer coordinates (eye space) to world coordinates. We multiply
    // this inverse onto the matrix created above in order to go directly from eye space
    // to shadow buffer space.
    Mat4x4f mat = translation_Mat4x4f(Vec3f(0.5));
    mat *= scaling_Mat4x4f(Vec3f(0.5));
    mat *= render_to_shadow_map.get_projection_matrix();
    mat *= render_to_shadow_map.get_view_matrix();
    mat *= invert(user.get_view_matrix());
    deferred_ssao.set_uniform("Mat", mat);

    // Draw quad to render SSAO computation
    draw_screen_aligned_quad(deferred_ssao);

    // Bind regular on-screen framebuffer.
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    // Bind program for combining SSAO and other shading.
    deferred_combination.use();
    set_light_and_camera(deferred_combination);

    // Bind texture resources, and assign uniforms
    gbuffer.bind_textures(0, 1, 2);
    deferred_combination.set_uniform("gtex", 0);
    deferred_combination.set_uniform("ntex", 1);
    deferred_combination.set_uniform("ctex", 2);
    
    deferred_combination.use_texture(GL_TEXTURE_RECTANGLE, "ssao_tex", ssao_tex, 3);

    shadow_buffer.bind_textures(4);
    deferred_combination.set_uniform("shadow_map", 4);
    deferred_combination.set_uniform("Mat", mat);
    
    // Go back to right viewport size
    glViewport(0, 0, window_width, window_height);
    
    // Draw quad in order to run the program that combines
    // SSAO and shading.
    draw_screen_aligned_quad(deferred_combination);

    // Reenable depth test
    glEnable(GL_DEPTH_TEST);
}

#ifdef SOLUTION_CODE
void TranslucentMaterials::render_indirect()
{
    static GLuint fbo=0, rb=0, tex=0;
    static ShaderProgram prog("", "$void main(){gl_Position = gl_Vertex;}", "",
                              "$uniform sampler2DRect tex; void main()"
                              "{gl_FragColor = vec4(0.5,0.5,0.9,0)*texture2DRect(tex, gl_FragCoord.xy);}");
    if(!fbo || reload_shaders)
    {
        prog.reload();
        glDeleteTextures(1,&tex);
        glDeleteFramebuffers(1,&fbo);
        glDeleteRenderbuffers(1,&rb);
        
        glGenTextures(1,&tex);
        glBindTexture(GL_TEXTURE_RECTANGLE, tex);
        glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA, window_width, window_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

        glGenFramebuffers(1,&fbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, tex, 0);
        
        glGenRenderbuffers(1,&rb);
        glBindRenderbuffer(GL_RENDERBUFFER, rb);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, window_width, window_height);
        glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rb);
    }
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    render_jensen(reload_shaders);
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glDrawBuffer(GL_BACK);

    glClearColor(0.4f,0.35f,0.95f,0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    prog.use();
    prog.use_texture(GL_TEXTURE_RECTANGLE, "tex", tex);
    draw_screen_aligned_quad(prog);
}
#endif


TranslucentMaterials::TranslucentMaterials( QWidget* parent)
    : QGLWidget( new Core4_3_context(), (QWidget*) parent),
      ax(0), ay(0), dist(12),ang_x(0),ang_y(0),mouse_x0(0),mouse_y0(0)
    , clearColor(Vec4f(0.0f,0.0f,0.0f,1.0f)), isVertexMode(true), isShadow(true), isGridVisible(true), areAxesVisible(true), params(new TranslucentParameters())
{
    Light mainLight (light_position, light_diffuse, 1.0f, light_specular, true);
    manager.addLight(mainLight);
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
    static vector<int> msecs_total(100);
    glFinish();
    stopwatch.start();
#endif

    setup_shadow(reload_shaders);

    glClearColor(clearColor[0],clearColor[1],clearColor[2],clearColor[3]);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);


    if(isGridVisible) draw_grid(reload_shaders);
    if(areAxesVisible) draw_axes(reload_shaders);

    //draw_bounding_boxes(reload_shaders);

    switch(render_mode)
    {
    case DRAW_JENSEN:
        render_jensen(reload_shaders);
        break;
    case DRAW_OBJ:
        render_direct_test(reload_shaders);
        break;
    case DRAW_BETTER:
        render_better_dipole(reload_shaders);
        break;
    case DRAW_DIRECTIONAL:
        render_directional_dipole(reload_shaders);
        break;
    case DRAW_SSAO:
        render_deferred_ssao(reload_shaders);
        break;
    }
    reload_shaders = false;
    check_gl_error();
#ifdef SOLUTION_CODE
    glFinish();
    int msecs = stopwatch.elapsed();
    msecs_total[frames++] = msecs;
    if (frames == 100)
    {
        nth_element(msecs_total.begin(), msecs_total.begin()+50, msecs_total.end());
        qDebug() << "median frame time (msecs)" << msecs_total[50];
        frames = 0;
        emit timeMeasurement(msecs_total[50]);
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
        render_mode = static_cast<RenderMode>((static_cast<int>(render_mode)+1)%rendermodes);
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
    bunny1->init(objects_path+"bunny-simplified.obj", "bunny1", *scattering_mat);
    bunny1->setScale(Vec3f(30.f));
    bunny1->setRotation(Vec3f(1,0,0), 90);
    bunny1->setTranslation(Vec3f(0,0,-1.0f));
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
