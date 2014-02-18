/*
 *  User.h
 *  02564_Framework
 *
 *  Created by J. Andreas Bærentzen on 02/02/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __USER_H__
#define __USER_H__

#include <CGLA/Vec4f.h>
#include <CGLA/Vec3f.h>
#include <CGLA/Vec2f.h>
#include <CGLA/Mat4x4f.h>
#include <cmath>
#include <GLGraphics/GLHeader.h>
#include <GLGraphics/ShaderProgram.h>

#undef min
#undef max

namespace GLGraphics {
    
    
    class HeightMap 
    {
    public:
        virtual float height(float x, float y) const
        {
            return 0*(x + y) + 0.5;
        }
    };
    
    const HeightMap* default_height_map();
    
    /** A simple class encapsulating position and direction of user */
    class User
    {
        CGLA::Vec3f pos;
        CGLA::Vec3f dir;
        CGLA::Vec2f speed;
        const HeightMap* terra;
        
    public:
        /** Construct by passing a terrain. The user is always on the terrain, so he needs to 
         query the height function. */
        User(const HeightMap* _terra): pos(0,0,0), dir(normalize(CGLA::Vec3f(1,1,0))), speed(0,0), terra(_terra) 
        {
            pos = CGLA::Vec3f(0,0,terra->height(0,0));
        }
        
        User(): pos(0,0,0), dir(normalize(CGLA::Vec3f(1,1,0))), speed(0,0), terra(0)
        {
            terra = default_height_map();
        }
        
        /// Move forward, project to terrain.
        void update_position() 
        {
            pos += speed[0] * normalize(CGLA::Vec3f(dir[0],dir[1],0));
            pos += speed[1] * normalize(CGLA::Vec3f(-dir[1],dir[0],0));
            pos[2] = std::max(1.0f,1.0f + terra->height(pos[0], pos[1]));
        }
        
        /// Get position
        const CGLA::Vec3f& get_pos() const {return pos;}
        
        /// Get direction
        const CGLA::Vec3f& get_dir() const {return dir;}
        
        /// Don't move!
        void stop() {speed = CGLA::Vec2f(0,0);}
        
        /// Go forward
        void forward() {speed = CGLA::Vec2f(.075f,0);}
        
        ///
        void run() {speed = CGLA::Vec2f(.15f,0);}

        /// Go back
        void back() {speed = CGLA::Vec2f(-.15f,0);}
        
        /// Move directly sideways left
        void strafe_left() {speed = CGLA::Vec2f(0,.15f);}
        
        /// Move directly sideways right
        void strafe_right() {speed = CGLA::Vec2f(0,-.15f);}
        
        /// Set orientation (in 2D).
        void set_dir(int mx, int my)
        {
            CGLA::Vec3f _dir;
            dir += -0.003*CGLA::Vec3f(-dir[1],dir[0],0)*mx+ 0.003*CGLA::Vec3f(0,0,1)*my;
            dir.normalize();	
        }
        
        /// Set position (in 2D).
        void set(const CGLA::Vec3f& _pos, const CGLA::Vec3f& _dir)
        {
            pos = _pos;
            dir = _dir;
        }
        
        /// Set orientation in 3D.
        void set_dir(const CGLA::Vec3f& _dir)
        {
            dir = _dir;
        }
        
        CGLA::Mat4x4f get_view_matrix() const
        {
            return lookat_Mat4x4f(pos, dir, CGLA::Vec3f(0,0,1));
        }
        
        
        void set_gl_view_zreflected(float z)
        {
            z += 0.0f;
            return;
//            gluLookAt(pos[0], pos[1], z-(pos[2]-z), 
//                      pos[0]+dir[0],
//                      pos[1]+dir[1],
//                      z-(pos[2]-z)-dir[2],
//                      0,0,1);
        }
    };
}

#endif 
