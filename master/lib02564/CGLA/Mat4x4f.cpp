#include "Mat4x4f.h"

namespace CGLA 
{

   Mat4x4f rotation_Mat4x4f(Axis axis, float angle)
	{
		Mat4x4f m(0.0f);

		switch(axis)
			{
			case XAXIS:
				m[0][0] = 1.0;
				m[1][1] = cos(angle);
				m[1][2] = sin(angle);
				m[2][1] = -sin(angle);
				m[2][2] = cos(angle);
				m[3][3] = 1.0;
				break;
			case YAXIS:
				m[0][0] = cos(angle);
				m[0][2] = -sin(angle);
				m[2][0] = sin(angle);
				m[2][2] = cos(angle);
				m[1][1] = 1.0;
				m[3][3] = 1.0;
				break;
			case ZAXIS:
				m[0][0] = cos(angle);
				m[0][1] = sin(angle);
				m[1][0] = -sin(angle);
				m[1][1] = cos(angle);
				m[2][2] = 1.0;
				m[3][3] = 1.0;
				break;
			}
		return m;
	}

	Mat4x4f translation_Mat4x4f(const Vec3f& v)
	{
		Mat4x4f m(0.0f);

		m[0][0] = 1.0;
		m[1][1] = 1.0;
		m[2][2] = 1.0;
		m[3][3] = 1.0;
  
		m[0][3] = v[0];
		m[1][3] = v[1];
		m[2][3] = v[2];
  
		return m;
	}

	Mat4x4f scaling_Mat4x4f(const Vec3f& v)
	{
		Mat4x4f m(0.0f);

		m[0][0] = v[0];
		m[1][1] = v[1];
		m[2][2] = v[2];
		m[3][3] = 1.0;
   
		return m;
	}

    Mat4x4f lookat_Mat4x4f(const CGLA::Vec3f& pos, const CGLA::Vec3f& dir, const CGLA::Vec3f& up)
    {
        CGLA::Mat4x4f T = translation_Mat4x4f(-pos);

        CGLA::Vec3f X,Y,Z;

        Z = -normalize(dir);
        X = normalize(cross(up, Z));
        Y = cross(Z,X);

        CGLA::Mat4x4f M(CGLA::Vec4f(X,0),
                        CGLA::Vec4f(Y,0),
                        CGLA::Vec4f(Z,0),
                        CGLA::Vec4f(0,0,0,1));

        return M*T;
    }

    Mat4x4f perspective_Mat4x4f(float fovy, float A, float glnear, float glfar)
    {
        Mat4x4f P(0.0);
        float f = 1.0/(tan(0.5 * fovy * (M_PI/180.0)));
        P[0][0] = f/A;
        P[1][1] = f;
        P[2][2] = (glfar+glnear)/(glnear-glfar);
        P[2][3] = 2*glnear*glfar/(glnear-glfar);
        P[3][2] = -1;
        return P;
    }

    Mat4x4f ortho_Mat4x4f(const CGLA::Vec3f& lbn,const CGLA::Vec3f& rtf)
    {
        Mat4x4f P(0.0);
        P[0][0] = 2.0/(rtf[0]-lbn[0]);
        P[0][3] = -(rtf[0]+lbn[0])/(rtf[0]-lbn[0]);

        P[1][1] = 2.0/(rtf[1]-lbn[1]);
        P[1][3] = -(rtf[1]+lbn[1])/(rtf[1]-lbn[1]);

        P[2][2] = - 2.0/(rtf[2]-lbn[2]);
        P[2][3] = -(rtf[2]+lbn[2])/(rtf[2]-lbn[2]);

        P[3][3] = 1.0;

        return P;
    }
}
