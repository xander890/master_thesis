#include <algorithm>
#include "ArithVec3Float.h"
#include "Vec3f.h"
#include "Vec3d.h"

using namespace std;

namespace CGLA {

		template<class T, class V>
		void ArithVec3Float<T,V>::get_spherical(T &theta, T &phi, T &rlen ) const
		{  
				rlen = this->length();
				theta = acos((*this)[2]/rlen);    
				if ((*this)[0]>0)
						phi = atan((*this)[1]/(*this)[0]);
				else 
						if ((*this)[0]<0)
								phi = atan((*this)[1]/(*this)[0]) + M_PI;
						else 
								phi = ((*this)[1]>0) ? M_PI_2 : -1 * M_PI_2;
		}


		template<class T, class V>
		void ArithVec3Float<T,V>::set_spherical(T theta, T phi, T rlen )
		{
				(*this)[0] = rlen * sin(theta) * cos(phi);
				(*this)[1] = rlen * sin(theta) * sin(phi);
				(*this)[2] = rlen * cos(theta);
		}

		template<class T, class V>
		void orthogonal(const ArithVec3Float<T,V>& _a, 
										ArithVec3Float<T,V>& b, 
										ArithVec3Float<T,V>& c)
		{
				V a = normalize(_a);
				T max_sqval=sqr(a[0]);
				unsigned int mi=0;
				for(unsigned int i=1;i<3;i++)
				{
						T sqval = sqr(a[i]);
						if(max_sqval<sqval)
						{
								max_sqval = sqval;
								mi = i;
						}
				}
				b[mi] = 0;
				b[(mi+1)%3] = 1;
				b[(mi+2)%3] = 0;

				b = normalize(b-a*dot(b,a));
				c = normalize(cross(a,b));

				if(dot(cross(b,c), a) < 0) swap(b,c);
		}


		template class ArithVec3Float<float, Vec3f>;
		template void orthogonal<float,Vec3f>(const ArithVec3Float<float,Vec3f>& a, 
																					ArithVec3Float<float,Vec3f>& b, 
																					ArithVec3Float<float,Vec3f>& c);

		template class ArithVec3Float<double, Vec3d>;
		template void orthogonal<double,Vec3d>(const ArithVec3Float<double,Vec3d>& a,
																					 ArithVec3Float<double,Vec3d>& b, 
																					 ArithVec3Float<double,Vec3d>& c);

}
