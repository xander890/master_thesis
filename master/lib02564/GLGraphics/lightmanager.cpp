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
        size++;
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

        for(int i = 0; i < positions.size(); i++)
        {
            Vec4f nepos = lights[i].position;
            float z = nepos[2];
            float y = nepos[1];
            nepos[2] = y;
            nepos[1] = -z;
            positions[i] = shader.get_view_matrix() * (shader.get_model_matrix() * (nepos));
        }

        shader.set_uniform(LIGHT_POS_UNIFORM, positions, size);
        shader.set_uniform(LIGHT_DIFFUSE_COLOR_UNIFORM, diffuse, size);
        shader.set_uniform(LIGHT_SPEC_COLOR_UNIFORM, specular, size);

        check_gl_error();
    }

    void LightManager::reloadLights()
    {
        diffuse.clear();
        specular.clear();
        positions.clear();
        cout << "dirty";
        for(vector<Light>::const_iterator i = lights.begin(); i != lights.end(); ++i) {
            Light l = *i;

            diffuse.push_back(l.diffuseIntensity);
            specular.push_back(l.specularIntensity);
            Vec4f pos(l.position[0], l.position[1], l.position[2], l.isDirectional? 0.0f : 1.0f);
            positions.push_back(pos);
        }
        isDirty = false;
    }
}
