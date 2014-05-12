#ifndef LIGHTMANAGER_H
#define LIGHTMANAGER_H
#include "light.h"
#include "ShaderProgram.h"

namespace GLGraphics
{
    class LightManager
    {
    public:
        LightManager() : lights(std::vector<Light>()),
            isDirty(true),
            diffuse(std::vector<CGLA::Vec4f>()),
            specular(std::vector<CGLA::Vec4f>()),
            positions(std::vector<CGLA::Vec4f>()),
            lightsize(0)
        {}

        void addLight(Light& l);
        void clearLights();
        static const int MAX_LIGHTS = 50;
        void loadLights(ShaderProgramDraw & shader);
        void reloadLights();
        int size() {return lightsize;}
        Light& operator[] (int x) {return lights[x];}

    private:
        std::vector<Light> lights;

        bool isDirty;
        int lightsize;

        std::vector<CGLA::Vec4f> diffuse;
        std::vector<CGLA::Vec4f> specular;
        std::vector<CGLA::Vec4f> positions;
    };
}
#endif // LIGHTMANAGER_H
