#include "threedgrid.h"

using namespace GLGraphics;
using namespace std;
using namespace CGLA;



void ThreeDGrid::display(ShaderProgramDraw &shader)
{
    shader.set_uniform("color", color);
    ThreeDObject::display(shader);
}

bool ThreeDGrid::init(std::string filename, std::string name, Mesh::Material &material)
{
    vector<Vec3f> verts;
    vector<Vec3f> normals;
    vector<Vec2f> uvs;
    vector<GLuint> ind;
    Vec3f extr_low_left = step * Vec3f(-size/2,-size/2,0.0f);
    Vec3f extr_top_left = step * Vec3f(-size/2,size/2,0.0f);
    Vec3f extr_low_right = step * Vec3f(size/2,-size/2,0.0f);
    int current = 0;
    for(int i = 0; i <= size; i++)
    {

        if(i != size/2)
        {
            Vec3f stepx(i * step,0.0f,0.0f);
            verts.push_back(extr_low_left + stepx);
            verts.push_back(extr_top_left + stepx);

            Vec3f stepy(0.0f,i * step,0.0f);
            verts.push_back(extr_low_left  + stepy);
            verts.push_back(extr_low_right + stepy);

            normals.push_back(Vec3f(0.0f));
            normals.push_back(Vec3f(0.0f));
            normals.push_back(Vec3f(0.0f));
            normals.push_back(Vec3f(0.0f));

            uvs.push_back(Vec2f(0.0f));
            uvs.push_back(Vec2f(0.0f));
            uvs.push_back(Vec2f(0.0f));
            uvs.push_back(Vec2f(0.0f));

            ind.push_back(4*current);
            ind.push_back(4*current+1);
            ind.push_back(4*current+2);
            ind.push_back(4*current+3);
            current++;
        }
    }

    this->mesh.load_external(ind,verts,normals,uvs,material,GL_LINES);
    return true;

}
