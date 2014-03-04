#ifndef THREEDSPHERE_H
#define THREEDSPHERE_H
#include "GLGraphics/ThreeDObject.h"
#include "GLGraphics/light.h"
#include "CGLA/Vec4f.h"

namespace GLGraphics
{
    class ThreeDSphere : public ThreeDObject
    {
    public:

        ThreeDSphere(Mesh::Material mat, int LOD) : material(mat), LOD(LOD), ThreeDObject()
        {
        }
        virtual void display(ShaderProgramDraw& shader) override;
        virtual bool init(std::string filename, std::string name) override;

    private:
        Mesh::Material material;
        int LOD;
    };

}
#endif // THREEDSPHERE_H
