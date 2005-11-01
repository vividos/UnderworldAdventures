#
# Makefile.am for Underworld Adventures tools
#
# $Id$
#

# compiler options
INCLUDES = -I$(srcdir)/../base -I$(srcdir)/../import \
  -I$(srcdir)/../lua/include -I$(srcdir)/../ui -I$(srcdir)/../audio \
  -I$(srcdir)/../renderer -I$(srcdir)/../underw -I$(srcdir)/../conv
CXXFLAGS = @CXXFLAGS@
LDFLAGS = @LIBS@

SUBDIRS = . uwdump

if BUILD_TOOLS
 noinst_PROGRAMS = cnvdasm cnvdbg xmi2mid mapdisp animview strpak uwmdlview
 noinst_LTLIBRARIES = libuatools.la
else
 noinst_PROGRAMS =
 noinst_LTLIBRARIES =
endif

# Makefile.common defines all the _SOURCES
include Makefile.common

if BUILD_TOOLS
 cnvdasm_LDADD = -luatools
 cnvdbg_LDADD = -luatools
 mapdisp_LDADD = -luatools
 animview_LDADD = -luatools
 strpak_LDADD = -luatools
 uwmdlview_LDADD = -luatools
endif

EXTRA_DIST = Makefile.common \
	map3ds.cpp \
	wrlview.cpp
