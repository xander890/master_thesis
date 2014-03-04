#include "threedplane.h"
#include "GLGraphics/dipolecpu.h"
#include "Mesh/scatteringmaterial.h"
#include "Mesh/Material.h"

using namespace CGLA;
using namespace std;
using namespace GLGraphics;

void ThreeDPlane::display(ShaderProgramDraw & shader)
{
    shader.use_texture(GL_TEXTURE_2D, "maintexture", tex, tex);
    ThreeDObject::display(shader);
}

bool ThreeDPlane::init(std::string filename, std::string name)
{
    this->name = name;
    vector<Vec3f> vertices;
    vector<Vec3f> normals;
    vector<Vec2f> texcoord;

    vertices.push_back(Vec3f(-1.f,1.f,0.0f));
    vertices.push_back(Vec3f(-1.f,-1.f,0.0f));
    vertices.push_back(Vec3f(1.f,1.f,0.0f));
    vertices.push_back(Vec3f(1.f,-1.f,0.0f));

    for(int i = 0; i < 4; i++)
        normals.push_back(Vec3f(0.0f,0.0f,1.0f));
    for(int i = 0; i < 4; i++)
        texcoord.push_back(Vec2f((vertices[i][0] + 1.0f)*0.5f,(vertices[i][1] + 1.0f)*0.5f));


    this->mesh.load_external(vertices,normals,texcoord,Mesh::Material(),GL_TRIANGLE_STRIP);
    //this->mesh.build_vertex_array_object();

    int MMW = TEXTURE_SIZE;
    int MMH = TEXTURE_SIZE;

    vector<Vec3f> texarray(TEXTURE_SIZE*TEXTURE_SIZE);

    DipoleCPU dip;
    dip.light = light;
    dip.material = Mesh::ScatteringMaterial();
    dip.calculate2x2Texture(80.0f,texarray,TEXTURE_SIZE);
//    for(int i = 0; i < 256*256; i++) texarray[i] = (i*1.0)/(256*256);

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, MMW, MMH, 0, GL_BGR, GL_FLOAT, &texarray[0]);
   // glGenerateMipmap(GL_TEXTURE_2D);
    return true;
}
