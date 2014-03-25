#include "threedline.h"

using namespace GLGraphics;
using namespace std;
using namespace CGLA;

void ThreeDLine::display(ShaderProgramDraw &shader)
{
    shader.set_uniform("color", color);
    ThreeDObject::display(shader);
}

bool ThreeDLine::init(std::string filename, std::string name, Mesh::Material &material)
{
    vector<Vec3f> verts;
    vector<Vec3f> normals;
    vector<Vec2f> uvs;
    vector<GLuint> ind;

    verts.push_back(Vec3f(0.0f,0.0f,-1.0f));
    verts.push_back(Vec3f(0.0f,0.0f,1.0f));
    normals.push_back(Vec3f(0.0f));
    normals.push_back(Vec3f(0.0f));
    uvs.push_back(Vec2f(0.0f));
    uvs.push_back(Vec2f(0.0f));
    ind.push_back(0);
    ind.push_back(1);

    this->mesh.load_external(ind,verts,normals,uvs,material,GL_LINES);
    return true;
}
