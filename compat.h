#ifndef COMPAT_H
#define COMPAT_H 1

#if defined(HAVE_CONFIG_H)
#   include "config.h"
#endif

#include <cstdio>
#include <cstdarg>

#if !defined(_WIN32) && !defined(_WIN64)
#   if defined(HAVE_MATH_H)
#      include <math.h>
#   else
#      error "math.h is missing."
#   endif
#   if defined(HAVE_GL_GL_H)
#      include <GL/gl.h>
#   elif defined(HAVE_OPENGL_GL_H)
#      include <OpenGL/gl.h>
#   endif
#   if defined(HAVE_GL_GLU_H)
#      include <GL/glu.h>
#   elif defined(HAVE_OPENGL_GLU_H)
#      include <OpenGL/glu.h>
#   endif
#endif

#if defined (_WIN32) || defined (_WIN64)
#   include <windows.h> // required by GL headers
#   include <gl/GLU.h>
#endif

#if defined(_WIN32) || defined(_WIN64)
#   ifndef min
#      define min(x, y) ((x) < (y) ? (x) : (y))
#   endif
#   ifndef max
#      define max(x, y) ((x) > (y) ? (x) : (y))
#    endif
#elif defined(HAVE_ALGORITHM)
#   include <algorithm>
    using std::min;
    using std::max;
#else
#   error "<algorithm> is missing."
#endif

#if defined(_WIN32) || defined(_WIN64)
#   define isnan(x) (((x) != (x)) || (x > 1e11) || (x < -1e11))
#endif

#if defined(_WIN32) || defined(_WIN64)
# define GLX_CALLBACK __stdcall
#else
# define GLX_CALLBACK
#endif

#endif /* COMPAT_H */
