/*
 *  ThreeDObject.cpp
 *  02564_Framework
 *
 *  Created by J. Andreas Bærentzen on 02/02/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include <CGLA/Quatf.h>
#include <CGLA/Mat4x4f.h>
#include "ThreeDObject.h"

using namespace CGLA;
using namespace Mesh;
using namespace std;

namespace GLGraphics
{	

    ThreeDObject::ThreeDObject():
    translation_vector(0), rotation_axis(1,0,0),
    rotation_angle(0), scaling_factors(1,1,1), mesh(Mesh::TriangleMesh())
    {
	}

    bool ThreeDObject::init(std::string filename, std::string name){
        this->name = name;
        return mesh.load(filename, true);
    }


    void ThreeDObject::display(ShaderProgramDraw& shader_prog)
	{
        Quatf q;
        q.make_rot(rotation_angle*M_PI/180.0, rotation_axis);

        Mat4x4f M =
        translation_Mat4x4f(translation_vector)
        *q.get_Mat4x4f()
        *scaling_Mat4x4f(scaling_factors);
        shader_prog.set_model_matrix(M);
        this->mesh.render(shader_prog);
	}
	

}
