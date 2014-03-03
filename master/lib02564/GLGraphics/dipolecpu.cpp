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
#ifdef NEWATTEMPT
    Vec3f _r_tr = material.transmissionCoefficient * _r;
    Vec3f _r_tr_p1 = _r_tr + Vec3f(1.0f);
    float x_dot_w12 = dot(_x,_w12);
    float no_dot_w12 = dot(_no,_w12);
    float x_dot_no = dot(_x,_no);
    Vec3f _D = material.D;
    Vec3f _T(exp(-_r_tr[0]),exp(-_r_tr[1]),exp(-_r_tr[2]));
    Vec3f coeff = 1.0f/(4 * material.C_s_inv) * 1.0f/(4.0f * M_PI * M_PI) * _T / powVec3f(_r,3.0f);
    Vec3f first = material.C_s * ( (_r * _r) / _D + 3 * _r_tr_p1 * x_dot_w12);
    Vec3f second = material.C_E * ( 3 * _D * _r_tr_p1 * no_dot_w12 - (_r_tr_p1 + 3 * _D * (3 *_r_tr_p1 + _r_tr * _r_tr) / (_r * _r) * x_dot_w12) * x_dot_no);
    Vec3f _S = coeff * (first - second);
    return _S;

#else
    Vec3f _oneplus = Vec3f(1.0f) + material.transmissionCoefficient * _r;
    Vec3f _D = material.D;
    //dot products
    float dot_x_w12 = dot(_x,_w12);
    float dot_x_no = dot(_x,_no);
    float dot_w12_no = dot(_w12,_no);

    float C_s_inv = material.C_s_inv;
    float C_s = material.C_s;
    float C_E = material.C_E;

    Vec3f _esp = Vec3f(exp(-material.transmissionCoefficient[0] * _r[0]),exp(-material.transmissionCoefficient[1] * _r[1]),exp(-material.transmissionCoefficient[2] * _r[2]));
    Vec3f _initial_coefficients = 0.25 * 0.25 * (1 / (M_PI * M_PI)) * (1 / (C_s_inv)) * _esp * invertVec3f(powVec3f(_r,3));
    Vec3f _D_rev = invertVec3f(_D);

    Vec3f _firstTerm = C_s * (powVec3f(_r,2) * _D_rev + 3 * _oneplus * dot_x_w12);

    Vec3f _intermediate = (3 * _oneplus * invertVec3f(powVec3f(_r,2)) + material.transmissionCoefficient) * 3 * _D * dot_x_w12;
    Vec3f _secondTerm = -C_E  * (3 * _D * _oneplus * dot_w12_no - (_oneplus + _intermediate) * dot_x_no);
    Vec3f _S_D = _initial_coefficients * (_firstTerm + _secondTerm);
    return _S_D;
#endif
}

void DipoleCPU::calculate2x2Texture(float inclinationDegreesFromNormal, std::vector<Vec3f> &texture, int textureSize)
{

    cout << endl;
    for(int i = 0; i < textureSize; i++)
    {
        float xincm = (((float)i) / (textureSize-1) - 0.5) * 2 * 16.0; //2cm
    //    for(int j = 0; j < textureSize; j++)
    //    {
            //float yincm = (((float)j) / textureSize - 0.5) * 4.0; //2cm
            float yincm = 0.0f;
            Vec3f xo = Vec3f(xincm, yincm,0.0f);
            Vec3f xi = Vec3f(0.0f);
            Vec3f ni = Vec3f(0.0f,0.0f,1.0f);
            Vec3f no = Vec3f(0.0f,0.0f,1.0f);

            float rad = inclinationDegreesFromNormal * M_PI / 180.0f;
            Vec3f wi = Vec3f(sin(rad),0.0f,cos(rad));
            wi.normalize();
            if(i == 256)
            {
                cout <<"";
            }
            Vec3f r = S_finite(xi,wi,xo,ni,no);
            float minR = -6.0f;
            float maxR =  0.0f;
            texture[i * textureSize] = r;//(-minR+log10(r))/abs(maxR-minR);
            //cout << "[" << xincm << "," << yincm  << "] " << r[0] << endl;
            cout << r[0] * material.T12 * material.T21 << endl;
     //   }
    }
}

Vec3f DipoleCPU::S_finite(Vec3f _xi,Vec3f _wi,Vec3f _xo, Vec3f _nin, Vec3f _no)
{

    //derivated quantities

    float C_s = material.C_s;
    float C_E = material.C_E;

    Vec3f _x = _xo - _xi;
    float r = length(_x);

    float ni = material.indexOfRefraction;
    Vec3f _D = material.D;

    float c = - dot(_nin, _wi);
    Vec3f _w12 = ni * _wi + (ni * c - sqrt(1 - ni * ni * (1 - c * c))) * _nin;
#ifdef NEWATTEMPT
    float mu = - dot(_no, _w12);
    float dot_x_w12 = dot(_x,_w12);

    Vec3f _r_sqr = Vec3f(r*r);
    Vec3f _dr_sqr = Vec3f(r * r);
    Vec3f _reduced_albedo = material.reducedScatteringCoefficient / material.reducedExtinctionCoefficent;
    Vec3f _de = 2.131 * _D / sqrtVec3f(_reduced_albedo);
    if(mu > 0)
    {
        Vec3f _cosbeta = -sqrtVec3f(Vec3f(r * r - dot_x_w12 * dot_x_w12) / (_r_sqr + _de*_de));
        _dr_sqr += _D * mu * (_D * mu - 2 * _de * _cosbeta);
    }
    else
    {
        _dr_sqr += Vec3f(1.0f) / (3 * (material.absorption + material.scattering));
    }

    Vec3f _dr = sqrtVec3f(_dr_sqr);

    float A = (1.0 - C_E) / (2.0 * C_s);

    Vec3f _t = normalize(cross(_nin,_x));
    Vec3f _nistar = cross(normalize(_x),_t);
    Vec3f _xv = _xi + 2 * A * _de * _nistar;
    Vec3f _dv = Vec3f(length(_xo - _xv));
    Vec3f _wv = _w12 - 2 * dot(_w12,_nistar) * _nistar;

    Vec3f _S_r = S_infinite(_xo - _xi, _w12, _dr, _no);
    Vec3f _S_v = S_infinite(_xo - _xv, _wv, _dv, _no);
    Vec3f _S = _S_r - _S_v;

    for(int i = 0; i < 3 ; i++)
    {
        _S[i] = std::max(0.0f,_S[i]);
    }
    return _S;




#else
    float mu = - dot(_no,_w12);

    Vec3f _reduced_albedo = (material.reducedScatteringCoefficient) / material.reducedExtinctionCoefficent;
    Vec3f _reduced_albedo_inv_sqr = invertVec3f(sqrtVec3f(_reduced_albedo));
    Vec3f _de = 2.131 * _D * _reduced_albedo_inv_sqr;

    float A = (1.0 - C_E) / (2.0 * C_s);


    Vec3f _dr;
    Vec3f _rsquare = Vec3f(r*r);

    if (mu > 0)
    {
        Vec3f _rsqPlusDesqInv = invertVec3f(_rsquare + _de * _de);
        Vec3f cosb = -sqrtVec3f((pow(r,2) - pow(dot(_x,_w12), 2))*_rsqPlusDesqInv);
        _dr = (_rsquare + _D * mu * (_D * mu - 2 * _de * cosb));
    }
    else
    {
        Vec3f _redInv = invertVec3f(3.0f * (material.absorption + material.scattering));
        _dr = _rsquare + _redInv;
    }
    _dr = sqrtVec3f(_dr);

    Vec3f _xnorm = normalize(_x);
    Vec3f _nicross = cross(_nin,_x);
    _nicross.normalize();
    Vec3f _nistar = cross(_xnorm,_nicross);

    Vec3f _xv = _xi + 2 * A * _de * _nistar;
    Vec3f _dv = Vec3f(length(_xo - _xv));
    Vec3f _wv = _w12 - 2 * dot(_w12,_nistar) * _nistar;
    Vec3f s1 = S_infinite(_xo - _xi, _w12, _dr, _no);
    Vec3f s2 = Vec3f(0.0f);
    if(r != 0.0f)
    {
        s2 = S_infinite(_xo - _xv, _wv, _dv, _no);
    }
    Vec3f _S = s1-s2;
    for(int i = 0; i < 3; i++)
        _S[i] = std::max(_S[i], 0.0f);
    return _S;
#endif
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
