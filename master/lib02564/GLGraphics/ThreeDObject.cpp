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
#include "Dipoles/dipolegpu.h"

using namespace CGLA;
using namespace Mesh;
using namespace std;

namespace GLGraphics
{	

    ThreeDObject::ThreeDObject():
        translation_vector(0),
        rotation(Quatf(0,0,0,1)),
        scaling_factors(1,1,1),
        mesh(Mesh::TriangleMesh()),
        enabled(false),
        boundingBoxEnabled(false)
    {
	}

    bool ThreeDObject::init(std::string filename, std::string name, Mesh::Material & material){
        this->firstTime = true;
        this->name = name;
        return mesh.load(filename, material);
    }


    Mat4x4f ThreeDObject::getModelMatrix()
    {
        Mat4x4f M =
        translation_Mat4x4f(translation_vector)
        * rotation.get_Mat4x4f()
        * scaling_Mat4x4f(scaling_factors);
        return M;
    }


    void ThreeDObject::display(ShaderProgramDraw& shader_prog)
	{
        if(firstTime)
        {
            //DipoleGPU gip;
            //gip.prepare(*this);
            firstTime = false;
        }
        Mat4x4f M = getModelMatrix();
        shader_prog.set_model_matrix(M);
        this->mesh.render(shader_prog);
    }

    BoundingBox * ThreeDObject::getBoundingBox()
    {
        return this->mesh.getBoundingBox();
    }

    BoundingBox *ThreeDObject::getBoundingBoxAdjusted()
    {
        BoundingBox * toReturn = new BoundingBox();
        toReturn->high = Vec3f(getModelMatrix() * Vec4f(this->getBoundingBox()->high,1.0f));
        toReturn->low = Vec3f(getModelMatrix() * Vec4f(this->getBoundingBox()->low,1.0f));
        return toReturn;
    }

    void ThreeDObject::getRawData(RawMeshData &data)
    {
        this->mesh.getRawData(data);
    }

    Vec3f ThreeDObject::getCenter()
    {
        BoundingBox * b = getBoundingBox();
        return Vec3f(0.5f * getModelMatrix() * Vec4f(b->high + b->low));
    }
	
    void ThreeDObject::addAttribute(string name, std::vector<Vec4f> &data)
    {
        this->mesh.add(name, data);
    }

}
