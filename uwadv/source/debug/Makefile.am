#
# Makefile.am for the Underworld Debugger
#
# $Id$
#

# compiler options
CXXFLAGS = @CXXFLAGS@
LDFLAGS = @LIBS@

if BUILD_UADEBUG
 bin_LTLIBRARIES = libuadebug.la
endif

# Makefile.common defines libuadebug_la_SOURCES
include Makefile.common


EXTRA_DIST = \
if BUILD_UADEBUG
else
	$(libuadebug_la_SOURCES) \
endif
	Makefile.common
