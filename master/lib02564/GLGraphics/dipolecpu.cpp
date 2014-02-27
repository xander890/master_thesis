#include "dipolecpu.h"
#include <algorithm>

using namespace CGLA;
using namespace std;

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
            float r = length(xo-xi);
            if( r == 0 || r > 2.0f)
            {
                break;
            }
            Vec3f ni = normalize(normals[j]);
            Vec3f wi = normalize(Vec3f(light.position) - xi);
            Vec3f S = S_finite(xi,wi,xo,ni,no);
            Vec3f Phi = Vec3f(light.diffuseIntensity[0]) * dot(wi, ni);
            Vec3f L = 4 * material.C_s_inv * S * Phi;
            resultColors[i] += Vec4f(L,1.0f);
        }
        if(resultColors[i][3] != 0.0f)
            resultColors[i] /= resultColors[i][3];
        //resultColors[i][3] = 1.0f;
        std::cout << "Calculating dipole... (" << (100*(float)i) / vertices.size() << "\%) " << resultColors[i] <<std::endl;
    }
}

Vec3f DipoleCPU::S_infinite(Vec3f _x, Vec3f _w, float r, Vec3f _no)
{

    if( r == 0)
    {
        return Vec3f(0.0f);
    }
    Vec3f _oneplus = Vec3f(1.0f) + material.transmissionCoefficient * r;
    Vec3f _D = material.D;
    //dot products
    float dot_x_w = dot(_x,_w);
    float dot_x_n = dot(_x,_no);
    float dot_w_n = dot(_w,_no);

    float C_s_inv = material.C_s_inv;
    float C_s = material.C_s;
    float C_E = material.C_E;

    Vec3f _esp = Vec3f(exp(-material.transmissionCoefficient[0] * r),exp(-material.transmissionCoefficient[1] * r),exp(-material.transmissionCoefficient[2] * r));
    Vec3f _initial_coefficients = 0.25 * 0.25 * (1 / (M_PI * M_PI)) * (1 / (C_s_inv)) * _esp / pow(r,3);
    Vec3f _D_rev = Vec3f(1.0f/_D[0],1.0f/_D[1],1.0f/_D[2]);

    Vec3f _firstTerm = C_s * (pow(r,2) * _D_rev + 3 * _oneplus * dot_x_w);

    Vec3f _intermediate = (3 * _oneplus / pow(r,2) + material.transmissionCoefficient) * 3 * _D * dot_x_w;
    Vec3f _secondTerm = -C_E  * (3 * _D * _oneplus * dot_w_n - (_oneplus + _intermediate) * dot_x_n);
    return _initial_coefficients * (_firstTerm + _secondTerm);
}

void DipoleCPU::calculate2x2Texture(float inclinationDegreesFromNormal, std::vector<float> &texture, int textureSize)
{

    for(int i = 0; i < textureSize; i++)
    {
        float xincm = (((float)i) / textureSize - 0.5) * 4.0; //2cm
        for(int j = 0; j < textureSize; j++)
        {
            float yincm = (((float)j) / textureSize - 0.5) * 4.0; //2cm
            Vec3f xo = Vec3f(xincm, yincm,0.0f);
            Vec3f xi = Vec3f(0.0f);
            Vec3f ni = Vec3f(0.0f,0.0f,1.0f);
            Vec3f no = Vec3f(0.0f,0.0f,1.0f);
            float rad = inclinationDegreesFromNormal * M_PI / 180.0f;
            Vec3f wi = Vec3f(sin(rad),0.0f,cos(rad));
            float r = S_finite(xi,wi,xo,ni,no)[0];
            texture[i * textureSize + j] = r;
            cout << "" << r << endl;
        }
    }
}

Vec3f DipoleCPU::S_finite(Vec3f _xi,Vec3f _wi,Vec3f _xo, Vec3f _nin, Vec3f _no)
{

    //derivated quantities

    float C_s = material.C_s;
    float C_E = material.C_E;

    Vec3f _x = _xo - _xi;
    float r = length(_x);
    if(r == 0.0)
    {
        return Vec3f(0.0);
    }
    float ni = material.indexOfRefraction;
    Vec3f _D = material.D;

    float c = - dot(_nin, _wi);
    Vec3f _w12 = ni * _wi + (ni * c - sqrt(1 - ni * ni * (1 - c * c))) * _nin;
    float mu = - dot(_no,_w12);

    Vec3f _reduced_albedo = (material.reducedScatteringCoefficient) / material.reducedExtinctionCoefficent;
    Vec3f _reduced_albedo_inv_sqr = invertVec3f(sqrtVec3f(_reduced_albedo));
    Vec3f _de = 2.131 * _D * _reduced_albedo_inv_sqr;
    float A = (1 - C_E) / (2 * C_s);


    float dr;
    Vec3f _rsquare = Vec3f(r*r);

    if (mu > 0)
    {
        Vec3f _rsqPlusDesqInv = invertVec3f(_rsquare + _de * _de);
        dr = (_rsquare + _D * mu * (_D * mu + 2 * _de * sqrtVec3f((pow(r,2) - pow(dot(_x,_w12), 2))*_rsqPlusDesqInv)))[0];
    }
    else
    {
        Vec3f _redInv = invertVec3f(material.reducedExtinctionCoefficent);
        dr = (_rsquare + 1 / 9 * _redInv * _redInv)[0];
    }
    dr = sqrt(dr);

    Vec3f _xnorm = _x / r;
    Vec3f _nicross = cross(_nin,_x);
    _nicross /= length(_nicross);
    Vec3f _nistar = cross(_xnorm,_nicross);
    Vec3f _xv = _xi + 2 * A * _de * _nistar;
    float dv = length(_xo - _xv);
    Vec3f _wv = _w12 - 2 * dot(_w12,_nistar) * _nistar;
    Vec3f _S = S_infinite(_xo - _xi, _w12, dr, _no) - S_infinite(_xo - _xv, _wv, dv, _no);
    for(int i = 0; i < 3; i++)
        _S[i] = std::max(_S[i], 0.0f);
    return _S;
}

Vec3f DipoleCPU::invertVec3f(const Vec3f vector)
{
    return Vec3f(1.0f/vector[0],1.0f/vector[1],1.0f/vector[2]);
}

Vec3f DipoleCPU::sqrtVec3f(const Vec3f vector)
{
    return Vec3f(sqrt(vector[0]),sqrt(vector[1]),sqrt(vector[2]));
}
