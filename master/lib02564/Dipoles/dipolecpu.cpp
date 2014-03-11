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
    vector<Vec3f> reflective(limit);

    for(int i = 0; i < limit; i++)
    {
        Vec3f xo = mat * vertices[i];
        Vec3f no = normalize(mat * normals[i]);
        Vec3f toEye = user.get_pos() - xo;
        Vec3f wo = normalize(toEye);

        Vec3f xoLight = normalize(Vec3f(light.position));
       // Vec3f xo = Vec3f(0.939205f, -0.343331f, 0.00428914);
       // Vec3f no = xo;

        for(int j = 0; j < limit; j++)
        {
            //std::cout << "Calculating dipole..." << j << std::endl;
            //if(i == j)
            //{
                Vec3f xi = mat * vertices[j];
                Vec3f ni = normalize(mat * normals[j]);

                Vec3f toLight = Vec3f(light.position); // dir light
                Vec3f wi = normalize(toLight);

                float l = length(toLight);

                float dot_n_w = dot(ni,wi);
                Vec3f Lo(0.0f);

                if(dot_n_w > 0.0f) //visibility term (for now)
                {
                    Vec3f Ei= Vec3f(light.diffuseIntensity);
                    Ei *= (dot_n_w);
                    Vec3f S = bssrdf.evaluate(xi,wi,ni,xo,wo,no);
                    Lo = Ei * S * areas[j];
                }
                resultColors[i] += Lo;
            //}
        }
        std::cout << "Calculating dipole... "<< i <<" (" << (100*(float)i) / vertices.size() << "\%) " << resultColors[i][0] << " " << xo << wo << light.position <<std::endl;
    }

    std::cout<< "Mi" << min <<endl;
    three.mesh.add("translucent", resultColors);
    //three.mesh.add("reflective",reflective);
    three.mesh.build_vertex_array_object();
}



