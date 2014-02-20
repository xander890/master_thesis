#ifndef RESOURCELOADER_H
#define RESOURCELOADER_H

#include <string>

namespace GLGraphics
{
    class ResourceLoader
    {
    public:
        static std::string get_optional_resource_path();
        static std::string compute_resource_path(const std::string &resourceName);
        static std::string load_text_resource(const std::string &resourceName);
    };
}

#endif // RESOURCELOADER_H
