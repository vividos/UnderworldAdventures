#
# Makefile.am for the Underworld Debugger
#
# $Id$
#

# compiler options
CXXFLAGS = @CXXFLAGS@
LDFLAGS = @LIBS@

if BUILD_UADEBUG
 lib_LTLIBRARIES = libuadebug.la
endif

# Makefile.common defines libuadebug_la_SOURCES
include Makefile.common


if BUILD_UADEBUG
 dist_add =
else
 dist_add = $(libuadebug_la_SOURCES) 
endif

EXTRA_DIST = \
	$(dist_add)
	Makefile.common

