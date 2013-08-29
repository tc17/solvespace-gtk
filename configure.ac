#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([SolverSpace], [VERSION], [BUG-REPORT-ADDRESS])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([groupmesh.cpp])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_LANG([C++])

# Checks for libraries.
AX_CHECK_GL
AS_IF([test "x$no_gl" == "xyes"], [AC_MSG_ERROR([OpenGL is missing.])])
AX_CHECK_GLU
AS_IF([test "x$no_glu" == "xyes"], [AC_MSG_ERROR([GLU is missing.])])

PKG_CHECK_MODULES([PNG], [libpng])

# Checks for header files.
AC_CHECK_HEADERS([malloc.h stdlib.h string.h strings.h unistd.h limits.h algorithm])
AC_CHECK_HEADERS([string], [], [AC_MSG_ERROR([<string> is missing.])])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([floor memmove memset pow sqrt strchr strrchr])

AC_CONFIG_FILES([Makefile
                 tools/Makefile])
AC_OUTPUT
