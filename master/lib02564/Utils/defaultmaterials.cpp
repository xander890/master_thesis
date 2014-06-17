#include "defaultmaterials.h"
#include "CGLA/Vec3f.h"

using namespace CGLA;

Mesh::ScatteringMaterial* getDefaultMaterial(DefaultScatteringMaterial material)
{
    const float scale = 100.0f;
    Vec3f g = Vec3f(0);
    Vec3f sigma_s = Vec3f(0);
    Vec3f sigma_a = Vec3f(0);
    switch(material)
    {
    case S_Chicken:
     sigma_s = Vec3f(0.15f, 0.21f, 0.38f);
     sigma_a = Vec3f(0.015f, 0.077f, 0.19f);
     g = Vec3f(0.0f, 0.0f, 0.0f);
    break;
    case S_Skin:
     sigma_s = Vec3f(0.74f, 0.88f, 1.01f);
     sigma_a = Vec3f(0.032f, 0.17f, 0.48f);
     g = Vec3f(0.0f, 0.0f, 0.0f);
    break;
    case S_Wholemilk:
     sigma_s = Vec3f(2.55f,3.21f,3.77f);
     sigma_a = Vec3f(0.0011f,0.0024f,0.014f);
     g = Vec3f(0.0f, 0.0f, 0.0f);
    break;
    case S_Whitegrapefruit:
     sigma_s = Vec3f(0.3513f, 0.3669f, 0.5237f);
     sigma_a = Vec3f(0.3609f, 0.3800f, 0.5632f) - sigma_s;
     g = Vec3f(0.548f, 0.545f, 0.565f);
    break;
    case S_Beer:
     sigma_a = Vec3f(0.1449f,0.3141f,0.7286f);
     sigma_s = Vec3f(0.0037f,0.0069f,0.0074f);
     g = Vec3f(0.917f,0.956f,0.982f);
    break;
    case S_Soymilk:
     sigma_a = Vec3f(0.0001f,0.0005f,0.0034f);
     sigma_s = Vec3f(0.2433f,0.2714f,0.4563f);
     g = Vec3f(0.873f,0.858f,0.832f);
    break;
    case S_Coffee:
     sigma_a = Vec3f(0.1669f,0.2287f,0.3078f);
     sigma_s = Vec3f(0.2707f,0.2828f,0.297f);
     g = Vec3f(0.907f,0.896f,0.88f);

    break;
    case S_Marble:
     sigma_a = Vec3f(0.0021f,0.0041f,0.0071f) / scale;
     sigma_s = Vec3f(2.19f,2.62f,3.00f) / scale;
     g = Vec3f(0.0f,0.0f,0.0f);
    break;
    case S_Potato:
     sigma_s = Vec3f(0.68f, 0.70f, 0.55f);
     sigma_a = Vec3f(0.0024f, 0.0090f, 0.12f);
     g = Vec3f(0.0f, 0.0f, 0.0f);
    break;
    case S_Ketchup:
     sigma_a = Vec3f(0.061f,0.97f,1.45f);
     sigma_s = Vec3f(0.18f,0.07f,0.03f);
     g = Vec3f(0.0f,0.0f,0.0f);
    break;
    case S_Apple:
     sigma_a = Vec3f(0.0030f,0.0034f,0.0046f);
     sigma_s = Vec3f(2.29f,2.39f,1.97f);
     g = Vec3f(0.0f,0.0f,0.0f);
    break;
     case S_Chocolate_Milk:
     sigma_s = Vec3f(0.7352f, 0.9142f, 1.0588f);
     sigma_a = Vec3f(0.7359f, 0.9172f, 1.0688f) - sigma_s;
     g = Vec3f(0.862f, 0.838f, 0.806f);
    break;
    }

    return new Mesh::ScatteringMaterial(1.33f,scale * sigma_a, scale * sigma_s, g);
}
