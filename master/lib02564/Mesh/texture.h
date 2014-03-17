#ifndef TEXTURE_H
#define TEXTURE_H

#include <GLGraphics/GLHeader.h>
#include <string>
#include <CGLA/Vec3f.h>


namespace Mesh
{
    /// A simple texture map class.
    class Texture
    {
        std::vector<CGLA::Vec3f> data;
        GLuint id;
        std::string name;
        unsigned int width;
        unsigned int height;

    public:

        // Constuctor/destructor
        Texture(): id(0), name("tex"), data(std::vector<CGLA::Vec3f>(1)), width(1), height(1) {}
        Texture(const std::string& _name, unsigned int width, unsigned int height, const std::vector<CGLA::Vec3f> & data):
            id(0),
            name(_name),
            data(data),
            width(width),
            height(height)
        {}

        ~Texture()
        {}

        /// get the texture name.
        const std::string& get_name() const {return name;}

        /// Initializes the texture wrt OpenGL.
        void init();

        GLuint get_id()
        {
            if(id == 0)
            {
                init();
            }
            return id;
        }
    };

}

#endif // TEXTURE_H
