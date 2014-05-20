#include "shaderpreprocessor.h"

#include "GLGraphics/ResourceLoader.h"
#include <QDir>
#include <QString>
#include <iostream>

using namespace std;

ShaderPreprocessor::ShaderPreprocessor(const string &shader_path) : shader_path(shader_path), processed(map<string,string>()), includes(map<string,string>()), includesLoaded(false)
{

}

void ShaderPreprocessor::loadIncludes()
{
    QString path(shader_path.c_str());
    GLGraphics::ResourceLoader r;
    QString base_path(r.get_optional_resource_path().c_str());
    base_path.append(path);
    QDir shader_folder(base_path);
    cout << shader_folder.absolutePath().toStdString().c_str() << endl;
    cout << shader_folder.exists() << endl;

    QStringList nameFilter("*.glinc");

    QStringList include_files = shader_folder.entryList(nameFilter);
    for(int i = 0; i < include_files.size(); i++)
    {
        std::string key = include_files.at(i).toStdString();
        std::string text = r.load_text_resource(shader_path + key);
        includes[key] = text;
    }

    //string test = r.load_text_resource(shader_path + "ss_cubemap_render_to_arraymap_multilight_jensen.frag");
    //processShader(test);
}

void ShaderPreprocessor::processShader(string &shader)
{
    if(!includesLoaded)
    {
        loadIncludes();
        includesLoaded = true;
    }

    size_t currentChar = -1;
    do
    {
        currentChar = shader.find("#include", currentChar+1);
        size_t begin = shader.find("\"",currentChar+1);
        size_t end = shader.find("\"",begin+1);
        size_t eol = shader.find("\n",currentChar);

        if(begin != end)
        {
            string include_key = shader.substr(begin + 1,end - begin - 1);
            string include = includes[include_key];
            //cout << include.c_str() << endl;
            size_t l = eol - currentChar + 1;
            if(include.length() > 0)
            {
                shader.replace(currentChar, l, include);
            }
            else
            {
                cout << "Error: include " << include_key << " not found." << endl;
            }
        }
    }
    while(currentChar < shader.length());

    //cout << shader << endl;
}

void ShaderPreprocessor::reload()
{
    loadIncludes();
}


