#include "proceduralsphere.h"
#include "GLGraphics/ShaderProgram.h"
using namespace CGLA;
using namespace std;


namespace GLGraphics
{

    void sphere(float r, int lat, int lon, std::vector<CGLA::Vec3f>& strip)
    {
        float lat_rad = (M_PI) / lat;
        float lon_rad = (2 * M_PI) / lon;

        for(int i = 1; i <= lat; i++)
        {
            float theta = i * lat_rad;
            float theta1 = (i-1) * lat_rad;

            for(int j = 0; j <= lon; j++)
            {

                float phi = j * lon_rad;
                Vec3f p2 = r * Vec3f(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
                Vec3f p1 = r * Vec3f(sin(theta1) * cos(phi), sin(theta1) * sin(phi), cos(theta1));
                cout << p1 << endl;
                if(i == 0 && j == 0)
                {
                    strip.push_back(p1);
                    strip.push_back(p1);
                }

                strip.push_back(p1);
                strip.push_back(p1);
                strip.push_back(p2);
                strip.push_back(p2);

                if(i == lat && j == lon)
                {
                    strip.push_back(p2);
                    strip.push_back(p2);
                }

            }
        }



    }

    GLuint create_sphere_vertex_array_object(vector<Vec3f>& strip)
    {
        GLuint VAO, VBO;
        GLuint vert_attrib = ShaderProgramDraw::get_generic_attrib_location("vertex");
        GLuint norm_attrib = ShaderProgramDraw::get_generic_attrib_location("normal");
        GLuint texcoord_attrib = ShaderProgramDraw::get_generic_attrib_location("texcoord");
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER,strip.size()*sizeof(Vec3f),&strip[0],GL_STATIC_DRAW);
        glVertexAttribPointer(norm_attrib, 3, GL_FLOAT, GL_FALSE, 2*sizeof(Vec3f), 0);
        glVertexAttribPointer(vert_attrib, 3, GL_FLOAT, GL_FALSE, 2*sizeof(Vec3f), (const GLvoid*)sizeof(Vec3f));
        glVertexAttrib3f(texcoord_attrib, 0, 0,0);
        glEnableVertexAttribArray(vert_attrib);
        glEnableVertexAttribArray(norm_attrib);
        return VAO;
    }
}
