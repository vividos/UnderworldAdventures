#
# Makefile.am for Underworld Adventures
#
# $Id$
#

# compiler options
CXXFLAGS = @CXXFLAGS@
LDFLAGS = @LIBS@

if BUILD_TOOLS
 noinst_PROGRAMS = uwdump
 uwdump_LDADD = -L.. -luatools
else
 noinst_PROGRAMS =
endif

# Makefile.common defines all the _SOURCES
include Makefile.common


EXTRA_DIST = Makefile.common
