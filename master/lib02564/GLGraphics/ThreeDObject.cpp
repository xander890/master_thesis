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
    translation_vector(0), rotation_matrix(identity_Mat4x4f()), scaling_factors(1,1,1), mesh(Mesh::TriangleMesh())
    {
	}

    bool ThreeDObject::init(std::string filename, std::string name, Mesh::Material & material){
        this->name = name;
        return mesh.load(filename, true);
    }


    Mat4x4f ThreeDObject::getModelMatrix()
    {
        Mat4x4f M =
        translation_Mat4x4f(translation_vector)
        *rotation_matrix
        *scaling_Mat4x4f(scaling_factors);
        return M;
    }


    void ThreeDObject::display(ShaderProgramDraw& shader_prog)
	{
        Mat4x4f M = getModelMatrix();
        shader_prog.set_model_matrix(M);
        this->mesh.render(shader_prog);
    }

    void ThreeDObject::getRawData(RawMeshData &data)
    {
        this->mesh.getRawData(data);
    }
	
    void ThreeDObject::addAttribute(string name, std::vector<Vec4f> &data)
    {
        this->mesh.add(name, data);
    }

}
