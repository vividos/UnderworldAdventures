#
# Makefile.am for Underworld Adventures tools
#
# $Id$
#

# compiler options
CXXFLAGS = @CXXFLAGS@ -I$(srcdir)/..
LDFLAGS = @LIBS@

if BUILD_TOOLS
noinst_PROGRAMS = cnvdasm cnvdbg xmi2mid mapdisp animview strpak
else
noinst_PROGRAMS =
endif

# Makefile.common defines all the _SOURCES
include Makefile.common

EXTRA_DIST = Makefile.common
