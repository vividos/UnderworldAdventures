#
# Makefile.am for Underworld Adventures
#
# $Id$
#

# compiler options
CXXFLAGS = -I.. -I$(srcdir)/.. $(SDL_CFLAGS) $(X_CFLAGS)
LDFLAGS = $(SDL_LIBS) $(X_LIBS)

if BUILD_TOOLS
noinst_PROGRAMS = cnvdasm cnvdbg xmi2mid mapdisp animview strpak
else
noinst_PROGRAMS =
endif

# Makefile.common defines all the _SOURCES
include Makefile.common

cnvdbg_LDADD = $(SDL_LIBS) -lz

cnvdasm_LDADD = $(SDL_LIBS) -lz

xmi2mid_LDADD = $(SDL_LIBS)

mapdisp_LDADD = -lGL -lGLU $(SDL_LIBS)

animview_LDADD = -lGL -lGLU $(SDL_LIBS)

strpak_LDADD = $(SDL_LIBS)


EXTRA_DIST = \
	Makefile.common

