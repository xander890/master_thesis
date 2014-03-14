#ifndef TRIANGLEMESH_H
#define TRIANGLEMESH_H

#include <vector>
#include <map>
#include <set>
#include <string>
#include <GLGraphics/GLHeader.h>
#include <CGLA/Vec2f.h>
#include <CGLA/Vec3f.h>
#include <CGLA/Vec4f.h>
#include <Mesh/Material.h>
#include <GLGraphics/ShaderProgram.h>

/// Represents a triangle mesh
/// Can either use indices (internally using glDrawElements) or not (uses glDrawArrays)
///
/// Example usage:
/// void init(){
///    vector<Vec3f> positions;
///    vector<Vec3f> colors;
///
///    positions.push_back(Vec3f(-0.5f, -0.5f, 0.0f));
///    positions.push_back(Vec3f(0.5f, -0.5f, 0.0f));
///    positions.push_back(Vec3f(0.0f,  0.5f, 0.0f));
///
///    colors.push_back(Vec3f(1.0f, 0.0, 0.0));
///    colors.push_back(Vec3f(0.0f, 1.0, 0.0));
///    colors.push_back(Vec3f(0.0f, 0.0, 1.0));
///
///    triangleMesh.add("vertex", positions);
///    triangleMesh.add("color", colors);
///    vector<int> elementIndices;
///    for (int i=0;i<3;i++){ elementIndices.push_back(i);}
///    triangleMesh.add_draw_call(elementIndices, Material(), GL_TRIANGLES);
///    triangleMesh.build_vertex_array_object();
/// }
///
/// void render(){
///    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
///    shader.use();
///    // set shader uniforms
///    triangleMesh.render(shader);
/// }
namespace Mesh {

struct RawMeshData
{
    std::vector<GLuint> indices;
    std::vector<CGLA::Vec3f> vertices;
    std::vector<CGLA::Vec3f> normals;
    std::vector<CGLA::Vec2f> uvs;

};

struct DrawCall {
    std::vector<GLuint> indices;
    Mesh::Material & material;
    GLenum renderMode;
    int count;
    int offset;
};

union DataUnion {
    int i[4];
    float vec[4];
    unsigned char raw[16];
};

struct DataVector {
    int glType; // such as GL_FLOAT or GL_INT
    int size; // in bytes
    int components; //3 for vec3
    std::vector<DataUnion> vector;
};

class TriangleMesh
{
public:
    TriangleMesh();

    /// add vertex attributes
    /// the name must match the vertex attribute name in the shader
    /// and the type of vertex attribute must also match
    void add(const std::string &name, std::vector<int> &vertexAttributes);
    void add(const std::string &name, std::vector<float> &vertexAttributes);
    void add(const std::string &name, std::vector<CGLA::Vec2f> &vertexAttributes);
    void add(const std::string &name, std::vector<CGLA::Vec3f> &vertexAttributes);
    void add(const std::string &name, std::vector<CGLA::Vec4f> &vertexAttributes);

    /// add_draw_call should be used for a single TriangleMesh
    /// renderMode must be: GL_TRIANGLES, GL_TRIANGLE_STRIP, etc
    void add_draw_call(std::vector<GLuint> indices, int count, Mesh::Material &material, GLenum renderMode);

    /// Build the vertex array object
    /// A shader program needs only to be used in case of none generic attribute locations
    void build_vertex_array_object(GLGraphics::ShaderProgram *shader = NULL);

    /// Render the vertex array
    /// Note that the shader must be bound prior to the rendering
    void render(GLGraphics::ShaderProgramDraw &shader);

    void set_debug(bool enable){ debug = enable; }

    bool load(const std::string &filename, Material & material);
    bool load_external(std::vector<GLuint> & indices, std::vector<CGLA::Vec3f>& outPositions, std::vector<CGLA::Vec3f>& outNormal, std::vector<CGLA::Vec2f>& outUv, Material& outMaterial, GLenum type);
    void getRawData(RawMeshData & data);
    GLenum getMode();
    // Calculate normals using angle weighted pseudo-normals
   // void recompute_normals(const char* positionName = "vertex", const char *normalName = "normal");
private:
    /// Make sure that all vertex attribute vectors has the same size
    void check_vertex_size(int vertexAttributeSize);
    // maps shader to vertex attribute location
    // shader is optional - if defined the attribute locations are taken from the shader, otherwise generic locations are used
    void map_data_to_shader_vertex_attributes(GLGraphics::ShaderProgram *shader = NULL);
    //std::vector<GLuint> convert_sub_meshes_to_triangle_indices(DrawCall &drawCall, bool removeDegenerate = true);
    std::map<std::string, DataVector> vertexAttributes;
    std::vector<DrawCall> drawCalls;
    GLuint vertexArrayObject;
    GLuint  vertexBuffer;
    GLuint  vertexElementArrayBuffer;
    int vertexCount;
    bool debug;
    bool initialized;
    std::vector<int> offset;
    std::vector<int> componentType;
    std::vector<int> componentSize; // 1 for float, 2 for Vec2f, etc,
    std::vector<std::string> names;
    int stride;
    RawMeshData rawData;

};
}
#endif // TRIANGLEMESH_H
