#include "ThreeDCube.h"
#include"Mesh/proceduralsphere.h"
#include "CGLA/Mat3x3f.h"
using namespace GLGraphics;
using namespace CGLA;
using namespace std;
#define ONE_THIRD 1.0f/3.0f

void ThreeDCube::display(GLGraphics::ShaderProgramDraw &shader)
{
    Mesh::RawMeshData data;
    mesh.getRawData(data);
    vector<Vec3f> & v = data.vertices;
    shader.set_uniform("d",v,1000);

    ThreeDObject::display(shader);
}

bool ThreeDCube::init(std::string filename, std::string name, Mesh::Material & material)
{
    this->name = name;
    vector<Vec3f> vertices;
    vector<Vec3f> normals;
    vector<Vec2f> texcoord;
    vector<GLuint> indices;

    CGLA::Axis axes[6] = {YAXIS,YAXIS,YAXIS,YAXIS,XAXIS,XAXIS};
    float angles[6] = {0.f, M_PI/2, M_PI, -M_PI/2, +M_PI/2, -M_PI/2};
    Vec3f translation = Vec3f(.0f,.0f,.5f);

    Vec2f uvFaceOrigin[6] = {Vec2f(0.0f,0.0f),Vec2f(0.0f,ONE_THIRD),Vec2f(0.0f,2 * ONE_THIRD), Vec2f(ONE_THIRD,0.0f),Vec2f(ONE_THIRD,ONE_THIRD), Vec2f(ONE_THIRD,2 * ONE_THIRD)};

    vector<Vec3f> facevertices;
    vector<Vec3f> facenormals;
    vector<Vec2f> facetexcoord;
    vector<GLuint> faceindices;
    generateFace(facevertices,facenormals,facetexcoord,faceindices);

    for(int i = 0; i < 6; i++)
    {
        unsigned int indexOffSet = vertices.size();
        Mat3x3f rot = rotation_Mat3x3f(axes[i],angles[i]);

        for(int j = 0; j < faceindices.size(); j++)
        {
            indices.push_back(faceindices[j] + indexOffSet);
        }

        for(int k = 0; k < facevertices.size(); k++)
        {
            Vec3f affine = rot * (facevertices[k] + translation);
            vertices.push_back(affine);
            Vec3f normal = rot * facenormals[k];
            normals.push_back(normal);
            Vec2f uv = uvFaceOrigin[i] + facetexcoord[k] * ONE_THIRD;
            texcoord.push_back(uv);
        }
    }

    this->mesh.load_external(indices,vertices,normals,texcoord,material,GL_TRIANGLE_STRIP);
    return true;
}

//generates a single face as a triangle strip, with the whole uvs
void ThreeDCube::generateFace(vector<Vec3f> & vertices, vector<Vec3f> & normals, vector<Vec2f> & texcoord, vector<GLuint> & indices)
{
    int currentVertex = 0;
    Vec3f normal(0.0f,0.0f,1.0f);

    indices.push_back(0);
    for(int i = 0; i < LOD; i++)
    {
        float u = ((float)i/(LOD - 1));

        for(int j = 0; j < LOD; j++)
        {
            float v = ((float)j / (LOD - 1));
            Vec3f vertex(u - 0.5f,v - 0.5f,0.0f);
            vertices.push_back(vertex);
            normals.push_back(normal);
            texcoord.push_back(Vec2f(u,v));

            if(i > 0)
            {
                if(j == 0)
                {
                    indices.push_back(currentVertex - LOD);
                }

                indices.push_back(currentVertex - LOD);
                indices.push_back(currentVertex);

                if(j == LOD - 1 )
                {
                    indices.push_back(currentVertex);
                }
            }

            currentVertex++;
        }
    }
}
