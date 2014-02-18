/*
 *  terrain.cpp
 *  02564_Framework
 *
 *  Created by J. Andreas Bærentzen on 02/02/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include <vector>

#include <GLGraphics/GLHeader.h>

#include "auxil.h"
#include "terrain.h"

#define USE_VERTEX_ARRAY 1
#define SOLUTION_CODE

using namespace CGLA;
using namespace GLGraphics;
using namespace std;

float Terrain::height(float _x, float _y) const
{
    float x = delta*_x+x0;
    float y = delta*_y+y0;
    return A*turbulence(x,y);
}

Vec3f Terrain::normal(float x, float y) const 
{
    Vec3f xvec(2,0,(height(x+1,y)-height(x-1,y))/2.0);
    Vec3f yvec(0,2,(height(x,y+1)-height(x,y-1))/2.0);
    return normalize(cross(xvec, yvec));
}


void Terrain::draw(ShaderProgramDraw& shader_prog)
{
    shader_prog.set_model_matrix(identity_Mat4x4f());

#ifdef SOLUTION_CODE
	static bool was_here_tex = false;
	static GLuint noise_tex = 0;
	if(!was_here_tex)
	{
		int MMW = 256;
		int MMH = 256;
		glGenTextures(1, &noise_tex);
		glBindTexture(GL_TEXTURE_2D, noise_tex);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        vector<unsigned char> noise_vec(MMW*MMH);
		gel_srand(0);
		for(int i=0;i<MMW*MMH;++i)
            noise_vec[i] = gel_rand() % UCHAR_MAX;
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, MMW, MMH, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, &noise_vec[0]);
        glGenerateMipmap(GL_TEXTURE_2D);
        was_here_tex = true;
	}
    
    shader_prog.use_texture(GL_TEXTURE_2D, "noise_tex", noise_tex);
#endif

    static bool bufferInitialized = false;
	static GLuint buffers[2];
    static int NUM_VERTICES=-1;
    if(!bufferInitialized)
	{
        bufferInitialized = true;
		glGenBuffers(2, buffers);
		
		vector<Vec3f> vertices;
		vector<Vec3f> normals;
		
		for(int j=-30;j<30;++j)
		{
			normals.push_back(normal(-30,j+1));
			vertices.push_back(Vec3f(-30,j+1,height(-30,j+1)));
			for(int i=-30;i<30;++i)
			{			
				normals.push_back(normal(i,j+1));
				vertices.push_back(Vec3f(i,j+1,height(i,j+1)));
				normals.push_back(normal(i,j));
				vertices.push_back(Vec3f(i,j,height(i,j)));			
			}
			normals.push_back(normal(29,j));
			vertices.push_back(Vec3f(29,j,height(29,j)));
		}
        glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
        glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(Vec3f), &vertices[0], GL_STATIC_DRAW);

		
        glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
        glBufferData(GL_ARRAY_BUFFER, normals.size()*sizeof(Vec3f), &normals[0], GL_STATIC_DRAW);


        NUM_VERTICES = vertices.size();
    }
    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    GLuint vertex_attrib = shader_prog.get_attrib_location("vertex");
    glEnableVertexAttribArray(vertex_attrib);
    glVertexAttribPointer(vertex_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    GLuint normal_attrib = shader_prog.get_attrib_location("normal");
    glEnableVertexAttribArray(normal_attrib);
    glVertexAttribPointer(normal_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, NUM_VERTICES);
}
