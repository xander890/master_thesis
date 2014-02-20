#ifndef __CGLA_EXCEPTIONSTANDARD_H__
#define __CGLA_EXCEPTIONSTANDARD_H__

#include <string>
#include <iostream>

namespace CGLA
{

	class CGLAMotherException
	{
		std::string str;
	public:
		CGLAMotherException(const std::string s)
			{
				str = s;
			}
  
		void print(std::ostream& os) const 
		{
            os << str.c_str() << std::endl;
		}
	};

#define CGLA_DERIVEEXCEPTION(nameoe)															\
	class nameoe: public CGLAMotherException									\
	{																													\
	public:																										\
		nameoe(const std::string& s): CGLAMotherException(s) {}	\
	};																												\

}

#endif
