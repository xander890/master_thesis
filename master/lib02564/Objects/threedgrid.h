#ifndef THREEDGRID_H
#define THREEDGRID_H

#include "GLGraphics/ThreeDObject.h"
#include "GLGraphics/light.h"
#include "CGLA/Vec4f.h"

namespace GLGraphics
{
    class ThreeDGrid : public ThreeDObject
    {
    public:

        ThreeDGrid(float step, int size) : ThreeDObject(), color(CGLA::Vec4f(1.0f)), step(step), size(size)
        {
        }
        virtual void display(ShaderProgramDraw& shader) override;
        virtual bool init(std::string filename, std::string name, Mesh::Material & material) override;

        void setColor(CGLA::Vec4f & color) {this->color = color;}

    private:
        CGLA::Vec4f color;
        float step;
        int size;

    };

}
#endif // THREEDGRID_H
