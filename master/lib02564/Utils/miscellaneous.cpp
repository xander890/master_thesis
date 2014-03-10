#include "miscellaneous.h"
using namespace CGLA;

void duplicateMap(const std::vector<CGLA::Vec3f> &originalVector, std::map<GLuint, std::vector<GLuint> > &duplicates, float tolerance)
{
    for(int i = 0; i < originalVector.size(); i++)
    {
        for(int j = i + 1; j < originalVector.size(); j++)
        {
            if(i != j && length(originalVector[i] - originalVector[j]) < tolerance)
            {
                duplicates[i].push_back(j);
                duplicates[j].push_back(i);
            }
        }
    }
}



CGLA::Vec3f refract(const CGLA::Vec3f &in, const CGLA::Vec3f & n, float n1, float n2)
{
    float eta = n1/n2;
    float c = dot(in, n);
    return eta * (c * n - in) - n * sqrt(1 - eta * eta * (1 - c * c));
}


Vec2f fresnelAmplitudeReflectance(const CGLA::Vec3f &in, const CGLA::Vec3f &n, float nin, float ntr)
{
    float cosin = dot(in,n);
    Vec3f refr = refract(in,n,nin,ntr);
    float costr = dot(refr,-n);

    float r_s = (nin * cosin - ntr * costr) / (nin * cosin + ntr * costr);
    float r_p = (nin * costr - ntr * cosin) / (nin * costr + ntr * cosin);
    //std::cout << abs(r_s) << " ¤ " << abs(r_p) << std::endl;
    return Vec2f(r_s,r_p);
}


Vec2f fresnelPowerReflectance(const Vec3f &in, const Vec3f &n, float n1, float n2)
{
    Vec2f r = fresnelAmplitudeReflectance(in,n,n1,n2);
    return r * r;
}


Vec2f fresnelAmplitudeTransmittance(const Vec3f &in, const Vec3f &n, float n1, float n2)
{
    float cosin = dot(in,n);
    Vec3f refr = refract(in,n,n1,n2);
    float costr = dot(refr,-n);
    //std::cout << n1 *  sqrt(1 - cosin * cosin) << " = " << n2 * sqrt(1 - costr * costr) << std::endl;
    float t_s = (2 * n1 * cosin) / (n1 * cosin + n2 * costr);
    float t_p = (2 * n1 * cosin) / (n1 * costr + n2 * cosin);
    //std::cout << abs(t_s) << " # " << abs(t_p) << std::endl;
    return Vec2f(t_s,t_p);
}


Vec2f fresnelPowerTransmittance(const Vec3f &in, const Vec3f &n, float n1, float n2)
{
    float cosin = dot(in,n);
    Vec3f refr = refract(in,n,n1,n2);
    float costr = dot(refr,-n);

    Vec2f t = fresnelAmplitudeTransmittance(in,n,n1,n2);

    return ((n2 * costr) / (n1 * cosin)) * (t * t);
}


void debugFresnelTerms()
{
    int steps = 50;
    for(int i = 0; i < steps; i++)
    {
        Vec3f normal = Vec3f(0,0,1);
        float angle = (i / ((float)steps - 1)) * M_PI / 2;
        Vec3f incident = Vec3f(sin(angle), 0 , cos(angle));
        float n1 = 1.0f;
        float n2 = 1.5f;

        float T = fresnel_T(incident,normal,n1,n2);

        float R = fresnel_R(incident,normal,n1,n2);
        Vec2f T1 = fresnelPowerReflectance(incident,normal,n1,n2);
        Vec2f R1 = fresnelPowerTransmittance(incident,normal,n1,n2);
        Vec2f c (T,R);
        std::cout << "*Fresnel test: n1 = " << n1 << ", n2 = " << n2 << ", dot = " << dot(incident,normal) << " (t,T,r,R) = " << c << ", R+T = " << (T+R) << std::endl;
    }
}


























