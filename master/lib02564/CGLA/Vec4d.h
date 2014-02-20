#ifndef __CGLA_VEC4D_H__
#define __CGLA_VEC4D_H__

#include "Vec3d.h"
#include "ArithVec4Float.h"

namespace CGLA {

	/** \brief A four dimensional floating point vector. 

			This class is also used (via typedef) for
			homogeneous vectors.
	*/

	class Vec4d: public ArithVec4Float<double,Vec4d>
	{
	public:
  
		/// Construct a (0,0,0,0) homogenous Vector
		Vec4d(): ArithVec4Float<double,Vec4d>(0,0,0,0.0) {}

		/// Construct a (0,0,0,0) homogenous Vector
		explicit Vec4d(double _a): ArithVec4Float<double,Vec4d>(_a,_a,_a,_a) {}

		/// Construct a 4D vector
		Vec4d(double _a, double _b, double _c, double _d): 
			ArithVec4Float<double,Vec4d>(_a,_b,_c,_d) {}

		/// Construct a homogenous vector (a,b,c,1)
		Vec4d(double _a, double _b, double _c): 
			ArithVec4Float<double,Vec4d>(_a,_b,_c,1.0) {}

		/// Construct a homogenous vector from a non-homogenous.
		explicit Vec4d(const Vec3d& v): 
			ArithVec4Float<double,Vec4d>(v[0],v[1],v[2],1.0) {}

		/// Construct a homogenous vector from a non-homogenous.
		explicit Vec4d(const Vec3d& v,double _d): 
			ArithVec4Float<double,Vec4d>(v[0],v[1],v[2],_d) {}

		operator Vec3d() const
		{
			double k = 1.0/(*this)[3];
			return Vec3d((*this)[0]*k,(*this)[1]*k,(*this)[2]*k);
		}
	};
}
#endif

