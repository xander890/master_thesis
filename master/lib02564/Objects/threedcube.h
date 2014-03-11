#ifndef ThreeDCube_H
#define ThreeDCube_H
#include "GLGraphics/ThreeDObject.h"
#include "GLGraphics/light.h"
#include "CGLA/Vec4f.h"

namespace GLGraphics
{
    class ThreeDCube : public ThreeDObject
    {
    public:

        ThreeDCube(int LOD) : ThreeDObject(), LOD(LOD)
        {
        }
        virtual void display(ShaderProgramDraw& shader) override;
        virtual bool init(std::string filename, std::string name, Mesh::Material & material) override;


    private:
        int LOD;
        void generateFace(std::vector<CGLA::Vec3f> & vertices, std::vector<CGLA::Vec3f> & normals, std::vector<CGLA::Vec2f> & texcoord, std::vector<GLuint> & indices);

    };

}
#endif // ThreeDCube_H
