#ifndef THREEDWIRECUBE_H
#define THREEDWIRECUBE_H

#include "GLGraphics/ThreeDObject.h"
#include "GLGraphics/light.h"
#include "CGLA/Vec4f.h"
#include "Mesh/TriangleMesh.h"

namespace GLGraphics
{
    class ThreeDWireCube : public ThreeDObject
    {
    public:

        ThreeDWireCube() : ThreeDObject(), color(CGLA::Vec4f(1.0f))
        {
        }
        virtual void display(ShaderProgramDraw& shader) override;
        virtual bool init(std::string filename, std::string name, Mesh::Material & material) override;

        void setColor(CGLA::Vec4f & color) {this->color = color;}

        void setBounds(Mesh::BoundingBox & box);
        void setBounds(float xlow, float xhigh, float ylow, float yhigh, float zlow, float zhigh);

    private:
        CGLA::Vec4f color;

    };

}

#endif // THREEDWIRECUBE_H
