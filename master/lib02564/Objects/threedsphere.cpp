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

    sphere(1.0f, LOD,LOD/2,vertices,normals,texcoord);
    this->mesh.load_external(vertices,normals,texcoord,material,GL_TRIANGLE_STRIP);
    return true;
}


