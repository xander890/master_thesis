#include "threedsphere.h"
#include"Mesh/proceduralsphere.h"

using namespace GLGraphics;
using namespace CGLA;
using namespace std;

void ThreeDSphere::display(GLGraphics::ShaderProgramDraw &shader)
{
    ThreeDObject::display(shader);
}

bool ThreeDSphere::init(std::string filename, std::string name)
{
    this->name = name;
    vector<Vec3f> vertices;
    vector<Vec3f> normals;
    vector<Vec2f> texcoord;
    vector<GLuint> indices;

    sphere(1.0f, LOD,LOD/2,indices,vertices,normals,texcoord);
    this->mesh.load_external(indices,vertices,normals,texcoord,material,GL_TRIANGLE_STRIP);
    return true;
}


