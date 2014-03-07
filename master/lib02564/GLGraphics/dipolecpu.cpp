#include "dipolecpu.h"
#include <algorithm>
#include <Utils/areaestimator.h>
#include <Mesh/TriangleMesh.h>
#include <GLGraphics/ThreeDObject.h>
#include <iostream>
#include <fstream>

using namespace CGLA;
using namespace std;
using namespace GLGraphics;

#define EPSILON_MU 0.01f

void convertMat4x4toMat3x3(const Mat4x4f & in, Mat3x3f & out)
{
    out[0][0] = in[0][0];
    out[0][1] = in[0][1];
    out[0][2] = in[0][2];
    out[1][0] = in[1][0];
    out[1][1] = in[1][1];
    out[1][2] = in[1][2];
    out[2][0] = in[2][0];
    out[2][1] = in[2][1];
    out[2][2] = in[2][2];
}

void DipoleCPU::calculate(ThreeDObject & three, std::vector<CGLA::Vec3f> &resultColors)
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
    AreaEstimator a;
    float result;
    Mat3x3f mat;
    convertMat4x4toMat3x3(three.getModelMatrix(), mat);
    a.totalArea(mesh,mode,mat, result);
    cout << mat << "Area " << three.name << ": " << result << endl;
    vector<float> areas;
    a.perVertexArea(mesh,mode,mat,areas);

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
                Vec3f wi = normalize(Vec3f(light.position));
                Vec3f S(0.0f);
                float dot_n_w = dot(ni,wi);
                Vec3f Li = light.diffuseIntensity ;
                //S = S_finite(xi,wi,xo,ni,no);
                Vec3f Lo = S * areas[j] * material.T12 * material.T21 * Li;
                resultColors[j] += Lo ;
                if(i != j && length(xo - xi) < 0.0001f)
                {
                    cout << "FO " << i << " " << j;
                }

            //}
        }
        resultColors[i] = Vec3f(areas[i]);
        if(resultColors[i][0] < min[0] && xo[2] > 0.5)
            min = resultColors[i];
        std::cout << "Calculating dipole... "<< i <<" (" << (100*(float)i) / vertices.size() << "\%) " << resultColors[i] << " " << xo <<dot(wo, no) << light.position <<std::endl;
    }
    std::cout<< "Mi" << min <<endl;
    three.mesh.add("translucent", resultColors);
    three.mesh.build_vertex_array_object();
}

Vec3f DipoleCPU::S_infinite(Vec3f _x, Vec3f _w12, Vec3f _r, Vec3f _no)
{
    Vec3f _r_tr = material.transmissionCoefficient * _r;
    Vec3f _r_tr_p1 = _r_tr + Vec3f(1.0f);
    float x_dot_w12 = dot(_x,_w12);
    float no_dot_w12 = dot(_no,_w12);
    float x_dot_no = dot(_x,_no);
    Vec3f _D = material.D;
    Vec3f _T(exp(-_r_tr[0]),exp(-_r_tr[1]),exp(-_r_tr[2]));
    Vec3f coeff = _T / powVec3f(_r,3.0f);
    Vec3f first = material.C_s * ( (_r * _r) / _D + 3 * _r_tr_p1 * x_dot_w12);
    Vec3f second = material.C_E * ( 3 * _D * _r_tr_p1 * no_dot_w12 - (_r_tr_p1 + 3 * _D * (3 *_r_tr_p1 + _r_tr * _r_tr) / (_r * _r) * x_dot_w12) * x_dot_no);
    Vec3f _S = coeff * (first - second);
    return _S;

}

void DipoleCPU::calculate2x2Texture(float inclinationDegreesFromNormal, std::vector<Vec3f> &texture, int textureSize)
{


    float rad = inclinationDegreesFromNormal * M_PI / 180.0f;
    Vec3f wi = Vec3f(sin(rad),cos(rad),0.0f);
    Vec3f xi = Vec3f(0.0f);
    Vec3f ni = Vec3f(0.0f,1.0f,0.0f);
    Vec3f no = Vec3f(0.0f,1.0f,0.0f);

    cout << endl;
    for(int i = 0; i < textureSize; i++)
    {
        float xincm = (((float)i) / (textureSize-1) - 0.5) * 2 * 2.0; //2cm

        for(int j = 0; j < textureSize; j++)
        {
            //int j = 0;
            float yincm = (((float)j) / textureSize - 0.5) * 2 *  2.0; //2cm
            //yincm = 0.0f;

            Vec3f xo = Vec3f(xincm, 0.0f, yincm);
            Vec3f r = S_finite(xi,wi,xo,ni,no);

           // Vec3f test = M_PI * S_finite(xi,wi,Vec3f(-15.995693f,0.0f,0.0038236063f),ni,no);
            Vec3f minR = Vec3f(-3.0f);
            Vec3f maxR =  Vec3f(0.0f);
            texture[i * textureSize + j] = (-minR + Vec3f(log10(r[0]/M_PI),log10(r[1]/M_PI),log10(r[2]/M_PI)))/(maxR-minR);
            //cout << "[" << xincm << "," << yincm  << "] " << r[0] << endl;
            //if(yincm == 0.0f)
            //    cout << r[0] << endl;
        }
    }
}

Vec3f DipoleCPU::S_finite(Vec3f _xi,Vec3f _wi,Vec3f _xo, Vec3f _nin, Vec3f _no)
{

    Vec3f _x = _xo - _xi;
    float r_sqr = dot(_x,_x);

    float eta = 1.0f/material.indexOfRefraction;
    Vec3f _D = material.D;

    float c = dot(_wi, _nin);
    Vec3f _w12 = (eta * (c * _nin - _wi) - _nin * sqrt(1 - eta * eta * (1 - c * c)));

    float mu = -dot(_no, _w12);
    float dot_x_w12 = dot(_x,_w12);
    float dot_x_no = dot(_x,_no);

    Vec3f _r_sqr = Vec3f(r_sqr);
    Vec3f _dr_sqr = Vec3f(r_sqr);
    Vec3f _de = material.de;

    Vec3f ee = Vec3f(1.0f) / (3 * (material.absorption + material.scattering));

    if(mu > EPSILON_MU)
    {
        Vec3f project = sqrtVec3f(Vec3f(r_sqr - dot_x_w12 * dot_x_w12)/sqrtVec3f(_r_sqr + _de*_de));
        Vec3f _D_prime = abs(mu) * _D;
        _dr_sqr += _D_prime * (_D_prime + 2.0f * _de * project);
    }
    else
    {
        Vec3f _D_prime = Vec3f(1.0f) / (3 * (material.absorption + material.scattering));
        _dr_sqr += _D_prime * _D_prime;
    }

    Vec3f _dr = sqrtVec3f(_dr_sqr);
    Vec3f _nistar;

    if(dot(_x, _nin) == 0.0f )
    {
        _nistar = normalize(_nin);
    }
    else
    {
        Vec3f _t = normalize(cross(_nin,_x));
        _nistar = cross(normalize(_x),_t);
    }

    Vec3f _xv = _xi + 2 * material.A * material.de * _nistar;
    Vec3f _dv = Vec3f(length(_xo - _xv));
    Vec3f _wv = _w12 - 2 * dot(_w12,_nistar) * _nistar;

    //for(int i = 0; i < 3 ; i++)
    //{
    //    _dr[i] = std::max(0.1f,_dr[i]);
    //}

    Vec3f _S_r = S_infinite(_xo - _xi, _w12, _dr, _no);
    Vec3f _S_v = S_infinite(_xo - _xv, _wv, _dv, _no);
    Vec3f _S = _S_r - _S_v;

    //std::cout <<  "xdw: " << dot_x_w12 << " S:" << _S[0] << " w12: " << _w12 << endl;
    float ex = 1.0f/(4 * material.C_s_inv) * 1.0f/(4.0f * M_PI * M_PI);
    _S *= ex;

    for(int i = 0; i < 3 ; i++)
    {
        _S[i] = std::max(0.0f,_S[i]);
    }

    return _S;
}

Vec3f DipoleCPU::invertVec3f(const Vec3f vector)
{
    return Vec3f(1.0f) / vector;
}

Vec3f DipoleCPU::sqrtVec3f(const Vec3f vector)
{
    return Vec3f(sqrt(vector[0]),sqrt(vector[1]),sqrt(vector[2]));
}

Vec3f DipoleCPU::powVec3f(const Vec3f vector, const float exp)
{
    return Vec3f(pow(vector[0],exp),pow(vector[1],exp),pow(vector[2],exp));
}
