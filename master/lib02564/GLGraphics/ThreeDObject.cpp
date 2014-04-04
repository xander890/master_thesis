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
        rotation_matrix(identity_Mat4x4f()),
        scaling_factors(1,1,1),
        mesh(Mesh::TriangleMesh()),
        enabled(false),
        boundingBoxEnabled(false),
        adjustedBox(new Mesh::BoundingBox())
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
        *rotation_matrix
        *scaling_Mat4x4f(scaling_factors);
        return M;
    }


    void ThreeDObject::display(ShaderProgramDraw& shader_prog)
	{
        if(firstTime)
        {
            DipoleGPU gip;
            gip.prepare(*this);
            firstTime = false;
        }
        Mat4x4f M = getModelMatrix();
        shader_prog.set_model_matrix(M);
        this->mesh.render(shader_prog);
    }

    BoundingBox * ThreeDObject::getBoundingBox()
    {
        BoundingBox * box = this->mesh.getBoundingBox();
        Mat4x4f m = getModelMatrix();
        Vec4f low = Vec4f(box->xlow,box->ylow,box->zlow,1.0f);
        Vec4f high = Vec4f(box->xhigh,box->yhigh,box->zhigh,1.0f);
        low = m * low;
        high = m * high;
        adjustedBox->xlow = low[0];
        adjustedBox->ylow = low[1];
        adjustedBox->zlow = low[2];
        adjustedBox->xhigh = high[0];
        adjustedBox->yhigh = high[1];
        adjustedBox->zhigh = high[2];
        return adjustedBox;
    }

    void ThreeDObject::getRawData(RawMeshData &data)
    {
        this->mesh.getRawData(data);
    }

    Vec3f ThreeDObject::getCenter()
    {
        BoundingBox * b = getBoundingBox();
        return 0.5f *  Vec3f(b->xhigh + b->xlow, b->yhigh + b->ylow, b->zhigh + b->zlow);
    }
	
    void ThreeDObject::addAttribute(string name, std::vector<Vec4f> &data)
    {
        this->mesh.add(name, data);
    }

}
