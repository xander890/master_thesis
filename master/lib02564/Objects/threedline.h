#ifndef THREEDLINE_H
#define THREEDLINE_H
#include "GLGraphics/ThreeDObject.h"
#include "GLGraphics/light.h"
#include "CGLA/Vec4f.h"

namespace GLGraphics
{
    class ThreeDLine : public ThreeDObject
    {
    public:

        ThreeDLine() : ThreeDObject(), color(CGLA::Vec4f(1.0f))
        {
        }
        virtual void display(ShaderProgramDraw& shader) override;
        virtual bool init(std::string filename, std::string name, Mesh::Material & material) override;

        void setColor(CGLA::Vec4f & color) {this->color = color;}

    private:
        CGLA::Vec4f color;

    };

}

#endif // THREEDLINE_H
