#
# Makefile.am for Underworld Adventures
#
# $Id$
#

AUTOMAKE_OPTIONS = gnu

top_srcdir = linux
top_builddir = linux

# SDL stuff
SDL_CFLAGS = `sdl-config --cflags`
SDL_LIBS = `sdl-config --libs`

CPPFLAGS = -I.. $(SDL_CFLAGS)
LDFLAGS = $(SDL_LIBS)

bin_PROGRAMS = uwadv

uwadv_SOURCES = font.cpp image.cpp inventory.cpp level.cpp \
	texture.cpp underworld.cpp uwadv.cpp player.cpp

EXTRA_DIST = 

