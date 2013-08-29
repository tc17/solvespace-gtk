ACLOCAL_AMFLAGS = -I m4

SUBDIRS = tools
bin_PROGRAMS = solverspace

solverspace_SOURCES = bsp.cpp clipboard.cpp confscreen.cpp constraint.cpp constrainteq.cpp \
	describescreen.cpp drawconstraint.cpp draw.cpp drawentity.cpp entity.cpp export.cpp \
	exportstep.cpp exportvector.cpp expr.cpp file.cpp generate.cpp glhelper.cpp graphicswin.cpp \
	group.cpp groupmesh.cpp mesh.cpp modify.cpp mouse.cpp polygon.cpp request.cpp solvespace.cpp \
	style.cpp system.cpp textscreens.cpp textwin.cpp toolbar.cpp ttf.cpp undoredo.cpp util.cpp view.cpp \
	dsc.h expr.h inttypes.h polygon.h sketch.h solvespace.h ui.h

nodist_solverspace_SOURCES = icons.h icons-proto.h

solverspace_CFLAGS = $(GL_CFLAGS) $(GLU_CFLAGS)
solverspace_LDADD = $(GL_LIBS) $(GLU_LIBS)

if HAVE_GTKMM
solverspace_SOURCES += gtk/unixutil.cpp gtk/config.cpp
endif

AM_CXXFLAGS = -Wall -Wextra -Wno-unused-parameter

CLEANFILES = icons.h icons-proto.h

#manual deps
textwin.$(OBJEXT): icons-proto.h

toolbar.$(OBJEXT): icons.h

icons-proto.h icons.h: $(srcdir)/icons $(srcdir)/png2c.pl
	perl $(srcdir)/png2c.pl $(srcdir)/icons icons.h icons-proto.h
