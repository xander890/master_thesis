#include "dipolecpu.h"
#include <algorithm>
#include <Utils/areaestimator.h>
#include <Mesh/TriangleMesh.h>
#include <GLGraphics/ThreeDObject.h>
#include <iostream>
#include <fstream>
#include "Utils/miscellaneous.h"
#include "Utils/cglautils.h"

using namespace CGLA;
using namespace std;
using namespace GLGraphics;

void DipoleCPU::calculate(ThreeDObject &three, std::vector<Vec3f> &resultColors, BSSRDF &bssrdf)
{
    Mesh::TriangleMesh mesh = three.mesh;
    Mesh::RawMeshData data;
    mesh.getRawData(data);

    vector<Vec3f> vertices = data.vertices;
    vector<GLuint> indices = data.indices;
    vector<Vec3f> normals = data.normals;
    cout << "Started computation of dipole. "<< vertices.size() << " vertices found." << std::endl;
    GLenum mode = mesh.getMode();

    //
    // Area estimation step
    //
    float result;
    Mat3x3f mat;
    convertMat4x4toMat3x3(three.getModelMatrix(), mat);
    totalArea(mesh,mode,mat, result);
    cout << mat << "Area " << three.name << ": " << result << endl;

    vector<float> areas;
    perVertexArea(mesh,mode,mat,areas);

    float total = 0.0f;
    for(int i = 0; i < areas.size(); i++)
        total += areas.at(i);
    cout << "Sum of single areas : " << total <<endl;

    //
    // Calculating dipole with the directional approximation and the rendering equation
    //
    resultColors = vector<Vec3f>(vertices.size());
    std::fill(resultColors.begin(),resultColors.end(),Vec3f(0.0f));

    int limit = vertices.size();

    Vec3f min = Vec3f(1000000.0f);

    for(int i = 0; i < limit; i++)
    {
        Vec3f xo = mat * vertices[i];
        Vec3f no = normalize(mat * normals[i]);
        Vec3f wo = normalize(Vec3f(light.position) - mat * xo);

       // Vec3f xo = Vec3f(0.939205f, -0.343331f, 0.00428914);
       // Vec3f no = xo;

        for(int j = 0; j < vertices.size(); j++)
        {
            //std::cout << "Calculating dipole..." << j << std::endl;
            //if(i != j)
            //{
                Vec3f xi = mat * vertices[j];
                Vec3f ni = normalize(mat * normals[j]);
                Vec3f toLight = Vec3f(light.position) - xi;

                Vec3f wi = normalize(toLight);
                float l = length(toLight);

                Vec3f S(0.0f);
                float dot_n_w = dot(ni,wi);
                Vec3f Li = light.diffuseIntensity / (l*l);
                if(dot_n_w > 0.0f)
                {
                    S = bssrdf.evaluate(xi,wi,ni,xo,no);
                }
                Vec3f Lo = Li * max(0.0f,dot_n_w) * S * areas[j] ;
                resultColors[i] += Lo;
            //}
        }
        std::cout << "Calculating dipole... "<< i <<" (" << (100*(float)i) / vertices.size() << "\%) " << resultColors[i][0] << " " << xo <<dot(wo, no) << light.position <<std::endl;
    }

    std::cout<< "Mi" << min <<endl;
    three.mesh.add("translucent", resultColors);
    three.mesh.build_vertex_array_object();
}



