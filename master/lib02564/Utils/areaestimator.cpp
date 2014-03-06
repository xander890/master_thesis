#include "areaestimator.h"

using namespace std;
using namespace CGLA;
using namespace Mesh;

AreaEstimator::AreaEstimator()
{
}

void computeLimits(int & stride, int & limit, GLenum & mode, int size)
{
    if(mode == GL_TRIANGLES)
    {
        stride = 3;
        limit = size - size % 3;
    }
    else if (mode == GL_TRIANGLE_STRIP)
    {
        stride = 1;
        limit = size - 2;
    }
    else
    {
        cout << "Area estimation not supported for types different than GL_TRIANGLES and GL_TRIANGLE_STRIP" << endl;
        return;
    }
}

void AreaEstimator::totalArea(Mesh::TriangleMesh &mesh, GLenum mode, Mat3x3f modelmatrix, float &result)
{

    RawMeshData data;
    mesh.getRawData(data);
    vector<GLuint> indices = data.indices;
    vector<Vec3f> vertices = data.vertices;

    int stride;
    int limit;
    computeLimits(stride,limit,mode,indices.size());

    result = 0.0f;


    for(int i = 0; i < limit; i += stride)
    {
        Vec3f v1 = modelmatrix * vertices[indices[i]];
        Vec3f v2 = modelmatrix * vertices[indices[i+1]];
        Vec3f v3 = modelmatrix * vertices[indices[i+2]];

        result += area(v1,v2,v3);
    }
}

void AreaEstimator::perVertexArea(TriangleMesh &mesh, GLenum mode, Mat3x3f modelmatrix, std::vector<float> &areas)
{
    // Calculate area based on barycenter

    RawMeshData data;
    mesh.getRawData(data);
    vector<GLuint> indices = data.indices;
    vector<Vec3f> vertices = data.vertices;

    int stride;
    int limit;
    computeLimits(stride,limit,mode,indices.size());

    areas = vector<float>(vertices.size());
    std::fill(areas.begin(),areas.end(), 0.0f);

    for(int i = 0; i < limit; i += stride)
    {
        GLuint i1 = indices[i];
        GLuint i2 = indices[i+1];
        GLuint i3 = indices[i+2];

        Vec3f v1 = modelmatrix * vertices[i1];
        Vec3f v2 = modelmatrix * vertices[i2];
        Vec3f v3 = modelmatrix * vertices[i3];

        Vec3f b = 1/(3.0f) * (v1+v2+v3);
        Vec3f h12 = (v1 + v2) * 0.5;
        Vec3f h13 = (v1 + v3) * 0.5;
        Vec3f h23 = (v2 + v3) * 0.5;

        areas[i1] += area(v1,h12,b) + area(v1,h13,b);
        areas[i2] += area(v2,h12,b) + area(v2,h23,b);
        areas[i3] += area(v3,h23,b) + area(v3,h13,b);
    }
}

float AreaEstimator::area(Vec3f & v1, Vec3f & v2, Vec3f & v3)
{
     return 0.5 * length(cross(v3-v1,v2-v1));
}


