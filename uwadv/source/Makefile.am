#
# Makefile.am for Underworld Adventures
#
# $Id$
#

AUTOMAKE_OPTIONS = gnu

top_srcdir = linux
top_builddir = linux

# SDL stuff
SDL_CFLAGS = `sdl-config --cflags'

CPPFLAGS = -I.. $(SDL_CFLAGS)
LDFLAGS = `sdl-config --libs'

bin_PROGRAMS = uwadv

uwadv_SOURCES = uwadv.cpp

EXTRA_DIST = 

