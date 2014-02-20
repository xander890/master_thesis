#ifndef GL_GRAPHICS_GLHEADER_H
#define GL_GRAPHICS_GLHEADER_H

#include <iostream>
#include <string>
#include <QtGlobal>
#if defined(Q_OS_WIN32) || defined(Q_OS_WIN64) || defined(Q_OS_WIN)
#   define GLEW_STATIC
#   include <GL/glew.h>
#elif defined(Q_OS_MAC) || defined(Q_OS_DARWIN)
#  include <OpenGL/gl3.h>
#  define __gl_h_
#else // assume OpenGL ES 2.0
#  include <GLES2/gl2.h>
#endif
#include <QtDebug>
#include "Core3_2_context.h"

void setup_gl();

void _check_gl_error(const char *file, int line);

///
/// Usage
/// [... some opengl calls]
/// glCheckError();
///
#define check_gl_error() _check_gl_error(__FILE__,__LINE__)

#endif // GL_GRAPHICS_GLHEADER_H
