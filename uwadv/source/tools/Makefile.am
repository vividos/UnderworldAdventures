#
# Makefile.am for Underworld Adventures
#
# $Id$
#

# compiler options
CXXFLAGS = -I.. $(SDL_CFLAGS) $(X_CFLAGS)
LDFLAGS = $(SDL_LIBS) $(X_LIBS)


noinst_PROGRAMS = cnvdasm cnvdbg xmi2mid mapdisp animview strpak

# Makefile.common defines all the _SOURCES
include Makefile.common

cnvdbg_LDADD = $(SDL_LIBS)

cnvdasm_LDADD = $(SDL_LIBS)

xmi2mid_LDADD = $(SDL_LIBS)

mapdisp_LDADD = -lGL -lGLU $(SDL_LIBS)

animview_LDADD = -lGL -lGLU $(SDL_LIBS)

strpak_LDADD = $(SDL_LIBS)


EXTRA_DIST =

