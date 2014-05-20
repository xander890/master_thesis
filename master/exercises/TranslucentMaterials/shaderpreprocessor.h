#ifndef SHADERPREPROCESSOR_H
#define SHADERPREPROCESSOR_H
#include <string>
#include <map>
class ShaderPreprocessor
{
    std::map<std::string,std::string> includes;
    std::map<std::string,std::string> processed;
    bool includesLoaded;
    std::string shader_path;
    void loadIncludes();
public:
    ShaderPreprocessor(const std::string & shader_path);
    void processShader(std::string & shader);
    void reload();
};

#endif // SHADERPREPROCESSOR_H
