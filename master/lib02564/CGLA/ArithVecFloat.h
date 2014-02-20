#ifndef __CGLA__ARITHVECFLOAT_H__
#define __CGLA__ARITHVECFLOAT_H__

#include "ArithVec.h"

namespace CGLA {

  template<class T, class V, unsigned int N>
    class ArithVecFloat: public ArithVec<T,V,N>
    {
    public:
	  
      ArithVecFloat() 
	{
#ifndef NDEBUG
	  std::fill_n(this->data, N, CGLA_INIT_VALUE);
#endif
	}
	  
      ArithVecFloat(T a): 
	ArithVec<T,V,N>(a) {}

      ArithVecFloat(T a, T b): 
	ArithVec<T,V,N>(a,b) {}

      ArithVecFloat(T a, T b, T c): 
	ArithVec<T,V,N>(a,b,c) {}

      ArithVecFloat(T a, T b, T c, T d): 
	ArithVec<T,V,N>(a,b,c,d) {}

      /// Compute Euclidean length.
      T length() const 
	{
	  return sqrt(sqr_length(*this));
	}
    
      /// Normalize vector.
      void normalize() 
	{
	  (*this) /= this->length();
	}
		
    };

  /// Returns normalized vector
  template<class T, class V, unsigned int N>
    inline T length(const ArithVecFloat<T,V,N>& v) 
    {
      return v.length();
    }
	
	
  /// Returns normalized vector
  template<class T, class V, unsigned int N>
    inline V normalize(const ArithVecFloat<T,V,N>& v) 
    {
      return v/v.length();
    }
}

#endif

