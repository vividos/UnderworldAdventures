#
# Makefile.am for Underworld Adventures
#
# $Id$
#

AUTOMAKE_OPTIONS = gnu

# ugly hack to let the makefile know where configure.in etc. is
top_srcdir = linux
top_builddir = linux

# SDL stuff
SDL_CFLAGS = `sdl-config --cflags`
SDL_LIBS = `sdl-config --libs`

CPPFLAGS = -I.. $(SDL_CFLAGS)
LDFLAGS = $(SDL_LIBS)

bin_PROGRAMS = uwadv

uwadv_SOURCES = font.cpp image.cpp inventory.cpp level.cpp \
	player.cpp quadtree.cpp texture.cpp underworld.cpp uwadv.cpp \
	audio.hpp common.hpp core.hpp font.hpp gamestrings.hpp \
	image.hpp inventory.hpp level.hpp player.hpp quadtree.hpp \
	screen.hpp settings.hpp texture.hpp uatypes.hpp \
	underworld.hpp uwadv.hpp

UWADVLIBS = audio/libaudio.la conv/libconv.la \
	resource/libresource.la screens/libscreens.la

uwadv_LDADD = $(UWADVLIBS)

uwadv_DEPENDENCIES = $(UWADVLIBS)

EXTRA_DIST = 

#SUBDIRS = audio conv resource screens
SUBDIRS = conv

