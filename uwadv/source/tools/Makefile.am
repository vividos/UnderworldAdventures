#
# Makefile.am for Underworld Adventures tools
#
# $Id$
#

# compiler options
CXXFLAGS = @CXXFLAGS@
LDFLAGS = @LIBS@

if BUILD_TOOLS
 noinst_PROGRAMS = cnvdasm cnvdbg xmi2mid mapdisp animview strpak
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
endif

EXTRA_DIST = Makefile.common
