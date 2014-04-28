#include "dipolegpu.h"
#include <algorithm>
#include <Utils/areaestimator.h>
#include <Mesh/TriangleMesh.h>
#include <GLGraphics/ThreeDObject.h>
#include <iostream>
#include <fstream>
#include "Utils/miscellaneous.h"
#include "Utils/cglautils.h"
#include <cmath>

using namespace CGLA;
using namespace std;
using namespace GLGraphics;
using namespace Mesh;

void DipoleGPU::prepare(ThreeDObject &three)
{
    TriangleMesh mesh = three.mesh;
    RawMeshData data;
    mesh.getRawData(data);

    vector<Vec3f> vertices = data.vertices;
    vector<Vec3f> normals = data.normals;
    GLenum mode = mesh.getMode();

    if(mode != GL_TRIANGLE_STRIP && mode != GL_TRIANGLES)
    {
        return;
    }
    //
    // Area estimation step
    //
    float result;
    Mat3x3f mx = Mat3x3f(three.getModelMatrix());
    totalArea(three, result);
    cout << mx << "Area " << three.name << ": " << result << endl;

    vector<float> areas;
    perVertexArea(mesh,mode,mx,areas);

    float total = 0.0f;

    vector<Vec3f> areasEnlarged(areas.size());
    for(int i = 0; i < areas.size(); i++)
    {
        total += areas.at(i);
        areasEnlarged[i] = Vec3f(areas[i]);
    }
    cout << "Sum of single areas : " << total <<endl;

    Material * mat = mesh.getMaterial();
    float size = sqrt((float)vertices.size());
    int textureSize = (int)pow(2,ceil(log(size)/log(2.0f)));
    static Vec3f zero = Vec3f(0.0f);
    while(vertices.size() < textureSize * textureSize)
    {
        vertices.push_back(zero);
        normals.push_back(zero);
        areasEnlarged.push_back(zero);
    }
    Texture * v_tex = new Texture("vertices",GL_TEXTURE_2D,textureSize,textureSize,vertices);
    Texture * n_tex = new Texture("normals",GL_TEXTURE_2D,textureSize,textureSize,normals);
    Texture * area_tex = new Texture("areas",GL_TEXTURE_2D,textureSize,textureSize,areasEnlarged);

    mat->addTexture(v_tex);
    mat->addTexture(n_tex);
    mat->addTexture(area_tex);
    mat->addUniform("vertex_size",(int)vertices.size());
    mat->addUniform("vertex_tex_size",(int)textureSize);

    three.mesh.build_vertex_array_object();
}
