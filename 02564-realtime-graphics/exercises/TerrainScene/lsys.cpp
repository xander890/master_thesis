//
//  L-Systems example (D0L Systems as described in Prusinkiewicz et al.
//  "L-systems: from the Theory to Visual Models of Plants"
//  Created by J. Andreas Bærentzen on 08/02/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <cmath>
#include <CGLA/Vec3f.h>
#include <CGLA/Mat4x4f.h>
#include <GLGraphics/ShaderProgram.h>

#define SOLUTION_CODE

#include "lsys.h"

using namespace std;
using namespace CGLA;
using namespace GLGraphics;

// Produce the mesh for a truncated cone
#ifdef SOLUTION_CODE
void truncated_cone(const Mat4x4f& m, float l, float w0, float w1, vector<Vec3f>& strip)
{
    float len = sqrt(l*l + sqr(w0-w1));
    float a = l/len;
    float b = (w0-w1)/len;
    const int N = 10;
    for(int i=0;i<=N;++i)
    {
        float alpha = 2.0*M_PI*float(i)/N;
        Vec3f p0 = m.mul_3D_point(Vec3f(w0*cos(alpha), w0*sin(alpha), 0));
        Vec3f p1 = m.mul_3D_point(Vec3f(w1*cos(alpha), w1*sin(alpha), l));
        Vec3f n = m.mul_3D_vector(a*Vec3f(cos(alpha), sin(alpha), b));
        if(i==0){
            strip.push_back(n);
            strip.push_back(p0);
        }
        strip.push_back(n);
        strip.push_back(p0);
        strip.push_back(n);
        strip.push_back(p1);
        if(i==N){
            strip.push_back(n);
            strip.push_back(p1);
        }
    }
}
#else

// Produce the mesh for a truncated cone
void truncated_cone(const Mat4x4f& m,           // transformation matrix used for the points
                    float l,                    // length of truncated cone
                    float w0,                   // width at base
                    float w1,                   // width at top
                    vector<Vec3f>& triangles,   // triangles (out)
                    vector<Vec3f>& normals)     // normals (out)
{
    float len = sqrt(l*l + sqr(w0-w1));
    float a = l/len;
    float b = (w0-w1)/len;
    const int N = 10;
    for(int i=0;i<=N;++i)
    {
        float alpha = 2.0*M_PI*float(i)/N;
        Vec3f p0 = m.mul_3D_point(Vec3f(w0*cos(alpha), w0*sin(alpha), 0));
        Vec3f p1 = m.mul_3D_point(Vec3f(w1*cos(alpha), w1*sin(alpha), l));
        Vec3f n0 = m.mul_3D_vector(a*Vec3f(cos(alpha), sin(alpha), b));
        Vec3f n1 = n0;

        alpha = 2.0*M_PI*float(i+1)/N;
        Vec3f p2 = m.mul_3D_point(Vec3f(w0*cos(alpha), w0*sin(alpha), 0));
        Vec3f p3 = m.mul_3D_point(Vec3f(w1*cos(alpha), w1*sin(alpha), l));
        Vec3f n2 = m.mul_3D_vector(a*Vec3f(cos(alpha), sin(alpha), b));
        Vec3f n3 = n2;

        normals.push_back(n0);
        triangles.push_back(p0);

        normals.push_back(n1);
        triangles.push_back(p1);

        normals.push_back(n3);
        triangles.push_back(p3);

        normals.push_back(n3);
        triangles.push_back(p3);

        normals.push_back(n2);
        triangles.push_back(p2);

        normals.push_back(n0);
        triangles.push_back(p0);
    }
}
#endif

// Symbols used for L-Systems
enum LSSymbol { LS_TURN = '+', LS_ROLL = '/', LS_WIDTH = '!', LS_A = 'A',
                LS_LEFT_BRACKET = '[', LS_RIGHT_BRACKET = ']', LS_DRAW = 'F'};

// LSElement is an LSSymbol and associated parameters.
struct LSElement
{
    LSSymbol symbol;
    double datum1, datum2;
    int data;

    LSElement(LSSymbol _symbol): symbol(_symbol), data(0) {}
    LSElement(LSSymbol _symbol, double _datum1): symbol(_symbol), datum1(_datum1), data(1) {}
    LSElement(LSSymbol _symbol, double _datum1, double _datum2): symbol(_symbol), datum1(_datum1), datum2(_datum2), data(2) {}

    void print(ostream& os)
    {
        os << static_cast<char>(symbol);
        if(data>0)
        {
            os << "(" << datum1;
            if(data>1)
                os << "," << datum2;
            os << ")";
        }
    }
};

// This class represents a single rule with some parameters that can be tuned to
// create specific structures. See Fig. 8 and Table 1 in Prusinkiewicz's paper.
class Rule
{
    float alpha1, alpha2, phi1, phi2, r1, r2, q, e, smin;

public:
    Rule(float _alpha1, float _alpha2, float _phi1, float _phi2,
         float _r1, float _r2, float _q, float _e, float _smin):
        alpha1(_alpha1), alpha2(_alpha2), phi1(_phi1), phi2(_phi2),
        r1(_r1), r2(_r2), q(_q), e(_e), smin(_smin) {}

    bool apply(const LSElement elem, vector<LSElement>& out)
    {
        if(elem.symbol == LS_A)
        {
            float s = elem.datum1;
            float w = elem.datum2;

            if(s>=smin)
            {
#ifdef SOLUTION_CODE
                out.push_back(LSElement(LS_WIDTH, w));
                out.push_back(LSElement(LS_DRAW, s));
                out.push_back(LSElement(LS_LEFT_BRACKET));
                out.push_back(LSElement(LS_TURN,alpha1));
                out.push_back(LSElement(LS_ROLL,phi1));
                out.push_back(LSElement(LS_A, s*r1, w*pow(q,e)));
                out.push_back(LSElement(LS_RIGHT_BRACKET));
                out.push_back(LSElement(LS_LEFT_BRACKET));
                out.push_back(LSElement(LS_TURN,alpha2));
                out.push_back(LSElement(LS_ROLL,phi2));
                out.push_back(LSElement(LS_A, s*r2, w*pow(1.0f-q,e)));
                out.push_back(LSElement(LS_RIGHT_BRACKET));
                return true;
#else
                out.push_back(LSElement(LS_DRAW, s));
                out.push_back(LSElement(LS_A, s*r1, w*pow(q,e)));
                return true;
#endif
            }
        }
        else out.push_back(elem);

        return false;
    }
};

// The state used for turtle graphics
struct TurtleState
{
    float w;
    Mat4x4f M;
    TurtleState(float w0, const Mat4x4f& M0): w(w0), M(M0) {}
};

// The turtle. Contains state and functions for crawling in 3D.
class Turtle
{
    TurtleState turtle_state;
    stack<TurtleState> tss;

public:
    Turtle(float w0): turtle_state(w0, identity_Mat4x4f()) {}
#ifdef SOLUTION_CODE
    void turn(float angle) {turtle_state.M = turtle_state.M * rotation_Mat4x4f(YAXIS, angle*M_PI/180.0);}
    void roll(float angle) {turtle_state.M = turtle_state.M * rotation_Mat4x4f(ZAXIS, angle*M_PI/180.0);}
    void move(float dist) {turtle_state.M = turtle_state.M * translation_Mat4x4f(Vec3f(0,0,dist));}
    void push() {
        tss.push(turtle_state);
    }
    void pop() {
        turtle_state = tss.top();
        tss.pop();
    }
    void set_width(float w) {turtle_state.w = w;}
#else
    void turn(float angle) {/* implement me */}
    void roll(float angle) {/* implement me */}
    void move(float dist) {/* implement me */}
    void push() {/* implement me */}
    void pop() {/* implement me */}
    void set_width(float w) {/* implement me */}
#endif

    float get_width() const {return turtle_state.w;}
    const Mat4x4f& get_transform() const {return turtle_state.M;}
};

// This function uses the turtle graphics approach to generate a skeletal representation of the tree.
#ifdef SOLUTION_CODE
void interpret(vector<LSElement> str, float w0,
               vector<Vec3f>& strip)
#else
void interpret(vector<LSElement> str, float w0,
               vector<Vec3f>& triangles, vector<Vec3f>& normals)
#endif
{
    Turtle turtle(w0);
    for(int i=0;i<str.size(); ++i)
    {
        LSElement elem = str[i];
        switch(elem.symbol)
        {
#ifdef SOLUTION_CODE
        case LS_DRAW:
            truncated_cone(turtle.get_transform(), elem.datum1,
                           turtle.get_width(), turtle.get_width()*0.75, strip);
            turtle.move(elem.datum1);
            break;
        case LS_TURN:
            turtle.turn(elem.datum1);
            break;
        case LS_ROLL:
            turtle.roll(elem.datum1);
            break;
        case LS_LEFT_BRACKET:
            turtle.push();
            break;
        case LS_RIGHT_BRACKET:
            turtle.pop();
            break;
        case LS_WIDTH:
            turtle.set_width(elem.datum1);
            break;
#else
        case LS_DRAW:
            truncated_cone(turtle.get_transform(), elem.datum1,
                           turtle.get_width(), turtle.get_width()*0.75,
                           triangles, normals);
            turtle.move(elem.datum1);
            break;
            // Implement the other cases
#endif
        }
    }
}

#ifdef SOLUTION_CODE
GLuint create_vertex_array_object(vector<Vec3f>& strip)
{
    GLuint VAO, VBO;
    GLuint vert_attrib = ShaderProgramDraw::get_generic_attrib_location("vertex");
    GLuint norm_attrib = ShaderProgramDraw::get_generic_attrib_location("normal");
    GLuint texcoord_attrib = ShaderProgramDraw::get_generic_attrib_location("texcoord");
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,strip.size()*sizeof(Vec3f),&strip[0],GL_STATIC_DRAW);
    glVertexAttribPointer(norm_attrib, 3, GL_FLOAT, GL_FALSE, 2*sizeof(Vec3f), 0);
    glVertexAttribPointer(vert_attrib, 3, GL_FLOAT, GL_FALSE, 2*sizeof(Vec3f), (const GLvoid*)sizeof(Vec3f));
    glVertexAttrib3f(texcoord_attrib, 0, 0,0);
    glEnableVertexAttribArray(vert_attrib);
    glEnableVertexAttribArray(norm_attrib);
    return VAO;
}
#else
// Create an OpenGL vertex array from a vector of triangles and normals
GLuint create_vertex_array_object(vector<Vec3f>& triangles, vector<Vec3f>& normals)
{
    GLuint VAO, VBO[2];
    GLuint vert_attrib = ShaderProgramDraw::get_generic_attrib_location("vertex");
    GLuint norm_attrib = ShaderProgramDraw::get_generic_attrib_location("normal");
    GLuint texcoord_attrib = ShaderProgramDraw::get_generic_attrib_location("texcoord");
    glGenVertexArrays(1, &VAO);
    glGenBuffers(2, &VBO[0]);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER,triangles.size()*sizeof(Vec3f),&triangles[0],GL_STATIC_DRAW);
    glVertexAttribPointer(vert_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER,normals.size()*sizeof(Vec3f),&normals[0],GL_STATIC_DRAW);
    glVertexAttribPointer(norm_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttrib3f(texcoord_attrib, 0, 0,0);
    glEnableVertexAttribArray(vert_attrib);
    glEnableVertexAttribArray(norm_attrib);
    return VAO;
}
#endif

// make_tree ties it all together producing a vertex array object with the geometry of a tree
// model created using L-Systems.
GLuint make_tree (GLint& count)
{
    float w0 = 20;                  // Initial branch width
    vector<LSElement> str;           // Initial string
    str.push_back(LSElement(LS_A, 100, w0)); // Push the initial symbol

#ifdef SOLUTION_CODE
    // Rules for three trees from Fig. 8 in Prusinkiewicz's paper
    Rule g(30,-30,137,137,0.8f,0.8f,0.5f,0.5f,0.0f);
    Rule i(-5, 30,137,137,0.55f,0.95f,0.4f,0.0f,5.0f);
    Rule d(25, -15,137,137,0.6f,0.85f,0.45f,0.5f,0.0f);
    Rule ex = g;
#else
    // Rules for a coarse example tree
    Rule ex(40,-40,90,-90,0.5f,0.5f,0.5f,0.5f,0.5f);
#endif

    // Now apply the rule a number of times.
    for(int iter=0;iter<8;++iter)
    {
        vector<LSElement> out_str;
        for(int e=0;e<str.size(); ++e)
            ex.apply(str[e], out_str);
        str = out_str;
    }

#ifdef SOLUTION_CODE
    vector<Vec3f> strip;
    interpret(str, w0, strip);
    count = strip.size()/2;
    return create_vertex_array_object(strip);
#else
    // Interpret the string producing a vector of triangles and normals
    vector<Vec3f> triangles, normals;
    interpret(str, w0, triangles, normals);

    // Generate a vertex array object and return that.
    count = triangles.size();
    return create_vertex_array_object(triangles, normals);
#endif
}
