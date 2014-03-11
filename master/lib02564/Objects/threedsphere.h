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

        ThreeDSphere(int LOD) : LOD(LOD), ThreeDObject()
        {
        }
        virtual void display(ShaderProgramDraw& shader) override;
        virtual bool init(std::string filename, std::string name, Mesh::Material & material) override;

    private:
        int LOD;
    };

}
#endif // THREEDSPHERE_H
