#include "threedwirecube.h"

using namespace GLGraphics;
using namespace std;
using namespace CGLA;

void ThreeDWireCube::display(ShaderProgramDraw &shader)
{
    shader.set_uniform("color", color);
    ThreeDObject::display(shader);
}

bool ThreeDWireCube::init(std::string filename, std::string name, Mesh::Material &material)
{
    this->firstTime = true;
    vector<Vec3f> verts;
    vector<Vec3f> normals;
    vector<Vec2f> uvs;
    vector<GLuint> ind;

    /*
     * Cube I'm using (indices)
     *
     *    6-------7
     *   /|      /|
     *  2-------3 |
     *  | |     | |
     *  | 4-----|-5
     *  |/      |/
     *  0-------1
     */
    verts.push_back(Vec3f(-0.5f,-0.5f,-0.5f));
    verts.push_back(Vec3f(+0.5f,-0.5f,-0.5f));
    verts.push_back(Vec3f(-0.5f,-0.5f,+0.5f));
    verts.push_back(Vec3f(+0.5f,-0.5f,+0.5f));
    verts.push_back(Vec3f(-0.5f,+0.5f,-0.5f));
    verts.push_back(Vec3f(+0.5f,+0.5f,-0.5f));
    verts.push_back(Vec3f(-0.5f,+0.5f,+0.5f));
    verts.push_back(Vec3f(+0.5f,+0.5f,+0.5f));

    //no shading or textures, no need of normals or uvs
    for(int i = 0; i < 8; i++)
    {
        normals.push_back(Vec3f(0.0f));
        uvs.push_back(Vec2f(0.0f));
    }

    ind.push_back(0);
    ind.push_back(1);

    ind.push_back(1);
    ind.push_back(3);

    ind.push_back(3);
    ind.push_back(2);

    ind.push_back(2);
    ind.push_back(0);

    ind.push_back(0);
    ind.push_back(4);

    ind.push_back(1);
    ind.push_back(5);

    ind.push_back(3);
    ind.push_back(7);

    ind.push_back(2);
    ind.push_back(6);

    ind.push_back(4);
    ind.push_back(6);

    ind.push_back(6);
    ind.push_back(7);

    ind.push_back(7);
    ind.push_back(5);

    ind.push_back(5);
    ind.push_back(4);

    this->mesh.load_external(ind,verts,normals,uvs,material,GL_LINES);
    return true;
}


void ThreeDWireCube::setBounds(Mesh::BoundingBox &box)
{
    this->setBounds(box.low, box.high);
}

void ThreeDWireCube::setBounds(CGLA::Vec3f & low, CGLA::Vec3f & high)
{
    Vec3f pos = 0.5 * (low + high);
    this->translate(pos);
    Vec3f scale = high - low;
    this->setScale(scale);
}
