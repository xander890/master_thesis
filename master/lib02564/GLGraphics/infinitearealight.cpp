#include "infinitearealight.h"
#include "QColor"

#include <Utils/miscellaneous.h>

using namespace CGLA;
InfiniteAreaLight::InfiniteAreaLight(QImage &source)
{
    width = source.width();
    height = source.height();
    float *img = new float[width*height]; //luminance map

    int nu = width; int nv = height;
    for (int u = 0; u < nu; ++u) {
        for (int v = 0; v < nv; ++v) {
            QColor color = QColor::fromRgb(source.pixel(u,v));
            img[v+u*nv] = (0.2126 * color.redF() + 0.7152 * color.greenF() + 0.0722 * color.blueF());
        }
    }

    float *func = (float *)alloca(std::max(nu, nv) *sizeof(float));
    float *sinVals = (float *)alloca(nv * sizeof(float));
    for (int i = 0; i < nv; ++i)
        sinVals[i] = sin(M_PI * float(i+.5)/float(nv));
    vDistribs = new Distribution1D *[nu];
    for (int u = 0; u < nu; ++u) {
        for (int v = 0; v < nv; ++v)
            func[v] = img[u*nv+v] * sinVals[v];
        vDistribs[u] = new Distribution1D(func, nv);
    }

    for (int u = 0; u < nu; ++u)
        func[u] = vDistribs[u]->funcInt;
    uDistrib = new Distribution1D(func, nu);
}

void InfiniteAreaLight::getLights(QImage & source, std::vector<GLGraphics::Light *> & lights, unsigned int number)
{
    gel_srand(10);

    gel_rand();
    gel_rand();

    for(int i = 0; i < number; i++)
    {
        float u1 = gel_rand() / float(GEL_RAND_MAX);
        float u2 = gel_rand() / float(GEL_RAND_MAX);
        Vec2i coord;

        Vec3f dir;
        Sample_L(u1,u2,coord,dir);
        QColor color = QColor::fromRgb(source.pixel(coord[0],coord[1]));
        Vec3f color_ref = Vec3f(color.redF(),color.greenF(),color.blueF());


        std::cout << coord << std::endl;
        lights.push_back(new GLGraphics::Light(Vec4f(-dir,0),Vec4f(color_ref,0), 5.0f, Vec4f(0), true));
    }
}

void InfiniteAreaLight::Sample_L(float u1,float u2, Vec2i & coord, Vec3f & dir)
{
    float pdfs[2];
    float fu = uDistrib->Sample(u1, &pdfs[0]);
    int u = clamp((int)(fu), 0, uDistrib->count-1);
    float fv = vDistribs[u]->Sample(u2, &pdfs[1]);
    int v = clamp((int)(fv), 0, uDistrib->count-1);

    coord = Vec2i(u,v);

    float theta = fv * vDistribs[u]->invCount * M_PI;
    float phi = fu * uDistrib->invCount * 2.f * M_PI;
    float costheta = cos(theta), sintheta = sin(theta);
    float sinphi = sin(phi);
    float cosphi = cos(phi);
    dir = Vec3f(sintheta * cosphi, sintheta * sinphi,costheta);
}
