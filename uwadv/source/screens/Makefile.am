#
# Makefile.am for Underworld Adventures
#
# $Id$
#

AUTOMAKE_OPTIONS = gnu

top_srcdir = ../linux
top_builddir = ../linux

SDL_CFLAGS = `sdl-config --cflags`
INCLUDES = -I$(srcdir)/.. $(SDL_CFLAGS)

noinst_LTLIBRARIES = libscreens.la

libscreens_la_SOURCES = ingame_orig.cpp start_splash.cpp \
	ingame_orig.hpp start_splash.hpp

EXTRA_DIST = 

SUBDIRS = 

