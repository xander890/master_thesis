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
    public:

        // Constuctors/destructor
        Texture():
            id(0),
            target(GL_TEXTURE_2D),
            name("tex"),
            data(std::vector<CGLA::Vec3f>(1)),
            width(1),
            height(1),
            isaImage(false),
            access(GL_READ_ONLY)
        {}

        Texture(const std::string& _name, GLenum target, unsigned int width, unsigned int height, std::vector<CGLA::Vec3f> & data):
            id(0),
            name(_name),
            target(target),
            data(data),
            width(width),
            height(height),
            isaImage(false),
            access(GL_READ_ONLY)
        {}


        /**
         * Use this constructor for a texture generated via a buffer or a fbo, so no data needs to be loaded.
         * So the texture wraps the data that can be sent to shader via a material.
         */
        Texture(const std::string& _name, GLint id, GLenum target):
            id(id),
            name(_name),
            target(target),
            data(std::vector<CGLA::Vec3f>(1)),
            width(1),
            height(1),
            isaImage(false),
            access(GL_READ_ONLY)
        {}


        ~Texture()
        {}

        // get the texture name.
        const std::string& get_name() const {return name;}

        // Initializes the texture wrt OpenGL.
        virtual void init();

        // Reloads data into GPU memory with the new data provided.
        void reloadData(std::vector<CGLA::Vec3f> & data, int newWidth, int newHeight);
        std::vector<CGLA::Vec3f> getData();

        GLuint get_id()
        {
            if(id == 0)
            {
                init();
            }
            return id;
        }

        GLenum get_target()
        {
            return target;
        }

        void set_name(std::string & name)
        {
            this->name = name;
        }

        void setIfImage(bool isaImage)
        {
            this->isaImage = isaImage;
        }

        void setAccess(GLenum access)
        {
            this->access = access;
        }

        GLenum getAccess() {return access;}
        bool isImage() {return isaImage;}

        bool isLayered()
        {
            return target == GL_TEXTURE_CUBE_MAP || target == GL_TEXTURE_2D_ARRAY || target == GL_TEXTURE_3D;
        }

        unsigned int width;
        unsigned int height;

    protected:
        void load();
        GLuint id;
        GLenum target;
        std::string name;
        std::vector<CGLA::Vec3f> data;


        bool isaImage;
        GLenum access;
    };

}

#endif // TEXTURE_H
