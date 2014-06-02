#include "defaultmaterials.h"
#include "CGLA/Vec3f.h"

using namespace CGLA;

Mesh::ScatteringMaterial* getDefaultMaterial(DefaultScatteringMaterial material)
{
    switch(material)
    {
        case S_Chocolate_Milk:
            return new Mesh::ScatteringMaterial(1.3f,Vec3f(0.032,0.17,0.48),Vec3f(0.74,0.88,1.01), Vec3f(0.0f));
        break;
        case S_Potato:
            return new Mesh::ScatteringMaterial(1.3f, 100 * Vec3f(0.0024,0.0090,0.12), 100 * Vec3f(0.68,0.70,0.55), Vec3f(0.0f));
        break;
        default:
        case S_Marble:
            return new Mesh::ScatteringMaterial(1.3f, Vec3f(0.0021f,0.0041f,0.0071f), Vec3f(2.19f,2.62f,3.0f), Vec3f(0.0f));
        break;
        case S_Soy_Milk_regular:
            Vec3f ext =  Vec3f(0.2434f,0.2719f,0.4597f);
            Vec3f scattering =  Vec3f(0.2433f,0.2714f,0.4563f);
            Vec3f g(0.873f,0.858f,0.832f);
            return new Mesh::ScatteringMaterial(1.33f,Vec3f(0.0f), scattering, g);
    }
}
