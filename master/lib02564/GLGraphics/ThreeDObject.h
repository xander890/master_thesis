#ifndef __THREE_D_OBJECT_H__
#define __THREE_D_OBJECT_H__

/*
 *  ThreeDObject.h
 *  02564_Framework
 *
 *  Created by J. Andreas Bærentzen on 02/02/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "GLHeader.h"
#include <CGLA/Vec3f.h>
#include "ShaderProgram.h"
#include <Mesh/TriangleMesh.h>
#include <string>
namespace GLGraphics
{

class ThreeDObject
{

protected:
    CGLA::Vec3f translation_vector;
    CGLA::Vec3f rotation_axis;
    float rotation_angle;
    CGLA::Vec3f scaling_factors;
    std::string name;

public:
    Mesh::TriangleMesh mesh;

    ThreeDObject();
    virtual bool init(std::string filename, std::string name);
    virtual void display(ShaderProgramDraw& shader);

    void rotate(const CGLA::Vec3f& axis, float angle)
    {
        rotation_axis = axis;
        rotation_angle = angle;
    }

    void translate(const CGLA::Vec3f& pos)
    {
        translation_vector = pos;
    }

    void scale(const CGLA::Vec3f& scale)
    {
        scaling_factors  = scale;
    }



};

}

#endif
