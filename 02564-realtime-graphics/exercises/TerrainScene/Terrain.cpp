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

#define SOLUTION_CODE

#include "auxil.h"
#include "Terrain.h"

#define USE_VERTEX_ARRAY 1

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
	static bool was_here_tex = false;
	static GLuint noise_tex = 0;
	if(!was_here_tex)
    {
#ifdef SOLUTION_CODE
        int MMW = 256;
        int MMH = 256;
        glGenTextures(1, &noise_tex);
        glBindTexture(GL_TEXTURE_2D, noise_tex);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        vector<float> noise_vec(MMW*MMH);
        gel_srand(0);
        for(int i=0;i<MMW*MMH;++i)
            noise_vec[i] =  gel_rand()/static_cast<float>(GEL_RAND_MAX);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, MMW, MMH, 0, GL_RED, GL_FLOAT, &noise_vec[0]);
        glGenerateMipmap(GL_TEXTURE_2D);
#else
        // Create texture at this point
#endif
		was_here_tex = true;
	}
    
    shader_prog.use_texture(GL_TEXTURE_2D, "noise_tex", noise_tex);

    static GLuint VAO=0;
	static GLuint buffers[2];
	static bool was_here=false;
    static int NUM_VERTICES=-1;
    if(VAO == 0)
	{
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
		was_here = true;
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
        GLuint vertex_attrib = shader_prog.get_attrib_location("vertex");
        glEnableVertexAttribArray(vertex_attrib);
        glVertexAttribPointer(vertex_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
		
        glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
        glBufferData(GL_ARRAY_BUFFER, normals.size()*sizeof(Vec3f), &normals[0], GL_STATIC_DRAW);
        GLuint normal_attrib = shader_prog.get_attrib_location("normal");
        glEnableVertexAttribArray(normal_attrib);
        glVertexAttribPointer(normal_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

        NUM_VERTICES = vertices.size();
    }
    else
        glBindVertexArray(VAO);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, NUM_VERTICES);
}
