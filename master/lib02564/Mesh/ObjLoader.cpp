#include "ObjLoader.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <sstream>
#include <GLGraphics/ResourceLoader.h>

using namespace std;
using namespace CGLA;
using namespace Mesh;

namespace Mesh {

Vec3f to_vec3(istringstream &iss){
    float x,y,z;
    if (iss.good()){
        iss >> x;
    }
    if (iss.good()){
        iss >> y;
    }
    if (iss.good()){
        iss >> z;
    }
    return Vec3f(x,y,z);
}

Vec2f to_vec2(istringstream &iss){
    float x,y;
    if (iss.good()){
        iss >> x;
    }
    if (iss.good()){
        iss >> y;
    }
    return Vec2f(x,y);
}

struct TriangleIndex{
    int position;
    int normal;
    int uv;

    bool replace(std::string& str, const std::string& from, const std::string& to) {
        size_t start_pos = str.find(from);
        if(start_pos == std::string::npos)
            return false;
        str.replace(start_pos, from.length(), to);
        return true;
    }

    TriangleIndex(string p):position(-1),normal(-1),uv(-1) {
        // position/uv/normal
        replace(p, "//","/-1/");
        stringstream ss(p);
        char buffer[50];

        ss.getline(buffer,50, '/');
        position = atoi(buffer);
        if (ss.good()){
            ss.getline(buffer,50, '/');
            uv = atoi(buffer);
        }
        if (ss.good()){
            ss.getline(buffer,50, '/');
            normal = atoi(buffer);
        }
    }

    // needed to use TriangleIndex as key in map
    bool operator <(const TriangleIndex& Rhs) const {
        return (position < Rhs.position) ||
                (position == Rhs.position && normal < Rhs.normal) ||
                (position == Rhs.position && normal == Rhs.normal && uv < Rhs.uv);
    }
};

struct TriangleString{
    TriangleIndex v0;
    TriangleIndex v1;
    TriangleIndex v2;
    int materialIndex;

    TriangleString(string v0, string v1, string v2, int materialIndex):v0(v0),v1(v1),v2(v2),materialIndex(materialIndex){
    }

    TriangleIndex get(int index){
        if (index == 0) {
            return v0;
        } else if (index == 1) {
            return v1;
        }
        return v2;
    }
};



string get_path(const string& _filename)
{
    // Make sure we only have slashes and not backslashes
    string filename = _filename;
    replace(filename.begin(),filename.end(),'\\','/');

    // Find the last occurrence of slash.
    // Everything before is path.
    unsigned int n = filename.rfind("/");
    if(n > filename.size())
        return "./";
    string pathname = "";
    pathname.assign(filename,0,n);
    pathname.append("/");
    return pathname;
}
vector<Material> load_material(const string& path, const string& filename){
    const string materialSource = GLGraphics::ResourceLoader::load_text_resource(path+filename);
    vector<Material> materials;

    istringstream ifs(materialSource);

    char buf[512];
    int nummaterials=-1;
    while(ifs.good())
    {
        ifs.getline(buf,512);
        string line(buf);
        istringstream material_file(line);
        if (material_file.good() && line.length()>0){
            string token;
            material_file >> token;
            switch(token[0])
            {
            case 'n': // newmtl
                materials.push_back(Material());
                ++nummaterials;
                if (material_file.good()){
                    material_file >> materials[nummaterials].name;
                }
                break;
            case 'N':
                switch(token[1])
                {
                case 's':
                    if (material_file.good()){
                        material_file >> materials[nummaterials].shininess;
                    }
                    materials[nummaterials].shininess *= 128.0f/1000.0f;
                    break;
                case 'i':
                    if (material_file.good()){
                        material_file >> materials[nummaterials].ior;
                    }
                    break;
                }
                break;
            case 'K':
                switch(token[1])
                {
                case 'd':
                    materials[nummaterials].diffuse = Vec4f(to_vec3(material_file),1.0);
                    break;
                case 's':
                    materials[nummaterials].specular = Vec4f(to_vec3(material_file));
                    break;
                case 'a':
                    materials[nummaterials].ambient = Vec4f(to_vec3(material_file));
                    break;
                }
                break;
            case 'T':
                switch(token[1])
                {
                case 'f':
                    materials[nummaterials].transmission = Vec4f(to_vec3(material_file));
                    break;
                }
                break;
            case 'i':
                if (material_file.good()){
                    material_file >> materials[nummaterials].illum;
                }
                break;
            case 'm': // Map ... all maps are treated equally.
                if (material_file.good() && token == "map_Kd"){
                    string name;
                    material_file >> name;
                    string texname = GLGraphics::ResourceLoader::compute_resource_path(path + name);
                    materials[nummaterials].tex_map.load(texname);
                    materials[nummaterials].has_texture = true;
                }
                break;
            case '#':
            default:
                material_file.ignore(1024, '\n');
                break;
            }
        }
    }

    return materials;
}



bool ObjLoader::load_object(const char * filename,
                           vector<Vec3f> *outPositions,
                           vector<vector<GLuint> > *outIndices,
                           vector<Material> *outMaterials,
                           vector<Vec3f> *outNormal,
                           vector<Vec2f> *outUv,
                           float scale){

    vector<Vec3f> positions;
    vector<Vec3f> normals;
    vector<Vec2f> uvs;
    const string path = get_path(filename);
    string resource = GLGraphics::ResourceLoader::load_text_resource(filename);
    vector<TriangleString> triangles;
    map<string, int> materialIndexMap; // maps from material index to material name
    istringstream ifs(resource);
    unsigned int currentMaterialIndex = 0;
    char buffer[512];
    while (ifs.good()){
        ifs.getline(buffer,512);

        string line(buffer);
        istringstream iss(line);
        if (iss.good() && line.length()>0){
            string token;
            iss >> token;
            if (token.compare("o")==0){
                // does not support multiple objects
            } else if (token.compare("mtllib")==0){
                // does not support multiple materials
                string matFilename;
                iss >> matFilename;
                vector<Material> materials = load_material(path, matFilename);
                for (unsigned int i=0;i< materials.size();i++){
                    int materialIndex = outMaterials->size();
                    outMaterials->push_back(materials[i]);
                    string materialName = materials[i].name;
                    materialIndexMap[materialName] = materialIndex;
                }
            } else if (token.compare("usemtl")==0){
                // does not support multiple materials
                string name;
                iss >> name;
                map<string, int>::const_iterator namePos = materialIndexMap.find(name);
                if (namePos == materialIndexMap.end()){
                    cout << "Could not find material "<<name.c_str() << endl;
                    currentMaterialIndex = 0;
                } else {
                    currentMaterialIndex = materialIndexMap[name];
                }
            } else if (token.compare("v")==0){
                positions.push_back( to_vec3(iss));
            } else if (token.compare("vn")==0){
                normals.push_back( to_vec3(iss));
            } else if (token.compare("vt")==0){
                uvs.push_back( to_vec2(iss));
            } else if (token.compare("f")==0){
                vector<string> polygon;
                do {
                    string index;
                    iss >> index;
                    if (index.length() > 0) {
                        polygon.push_back(index);
                    }
                } while (iss);

                // triangulate pologon
                TriangleString triangle(polygon[0], polygon[1], polygon[2], currentMaterialIndex);
                triangles.push_back(triangle);
                for (unsigned int i=3;i<polygon.size();i++){
                    TriangleString triangle2(polygon[i-1], polygon[i], polygon[0], currentMaterialIndex);
                    triangles.push_back(triangle2);
                }
            } else if (token.compare("g")==0){
                // group name not supported
            } else if (token.compare("s")==0){
                // smooth shade. Always smooth shade
            } else if (token[0] == '#' || line.length() == 0){
                // comment - ignore line
            } else {
                cout << "ObjLoader::Unsupported line:" << endl;
                cout <<"'"<< line.c_str() << "'"<<token.compare("#")<<int(token[0])<<endl;
            }
        }
    }


    if (outMaterials->size() == 0){ // ensure a single material object exist
        outMaterials->push_back(Material());
    }
    map<TriangleIndex,int> cache;
    for (unsigned int i=0;i<triangles.size();i++){
        TriangleString triangleString = triangles[i];
        for (int j=0;j<3;j++){
            TriangleIndex index = triangleString.get(j);
//            map<TriangleIndex,int>::iterator cachedIndex = cache.find(index);
//            if (cachedIndex != cache.end()) {
//                (*outIndices)[triangleString.materialIndex].push_back(cachedIndex->second);
//            } else {
                int vertexIndex = (*outPositions).size();
                (*outPositions).push_back(positions[index.position-1] * scale);
                if (index.normal != -1 && outNormal != NULL){
                    (*outNormal).push_back(normals[index.normal-1]);
                }
                if (index.uv != -1 && outUv != NULL) {
                    (*outUv).push_back(uvs[index.uv-1]);
                }
                while (triangleString.materialIndex >= (*outIndices).size()){
                    (*outIndices).push_back(vector<GLuint>());
                }

 //               (*outIndices)[triangleString.materialIndex].push_back(vertexIndex);
 //               cache[index] = vertexIndex;
 //           }
        }
    }

    return true;
}



}
