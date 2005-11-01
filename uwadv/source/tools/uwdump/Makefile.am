#
# Makefile.am for Underworld Adventures tools
#
# $Id$
#

# compiler options
INCLUDES = -I$(srcdir)/../../base -I$(srcdir)/../../import \
  -I$(srcdir)/../../ui \
  -I$(srcdir)/../../renderer -I$(srcdir)/../../underw -I$(srcdir)/../../conv
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
