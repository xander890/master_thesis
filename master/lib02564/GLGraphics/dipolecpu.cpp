#include "dipolecpu.h"
#include <algorithm>

using namespace CGLA;
using namespace std;
#define NEWATTEMPT

void DipoleCPU::calculate(std::vector<CGLA::Vec3f> &vertices, std::vector<CGLA::Vec3f> &normals, std::vector<CGLA::Vec4f> &resultColors)
{
    std::cout << "Started computation of dipole. "<< vertices.size() << " vertices found." << std::endl;

    // Assuming the points are equally distributed.
    for(int i = 0; i < vertices.size(); i++)
    {
        Vec3f xo = vertices[i];
        Vec3f no = normalize(normals[i]);
        resultColors[i] = Vec4f(0.0f);
        for(int j = 0; j < vertices.size(); j++)
        {
            //std::cout << "Calculating dipole..." << j << std::endl;
            if(i == j)
            {
                break;
            }
            Vec3f xi = vertices[j];
            Vec3f ni = normalize(normals[j]);
            Vec3f wi = normalize(Vec3f(light.position) - xi);
            Vec3f S = S_finite(xi,wi,xo,ni,no);
            Vec3f Phi = Vec3f(light.diffuseIntensity[0]);
            Vec3f R = S * M_PI;
            resultColors[i] += Vec4f(R,0.0f);
        }
        //if(resultColors[i][3] != 0.0f)
        //    resultColors[i] /= resultColors[i][3];
        resultColors[i][3] = 1.0f;
        std::cout << "Calculating dipole... (" << (100*(float)i) / vertices.size() << "\%) " << resultColors[i] <<std::endl;
    }
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
            Vec3f test = S_finite(xi,wi,Vec3f(0.001,0.0f,0.0f),ni,no);

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

    float eta = material.indexOfRefraction;
    Vec3f _D = material.D;

    float c = - dot(_nin, _wi);
    Vec3f _w12 = eta * _wi + (eta * c - sqrt(1 - eta * eta * (1 - c * c))) * _nin;

    float mu = - dot(_no, _w12);
    float dot_x_w12 = dot(_x,_w12);
    float dot_x_no = dot(_x,_no);

    Vec3f _r_sqr = Vec3f(r_sqr);
    Vec3f _dr_sqr = Vec3f(r_sqr);
    Vec3f _de = material.de;

    if(mu > 0)
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

    if(dot_x_no == 0.0f)
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

    Vec3f _S_r = S_infinite(_xo - _xi, _w12, _dr, _no);
    Vec3f _S_v = S_infinite(_xo - _xv, _wv, _dv, _no);
    Vec3f _S = _S_r - _S_v;
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
