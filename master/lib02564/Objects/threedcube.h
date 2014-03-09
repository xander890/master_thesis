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

        ThreeDCube(Mesh::Material mat, int LOD) : material(mat), LOD(LOD), ThreeDObject()
        {
        }
        virtual void display(ShaderProgramDraw& shader) override;
        virtual bool init(std::string filename, std::string name) override;

    private:
        Mesh::Material material;
        int LOD;
        void generateFace(std::vector<CGLA::Vec3f> & vertices, std::vector<CGLA::Vec3f> & normals, std::vector<CGLA::Vec2f> & texcoord, std::vector<GLuint> & indices);

    };

}
#endif // ThreeDCube_H
