#include "lightmanager.h"

using namespace std;
using namespace CGLA;

namespace GLGraphics
{
    void LightManager::addLight(Light& l)
    {
        if(lights.size() > MAX_LIGHTS)
        {
            cout << "Too many lights already: not adding new one." << endl;
            return;
        }
        lights.push_back(l);
        isDirty = true;
        lightsize++;
    }

    void LightManager::clearLights()
    {
        lights.clear();
    }

    void LightManager::loadLights(ShaderProgramDraw & shader)
    {
        if(isDirty)
        {
            reloadLights();

        }

        /*
        for(int i = 0; i < positions.size(); i++)
        {
            Vec4f nepos = lights[i].position;
            positions[i] = shader.get_view_matrix() * (nepos);
        }
        */
        shader.set_uniform(LIGHT_POS_UNIFORM, positions, lightsize);
        shader.set_uniform(LIGHT_DIFFUSE_COLOR_UNIFORM, diffuse, lightsize);
        shader.set_uniform(LIGHT_SPEC_COLOR_UNIFORM, specular, lightsize);
        shader.set_uniform("light_number", lightsize);

        check_gl_error();
    }

    void LightManager::reloadLights()
    {
        diffuse.clear();
        specular.clear();
        positions.clear();
        for(vector<Light>::const_iterator i = lights.begin(); i != lights.end(); ++i) {
            Light l = *i;

            diffuse.push_back(l.diffuseColor * l.intensity);
            specular.push_back(l.specularIntensity);
            Vec4f pos(l.position[0], l.position[1], l.position[2], l.isDirectional? 0.0f : 1.0f);
            positions.push_back(pos);
        }
        isDirty = false;
    }
}
