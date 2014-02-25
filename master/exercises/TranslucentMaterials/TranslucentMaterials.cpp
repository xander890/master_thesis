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
#include "TranslucentMaterials.h"
#include <GLGraphics/ShaderProgram.h>
#include <GLGraphics/lightmanager.h>
#include <GLGraphics/light.h>

#include "Mesh/proceduralsphere.h"

using namespace std;
using namespace CGLA;
using namespace GLGraphics;


const string shader_path = "./shaders/TranslucentMaterials/";
const string objects_path = "./data/";
static const Vec4f light_ambient(0.3f,0.4f,0.6f,0.4f);

Terrain terra(30,0.025f);
//Vec4f light_position(.3f,.3f,1,0);

User user (&terra);
bool reload_shaders = true;
enum RenderMode {DRAW_NORMAL=0, DRAW_WIRE=1, DRAW_DEFERRED_TOON=2, DRAW_SSAO=3, DRAW_FUR=4};
RenderMode render_mode = DRAW_NORMAL;

LightManager manager;

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

void draw_objects(ShaderProgramDraw& shader_prog)
{

static vector<ThreeDObject> objects;
    if(objects.empty())
    {
        //objects.push_back(ThreeDObject());
        //objects[objects.size()-1].init(objects_path+"cottage_obj/cottage.obj");
        //objects.back().scale(Vec3f(1));
        //objects.back().translate(Vec3f(-14.8718f,-7.91218f,terra.height(-14.8718f,-7.91218f)));

        objects.push_back(ThreeDObject());
        objects[objects.size()-1].init(objects_path+"cow.obj");
        objects.back().scale(Vec3f(.5f));
        objects.back().rotate(Vec3f(1,0,0), 90);
        objects.back().translate(Vec3f(5,7,terra.height(5,7)+1.6f));

        //objects.push_back(ThreeDObject());
        //objects[objects.size()-1].init(objects_path+"portal.obj");
        //objects.back().scale(Vec3f(2));
        //objects.back().translate(Vec3f(0,0,terra.height(0,0)+1.0));
    }


    for(unsigned int i=0;i < objects.size();++i)
        objects[i].display(shader_prog);
}

void draw_sphere(ShaderProgramDraw& shader_prog, Vec3f position, Mesh::Material m, float radius, int LOD)
{

    static bool was_here = false;
    static Mesh::TriangleMesh me;
    if(!was_here)
    {
        vector<Vec3f> vertices;
        vector<Vec3f> normals;
        vector<Vec2f> texcoord;

        sphere(radius, LOD,LOD,vertices,normals,texcoord);


        me.load_external(vertices,normals, texcoord, m ,GL_TRIANGLE_STRIP);

        was_here =true;
    }
    shader_prog.set_model_matrix(translation_Mat4x4f(position));

    me.render(shader_prog);
    check_gl_error();

}

void TranslucentMaterials::set_light_and_camera(ShaderProgramDraw& shader_prog)
{

    shader_prog.set_projection_matrix(perspective_Mat4x4f(55, float(window_width)/window_height, 0.01f, 100));
    shader_prog.set_view_matrix(user.get_view_matrix());
    shader_prog.set_uniform("light_amb", light_ambient);
    check_gl_error();
    manager.loadLights(shader_prog);
    check_gl_error();
    //shader_prog.set_light_position(light_position);
    //shader_prog.set_light_intensities(light_diffuse, light_specular, light_ambient);
}

/*
#ifdef SOLUTION_CODE
void draw_trees(ShaderProgramDraw& shader_prog)
{
    static GLint count;
    static GLuint vao = make_tree(count);

    const int N = 100; // number of trees

    static Mat4x4f M[N];
    static Vec4f mat_diff[N];
    static bool was_here = false;
    if(!was_here)
    {
        gel_srand(0);
        was_here = true;
        for(int i=0;i<N;++i)
        {

            float x = 60*(gel_rand()/float(GEL_RAND_MAX)-0.5f);
            float y = 60*(gel_rand()/float(GEL_RAND_MAX)-0.5f);
            Vec3f p(x,y,terra.height(x,y));
            M[i] = transpose(translation_Mat4x4f(p)*scaling_Mat4x4f(Vec3f(0.01+0.013*gel_rand()/float(GEL_RAND_MAX)))
                             *rotation_Mat4x4f(ZAXIS, 2*M_PI*gel_rand()/float(GEL_RAND_MAX)));
            mat_diff[i] = Vec4f(0.4f,0.3f,0.2f,0.2f) + 0.2f * Vec4f(gel_rand(),gel_rand(),gel_rand(),0)/float(GEL_RAND_MAX);
        }

    }

    bool useInstancing = true;
    if (useInstancing)
    {
        shader_prog.set_model_matrix(identity_Mat4x4f());
        glUniformMatrix4fv(shader_prog.get_uniform_location("InstanceMatrix"),N,GL_FALSE,
                           (const GLfloat*) &M[0]);
        glUniform4fv(shader_prog.get_uniform_location("mat_diff"),N,
                     (const GLfloat*) &mat_diff[0]);
        shader_prog.set_model_matrix(identity_Mat4x4f());
        glBindVertexArray(vao);
        glDrawArraysInstanced(GL_TRIANGLE_STRIP,0, count, N);
    }
    else
    {
        shader_prog.set_model_matrix(identity_Mat4x4f());
        glBindVertexArray(vao);
        for(int i=0;i<N;++i)
        {
            glUniformMatrix4fv(shader_prog.get_uniform_location("InstanceMatrix"),1,GL_FALSE,
                               (const GLfloat*) &M[i]);
            glUniform4fv(shader_prog.get_uniform_location("mat_diff"),1,
                         (const GLfloat*) &mat_diff[i]);
            glDrawArrays(GL_TRIANGLE_STRIP,0, count);
        }
    }
}
#else
    void draw_trees(ShaderProgramDraw& shader_prog)
    {
        static GLint count;
        static GLuint vao = make_tree(count);
        shader_prog.set_model_matrix(translation_Mat4x4f(Vec3f(10,10,terra.height(10,10)+0.0))
                                     *scaling_Mat4x4f(Vec3f(0.01)));
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES,0, count);
        glBindVertexArray(0);
    }
#endif
*/

void TranslucentMaterials::render_direct(bool reload)
{
    // Load shaders for terrain and general objects. The terrain color is computed
    // procedurally, so we need a different shader.

    static ShaderProgramDraw terrain_shader(shader_path, "terrain.vert", "", "terrain.frag");

    static ShaderProgramDraw object_shader(shader_path, "object.vert", "", "object.frag");

    static ShaderProgramDraw t_shader(shader_path, "translucent.vert", "", "translucent.frag");

    if(reload)
    {
        terrain_shader.reload();
        object_shader.reload();
        t_shader.reload();
    }

    glClearColor(0.4f,0.35f,0.95f,0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    terrain_shader.use();
    set_light_and_camera(terrain_shader);

    terra.draw(terrain_shader);

    object_shader.use();
    set_light_and_camera(object_shader);
    draw_objects(object_shader);

    Mesh::Material m;
    draw_sphere(object_shader, Vec3f(5.0f, 0.0f, 5.0f), m, 2.0f, 300);

    t_shader.use();
    set_light_and_camera(t_shader);

    draw_sphere(t_shader, Vec3f(0.0f, 0.0f, 5.0f), m, 2.0f, 300);

#ifdef SOLUTION_CODE
    //instanced_object_shader.use();
    //set_light_and_camera(instanced_object_shader);
    //draw_trees(instanced_object_shader);
#else
    //draw_trees(object_shader);
#endif
    check_gl_error();
}

void TranslucentMaterials::render_direct_wireframe(bool reload)
{
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

    terra.draw(wire_shader);
    draw_objects(wire_shader);
    Mesh::Material m;
    draw_sphere(wire_shader, Vec3f(5.0f, 0.0f, 5.0f), m, 2.0f, 300);




    draw_sphere(wire_shader, Vec3f(0.0f, 0.0f, 5.0f), m, 2.0f, 300);
    //draw_trees(wire_shader);
}

void TranslucentMaterials::render_direct_fur(bool reload)
{
    static GLuint texname=0;
    static bool was_here = false;

     if(!was_here)
     {
         was_here = true;
         int width = 32;
         int height = 32;
         int depth = 32;
         vector<float> texels(width*height*depth*3);
         for (int i = 0; i < width*height*depth*3; ++i)
             texels[i] =float(gel_rand()) / GEL_RAND_MAX;

         glGenTextures(1, &texname);
         glBindTexture(GL_TEXTURE_3D, texname);
         glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
         glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
         glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
         glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
         glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
         glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB, width, height, depth, 0, GL_RGB, GL_FLOAT, &texels[0]);
     }

    // Create shader program. Note that this is the only program which has
    // a geometry shader. Also there is only one shader in this function since
    // all geometry is treated in the same way.
    static ShaderProgramDraw fur_shader(shader_path, "fur.vert", "", "fur.frag");
    if(reload)
        fur_shader.reload();

    glClearColor(0.9f,0.9f,0.9f,0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    fur_shader.use();
    set_light_and_camera(fur_shader);
    fur_shader.use_texture(GL_TEXTURE_3D, "noisetex", texname, 0);
    for(int i=0;i<30;++i)
    {
        fur_shader.set_uniform("s", float(i/30.0));
        terra.draw(fur_shader);
        draw_objects(fur_shader);
    }
    glDisable(GL_BLEND);
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

void TranslucentMaterials::render_deferred_toon(bool reload)
{
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
    render_direct(reload_shaders);
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glDrawBuffer(GL_BACK);

    glClearColor(0.4f,0.35f,0.95f,0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    prog.use();
    prog.use_texture(GL_TEXTURE_RECTANGLE, "tex", tex);
    draw_screen_aligned_quad(prog);
}
#endif


TranslucentMaterials::TranslucentMaterials( const QGLFormat& format, QWidget* parent)
    : QGLWidget( new Core3_2_context(format), (QWidget*) parent),
      ax(0), ay(0), dist(12),ang_x(0),ang_y(0),mouse_x0(0),mouse_y0(0)
{
    static const Vec4f light_specular(0.6f,0.6f,0.3f,0.6f);
    static const Vec4f light_diffuse(0.6f,0.6f,0.3f,0.6f);

    Vec4f light_position(.3f,.3f,1,0);

    Light mainLight (light_position, light_diffuse, light_specular, true);
    manager.addLight(mainLight);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(animate()));
    timer->start(16);
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

    switch(render_mode)
    {
    case DRAW_NORMAL:
#ifdef SOLUTION_CODE
        //render_indirect();
#endif

        render_direct(reload_shaders);
        break;
    case DRAW_FUR:
        render_direct_fur(reload_shaders);
        break;
    case DRAW_WIRE:
        render_direct_wireframe(reload_shaders);
        break;
    case DRAW_DEFERRED_TOON:
        render_deferred_toon(reload_shaders);
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
    QWidget::repaint();
}

void TranslucentMaterials::keyPressEvent(QKeyEvent *e)
{
    static bool is_full_screen = false;
    switch(e->key())
    {
    case Qt::Key_Escape: exit(0);
    case ' ':
        render_mode = static_cast<RenderMode>((static_cast<int>(render_mode)+1)%5);
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
        user.set(Vec3f(-7.60765f, 14.907f, 4.37377f), Vec3f(0.333226f, -0.925571f, 0.179661f));
        break;
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

int main(int argc, char *argv[])
{

    const int WINDOW_WIDTH = 1136;
    const int WINDOW_HEIGHT = 640;

    qDebug() << argv[0] << " - " << argv[1] << endl;
    QApplication a( argc, argv );

    QGLFormat glFormat;
    glFormat.setVersion( 4, 3 );
    glFormat.setProfile( QGLFormat::CoreProfile ); // Requires >=Qt-4.8.0
    TranslucentMaterials w( glFormat);
    w.resize(WINDOW_WIDTH, WINDOW_HEIGHT);
    w.setFocusPolicy(Qt::ClickFocus);
    w.show();

    return a.exec();
}