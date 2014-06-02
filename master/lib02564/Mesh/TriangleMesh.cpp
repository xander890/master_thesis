#include "TriangleMesh.h"

#include <iostream>
#include "ObjLoader.h"
#include "Utils/performancetimer.h"
#include <string>

using namespace std;
using namespace CGLA;
using namespace GLGraphics;

namespace Mesh {

TriangleMesh::TriangleMesh()
    : vertexArrayObject(0), vertexBuffer(0), vertexCount(0), debug(true), initialized(false),
      offset(), componentType(), componentSize(), names(), stride(0), boundingBox(new BoundingBox())
{
}


bool TriangleMesh::load(const string &filename, Material & material){
    vector<Vec3f> * outPositions = new vector<Vec3f>();
    vector<Vec3f> * outNormal = new vector<Vec3f>();
    vector<Vec2f> * outUv = new vector<Vec2f>();
    vector<vector<GLuint> > * outIndices = new vector<vector<GLuint> >();
    vector<Material> * outMaterials = new vector<Material>();

    bool loaded = ObjLoader::load_object(filename.c_str(),
        outPositions,
        outIndices,
        outMaterials,
        outNormal,
        outUv,
        1.0f);

    if (!loaded){
        return false;
    }


    return load_external((*outIndices)[0], (*outPositions),(*outNormal),(*outUv),material, GL_TRIANGLES);
}

bool TriangleMesh::load_external(vector<GLuint> & indices, vector<Vec3f>& outPositions, vector<Vec3f>& outNormal, vector<Vec2f>& outUv, Material& outMaterial, GLenum type)
{
    add("vertex", outPositions);
    add_draw_call(indices, indices.size(), outMaterial, type);

    if (outNormal.size()>0){
        add("normal", outNormal);
        rawData.normals = outNormal;
    }
    else
    {
        recompute_normals();
    }
    if (outUv.size()>0){
        add("texcoord", outUv);
    }
    rawData.vertices = outPositions;
    rawData.uvs = outUv;
    rawData.indices = indices;

    this->generateBoundingBox();

    return true;
}

void TriangleMesh::getRawData(RawMeshData & data)
{
    data.vertices = rawData.vertices;
    data.normals = rawData.normals;
    data.uvs = rawData.uvs;
    data.indices = rawData.indices;
}

Material * TriangleMesh::getMaterial()
{
    return &(this->drawCalls.at(0).material);
}

GLenum TriangleMesh::getMode()
{
    //support only to the first draw call
    return drawCalls.at(0).renderMode;
}

BoundingBox *TriangleMesh::getBoundingBox()
{
    return this->boundingBox;
}

void TriangleMesh::check_vertex_size(int vertexAttributeSize) {
    if (vertexCount != 0 && vertexCount != vertexAttributeSize && debug){
        cout << "invalid vertex attribute size" << endl;
    }
    vertexCount = vertexAttributeSize;
}

void TriangleMesh::add(const string &name, vector<int> &vertexAttributesI){
    check_vertex_size(vertexAttributesI.size());
    DataVector dataVector = {GL_INT, sizeof(int), 1, std::vector<DataUnion>()};
    DataUnion d;
    for (unsigned int i=0;i<vertexAttributesI.size();i++){
        d.i[0] = vertexAttributesI[i];
        dataVector.vector.push_back(d);
    }
    vertexAttributes[name] = dataVector;
}

void TriangleMesh::add(const string &name, vector<float> &vertexAttributesF){
    check_vertex_size(vertexAttributesF.size());
    DataVector dataVector = {GL_FLOAT, sizeof(float), 1, std::vector<DataUnion>()};
    DataUnion d;
    for (unsigned int i=0;i<vertexAttributesF.size();i++){
        d.vec[0] = vertexAttributesF[i];
        dataVector.vector.push_back(d);
    }
    vertexAttributes[name] = dataVector;
}

void TriangleMesh::add(const string &name, vector<Vec2f> &vertexAttributesV2){
    check_vertex_size(vertexAttributesV2.size());
    DataVector dataVector = {GL_FLOAT, sizeof(float)*2, 2, std::vector<DataUnion>()};
    DataUnion d;
    for (unsigned int i=0;i<vertexAttributesV2.size();i++){
        d.vec[0] = vertexAttributesV2[i][0];
        d.vec[1] = vertexAttributesV2[i][1];
        dataVector.vector.push_back(d);
    }
    vertexAttributes[name] = dataVector;
}

void TriangleMesh::add(const string &name, vector<Vec3f> &vertexAttributesV3){
    check_vertex_size(vertexAttributesV3.size());
    DataVector dataVector = {GL_FLOAT, sizeof(float)*3, 3, std::vector<DataUnion>()};
    DataUnion d;
    for (unsigned int i=0;i<vertexAttributesV3.size();i++){
        d.vec[0] = vertexAttributesV3[i][0];
        d.vec[1] = vertexAttributesV3[i][1];
        d.vec[2] = vertexAttributesV3[i][2];
        dataVector.vector.push_back(d);
    }
    vertexAttributes[name] = dataVector;
}

void TriangleMesh::add(const string &name, vector<Vec4f> &vertexAttributesV4){
    check_vertex_size(vertexAttributesV4.size());
    DataVector dataVector = {GL_FLOAT, sizeof(float)*4, 4, std::vector<DataUnion>()};
    DataUnion d;
    for (unsigned int i=0;i<vertexAttributesV4.size();i++){
        d.vec[0] = vertexAttributesV4[i][0];
        d.vec[1] = vertexAttributesV4[i][1];
        d.vec[2] = vertexAttributesV4[i][2];
        d.vec[3] = vertexAttributesV4[i][3];
        dataVector.vector.push_back(d);
    }
    vertexAttributes[name] = dataVector;
}

void TriangleMesh::add_draw_call(vector<GLuint> indices, int count, Material &material, GLenum renderMode){
    int offset = 0;
    if (drawCalls.size()>0){
        offset = drawCalls[drawCalls.size()-1].offset + drawCalls[drawCalls.size()-1].count * sizeof(GLuint);
    }
    DrawCall c = {
        indices,
        material,
        renderMode,
        count,
        offset
    };
    drawCalls.push_back(c);
}

void copy_to_interleaved_data(DataUnion& data, vector<float> &interleavedData, int bytes){
    assert(bytes<=16);
    for (int i=0;i<bytes/4;i++){
        interleavedData.push_back(data.vec[i]);
    }
}

void TriangleMesh::map_data_to_shader_vertex_attributes(GLGraphics::ShaderProgram *shader){
    // map data to shader vertex attributes
    for (unsigned int i=0;i<names.size();i++){
        GLint location;
        if (shader != NULL){
            location = shader->get_attrib_location(names[i].c_str());
            if(location == -1)
            {
                location = ShaderProgramDraw::get_generic_attrib_location(names[i].c_str());
            }
        } else {
            location = ShaderProgramDraw::get_generic_attrib_location(names[i].c_str());
        }
        if (location != -1){
            glEnableVertexAttribArray(location);
            glVertexAttribPointer(location, componentSize[i], componentType[i], GL_FALSE, stride, (const GLvoid *)offset[i]);
        } else if (debug){
            cout << "Cannot find vertex attribute location "<<names[i].c_str() << " in shader" << endl;
        }
    }
}


void TriangleMesh::build_vertex_array_object(GLGraphics::ShaderProgram *shader){
    // create interleaved data
    vector<float> interleavedData;
    vector<int> sizeInBytes;
    offset.clear();
    componentType.clear();
    componentSize.clear(); // 1 for float, 2 for Vec2f, etc,
    names.clear();
    stride = 0;
    // do the actual interleaving of the data
    for (int i=0;i<vertexCount;i++){
        for (std::map<std::string, DataVector>::iterator it = vertexAttributes.begin();it != vertexAttributes.end();it++){
            int numberOfComponents = it->second.components;
            int size = it->second.size;
            copy_to_interleaved_data(it->second.vector[i], interleavedData, size);

            if (i==0){
                offset.push_back(stride);
                stride += size;
                sizeInBytes.push_back(size);
                names.push_back(it->first);
                componentType.push_back(it->second.glType);
                componentSize.push_back(numberOfComponents);
            }
        }
    }

    glGenVertexArrays(1, &vertexArrayObject);
    glGenBuffers(1, &vertexBuffer);

    glBindVertexArray(vertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    // upload to vertex buffer
    glBufferData(GL_ARRAY_BUFFER, stride * vertexCount, &(interleavedData[0]), GL_STATIC_DRAW);


    // concatenate all indices
    std::vector<GLuint> elementArrayBuffer;
    for (std::vector<DrawCall>::iterator iter = drawCalls.begin();iter != drawCalls.end(); iter++){
        elementArrayBuffer.insert(elementArrayBuffer.end(),iter->indices.begin(), iter->indices.end() );
    }

    glGenBuffers(1, &vertexElementArrayBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexElementArrayBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*elementArrayBuffer.size(), &(elementArrayBuffer[0]), GL_STATIC_DRAW);

    for (std::vector<DrawCall>::iterator iter = drawCalls.begin();iter != drawCalls.end(); iter++){
        iter->material.initTextures();
    }
    initialized = true;
}

static PerformanceTimer t(20);
void TriangleMesh::render(ShaderProgramDraw &shader){
    check_gl_error();

    if (!initialized){
        build_vertex_array_object();
        return;
    }
    check_gl_error();
    glBindVertexArray(vertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexElementArrayBuffer);

    map_data_to_shader_vertex_attributes(&shader);
   for (std::vector<DrawCall>::iterator iter = drawCalls.begin();iter != drawCalls.end(); iter++){
        Material & mat = iter->material;
        mat.loadUniforms(shader);
        glDrawElements(iter->renderMode, iter->count, GL_UNSIGNED_INT, reinterpret_cast<GLvoid*>( iter->offset));
    }

}


vector<GLuint> TriangleMesh::convert_sub_meshes_to_triangle_indices(DrawCall &drawCall, bool removeDegenerate){
    vector<GLuint> indices = drawCall.indices;
    vector<GLuint> trianleIndices;
    if (drawCall.renderMode == GL_TRIANGLES){
        return indices;
    } else if (drawCall.renderMode != GL_TRIANGLE_STRIP){
        cerr << "Unsupported rendermode for recompute_normals. Only GL_TRIANGLES and GL_TRIANGLE_STRIP supported." << endl;
        return trianleIndices;
    }
    int even = 1;


    trianleIndices.push_back(indices[0]);
    trianleIndices.push_back(indices[1]);
    trianleIndices.push_back(indices[2]);

    for (unsigned int i=3;i<indices.size();i++){
        if (removeDegenerate){
            if (indices[i-1] == indices[i] ||
                indices[i-2] == indices[i] ||
                indices[i-1] == indices[i-2]){
                continue;
            }
        }
        if (i % 2 == even) {
            trianleIndices.push_back(indices[i-1]);
            trianleIndices.push_back(indices[i-2]);
            trianleIndices.push_back(indices[i]);
        } else {
            trianleIndices.push_back(indices[i-2]);
            trianleIndices.push_back(indices[i-1]);
            trianleIndices.push_back(indices[i]);
        }
    }
    return trianleIndices;
}

void TriangleMesh::generateBoundingBox()
{
    vector<Vec3f> vertices = rawData.vertices;

    for(int i = 0; i < vertices.size(); i++)
    {
        Vec3f vertex = vertices[i];
        boundingBox->low[0] = std::min(boundingBox->low[0] , vertex[0]);
        boundingBox->low[1] = std::min(boundingBox->low[1] , vertex[1]);
        boundingBox->low[2] = std::min(boundingBox->low[2] , vertex[2]);

        boundingBox->high[0] = std::max(boundingBox->high[0], vertex[0]);
        boundingBox->high[1] = std::max(boundingBox->high[1], vertex[1]);
        boundingBox->high[2] = std::max(boundingBox->high[2], vertex[2]);
    }
}


void TriangleMesh::recompute_normals(const char* positionName, const char *normalName){
    vector<Vec3f> * normals = new vector<Vec3f>(); //need it to stay in memory
    for (int i=0;i<vertexCount;i++) {
        normals->push_back(Vec3f(0.0, 0.0, 0.0));
    }

    vector<Vec3f> vertex;
    DataVector vertexRaw = vertexAttributes[positionName];
    for (int i=0;i< vertexCount;i++){
        DataUnion data = vertexRaw.vector[i];
        vertex.push_back(Vec3f(data.vec[0], data.vec[1], data.vec[2]));
    }

    if (drawCalls.size()==0 && debug){
        cout << "warn - no draw calls "<< endl;
    }

    for (std::vector<DrawCall>::iterator iter = drawCalls.begin(); iter != drawCalls.end(); iter++){
        DrawCall &drawCall = *iter;
        vector<GLuint> triangles = convert_sub_meshes_to_triangle_indices(drawCall);

        int triangleCount = triangles.size()/3;

        for (int a = 0; a < triangleCount; a++) {
            GLuint i1 = triangles[a * 3];
            GLuint i2 = triangles[a * 3 + 1];
            GLuint i3 = triangles[a * 3 + 2];

            Vec3f v1 = vertex[i1];
            Vec3f v2 = vertex[i2];
            Vec3f v3 = vertex[i3];

            Vec3f v1v2 = normalize(v2 - v1);
            Vec3f v1v3 = normalize(v3 - v1);
            Vec3f v2v3 = normalize(v3 - v2);
            Vec3f normal = normalize(cross(v1v2, v1v3));

            float weight1 = acos(max(-1.0f, min(1.0f, dot(v1v2, v1v3))));
            float weight2 = M_PI - acos(max(-1.0f, min(1.0f, dot(v1v2, v2v3))));

            (*normals)[i1] += normal * weight1;
            (*normals)[i2] += normal * weight2;
            (*normals)[i3] += normal * (M_PI - weight1 - weight2);
        }
    }

    for (int i=0;i<vertexCount;i++) {
        (*normals)[i] = normalize((*normals)[i]);
    }

    rawData.normals = *normals;
    add(normalName, *normals);
}


}
