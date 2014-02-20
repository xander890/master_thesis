#ifndef __TEXMAP_H__
#define __TEXMAP_H__

#include <GLGraphics/GLHeader.h>
#include <string>
#include <CGLA/Vec3f.h>
#include <QImage>
#include <QDebug>

namespace Mesh
{
    /// A simple texture map class.
	class Texmap 
	{
        QImage qgl_data;
		GLuint id;
		std::string name;
        
	public:
        
		// Constuctor/destructor
        Texmap():id(0), name("") {}
        Texmap(const std::string& _name): id(0), name("") {
            bool loadSucces = load(_name);
            if (!loadSucces){
                qDebug() << "Unable to load "<<_name.c_str();
            }
        }
        ~Texmap() {}
        
		/// get the texture name.
		const std::string& get_name() const {return name;}
        
		/// Load with a given file name
		bool load(const std::string& _name);
                
		/// Initializes the texture wrt OpenGL.
		void gl_init();

        GLuint get_id() {if(!id) gl_init(); return id;}
	};
    
}
#endif



