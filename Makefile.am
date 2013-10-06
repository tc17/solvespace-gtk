ACLOCAL_AMFLAGS = -I m4

SUBDIRS = tools
bin_PROGRAMS = solvespace

AM_CXXFLAGS = -Wall -Wextra -Wno-unused-parameter

solvespace_SOURCES = bsp.cpp clipboard.cpp confscreen.cpp constraint.cpp constrainteq.cpp \
	describescreen.cpp drawconstraint.cpp draw.cpp drawentity.cpp entity.cpp export.cpp \
	exportstep.cpp exportvector.cpp expr.cpp file.cpp generate.cpp glhelper.cpp graphicswin.cpp \
	group.cpp groupmesh.cpp mesh.cpp modify.cpp mouse.cpp polygon.cpp request.cpp solvespace.cpp \
	style.cpp system.cpp textscreens.cpp textwin.cpp toolbar.cpp ttf.cpp undoredo.cpp util.cpp view.cpp \
	dsc.h expr.h inttypes.h polygon.h sketch.h solvespace.h ui.h

solvespace_SOURCES += srf/boolean.cpp srf/curve.cpp srf/merge.cpp srf/ratpoly.cpp srf/raycast.cpp srf/surface.cpp \
		      srf/surface.h srf/surfinter.cpp srf/triangulate.cpp

solvespace_SOURCES += nihstring.h nihstring.cpp

nodist_solvespace_SOURCES = icons.h icons-proto.h

AM_CXXFLAGS += $(GL_CFLAGS) $(GLU_CFLAGS) $(PNG_CFLAGS) $(FONTCONFIG_CFLAGS) $(GTKMM_CFLAGS)
solvespace_LDADD = $(GL_LIBS) $(GLU_LIBS) $(PNG_LIBS) $(FONTCONFIG_LIBS) $(GTKMM_LIBS)

#if HAVE_GTKMM
solvespace_SOURCES += gtk/unixutil.cpp gtk/config.cpp gtk/ui.cpp gtk/ui.h gtk/aux_ui.cpp gtk/aux_ui.h \
		      gtk/popup_menu_ui.cpp gtk/popup_menu_ui.h gtk/float_window_ui.cpp gtk/float_window_ui.h \
		      gtk/fcutil.h gtk/fcutil.cpp gtk/main.cpp
#endif


CLEANFILES = icons.h icons-proto.h

#manual deps
textwin.$(OBJEXT): icons-proto.h

toolbar.$(OBJEXT): icons.h

icons-proto.h icons.h: $(srcdir)/icons $(srcdir)/png2c.pl
	perl $(srcdir)/png2c.pl $(srcdir)/icons icons.h icons-proto.h
