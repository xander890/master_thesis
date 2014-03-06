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

public:
    Mesh::TriangleMesh mesh;
    std::string name;

    ThreeDObject();
    virtual bool init(std::string filename, std::string name);
    virtual void display(ShaderProgramDraw& shader);
    void getRawData(Mesh::RawMeshData & data);

    void addAttribute(std::string name, std::vector<CGLA::Vec4f>& data);

    bool operator == (const ThreeDObject &b) const
    {
        return strcmp(name.c_str(),b.name.c_str()) == 0;
    }

    bool operator == (const std::string &b) const
    {
        return strcmp(name.c_str(),b.c_str()) == 0;
    }

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

    CGLA::Mat4x4f ThreeDObject::getModelMatrix();

};

}

#endif
