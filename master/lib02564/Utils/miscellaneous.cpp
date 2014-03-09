#include "miscellaneous.h"

void duplicateMap(const std::vector<CGLA::Vec3f> &originalVector, std::map<GLuint, std::vector<GLuint> > &duplicates, float tolerance)
{
    for(int i = 0; i < originalVector.size(); i++)
    {
        for(int j = i + 1; j < originalVector.size(); j++)
        {
            if(i != j && length(originalVector[i] - originalVector[j]) < tolerance)
            {
                duplicates[i].push_back(j);
                duplicates[j].push_back(i);
            }
        }
    }
}

