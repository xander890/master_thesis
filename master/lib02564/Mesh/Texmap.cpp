#include <GLGraphics/GLHeader.h>
#include <QDebug>
#include <QPixmap>

#include <iostream>
#include <CGLA/Vec2i.h>
#include <CGLA/Vec2f.h>
#include <Mesh/Texmap.h>

using namespace std;
using namespace CGLA;

namespace Mesh
{    
    
	bool Texmap::load(const std::string& _name)
	{
        name = _name;
        QImage * image = new QImage();
        if(image->load(QString(name.c_str()))) {
            qgl_data = QGLWidget::convertToGLFormat(*image);
            return true;
        }
        return false;
    }
		
    
    void Texmap::gl_init()
    {
        glGenTextures(1,&id);
        glBindTexture(GL_TEXTURE_2D, id);
        if(qgl_data.isNull())
        {
            GLubyte dummy[] = {255,255,255,255};
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1,0,GL_RGBA,GL_UNSIGNED_BYTE,dummy);
        }
        else
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, qgl_data.width(), qgl_data.height(),0,GL_RGBA,GL_UNSIGNED_BYTE,qgl_data.bits());
        }
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    }
    
}	
