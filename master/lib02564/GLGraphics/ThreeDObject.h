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
    CGLA::Mat4x4f rotation_matrix; //optimization.
    CGLA::Vec3f eulerAngles;
    CGLA::Vec3f scaling_factors;
    bool firstTime;
    Mesh::BoundingBox * adjustedBox;

public:
    Mesh::TriangleMesh mesh;
    std::string name;
    bool enabled;
    bool boundingBoxEnabled;

    ThreeDObject();
    virtual bool init(std::string filename, std::string name, Mesh::Material & material);
    virtual void display(ShaderProgramDraw& shader);

    Mesh::BoundingBox * getBoundingBox();
    void getRawData(Mesh::RawMeshData & data);
    CGLA::Mat4x4f ThreeDObject::getModelMatrix();
    CGLA::Vec3f ThreeDObject::getEulerAngles() {return eulerAngles;}
    CGLA::Vec3f ThreeDObject::getPosition() {return translation_vector;}
    CGLA::Vec3f ThreeDObject::getScale() {return scaling_factors;}
    CGLA::Vec3f getCenter();

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
        rotation_matrix = q.get_Mat4x4f();
        this->eulerAngles = axis * angle;
    }

    void setEulerAngles(CGLA::Vec3f & eulerAngles)
    {
        //avoiding singular matrix
        if(eulerAngles == CGLA::Vec3f(0.0f))
        {
            this->rotation_matrix = CGLA::identity_Mat4x4f();
            this->eulerAngles = eulerAngles;
            return;
        }


        //precomputing the rotation matrix, so we do not have to do quaternion stuff
        //every frame unless we are forced to.
        CGLA::Quatf qx;
        CGLA::Quatf qy;
        CGLA::Quatf qz;
        qx.make_rot(eulerAngles[0] * M_PI / 180.0f, CGLA::Vec3f(1.0f,0.0f,0.0f));
        qy.make_rot(eulerAngles[1] * M_PI / 180.0f, CGLA::Vec3f(0.0f,1.0f,0.0f));
        qz.make_rot(eulerAngles[2] * M_PI / 180.0f, CGLA::Vec3f(0.0f,0.0f,1.0f));
        CGLA::Quatf q = qx * (qy * qz);

        this->eulerAngles = eulerAngles;
        rotation_matrix = q.get_Mat4x4f();
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
