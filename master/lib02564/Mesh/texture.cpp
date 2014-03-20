#include "texture.h"

using namespace std;

void Mesh::Texture::init()
{
    if(id != 0)
    {
        cerr << "Texture " << name << " initialized twice. Not initializing again." << endl;
        return;
    }
    if(name.compare("vertices") == 0)
    {
        cerr << "H!" << endl;
    }

    glGenTextures(1,&id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F , width, height,0, GL_RGB,GL_FLOAT, &data[0]);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

}
