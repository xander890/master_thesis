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
#include <CGLA/Quatf.h>
#include "ShaderProgram.h"
#include <Mesh/TriangleMesh.h>
#include <string>

namespace GLGraphics
{

class ThreeDObject
{

protected:
    CGLA::Vec3f translation_vector;
    //CGLA::Vec3f rotation_axis;
    //float rotation_angle;
    CGLA::Mat4x4f rotation_matrix;
    CGLA::Vec3f scaling_factors;

public:
    Mesh::TriangleMesh mesh;
    std::string name;

    ThreeDObject();
    virtual bool init(std::string filename, std::string name, Mesh::Material & material);
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

        CGLA::Quatf q;
        q.make_rot(angle*M_PI/180.0, axis);
        rotation_matrix *= q.get_Mat4x4f();
    }

    void rotate(const CGLA::Mat4x4f & rotation)
    {

        rotation_matrix *= rotation;
        rotation_matrix[0][3] = 0.0f;
        rotation_matrix[1][3] = 0.0f;
        rotation_matrix[2][3] = 0.0f;
    }


    void set_rotation(const CGLA::Mat4x4f & rotation)
    {



        rotation_matrix = rotation;
        rotation_matrix[0][3] = 0.0f;
        rotation_matrix[1][3] = 0.0f;
        rotation_matrix[2][3] = 0.0f;

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
