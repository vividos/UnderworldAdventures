#
# Makefile.am for Underworld Adventures
#
# $Id$
#

AUTOMAKE_OPTIONS = gnu

top_srcdir = ../../linux
top_builddir = ../../linux

SDL_CFLAGS = `sdl-config --cflags`
INCLUDES = -I$(srcdir)/.. $(SDL_CFLAGS)

noinst_LTLIBRARIES = libmidi_driver.la

libmidi_driver_la_SOURCES = uni_fmod.cpp

noinst_HEADERS = uni_fmod.h

EXTRA_DIST = win_midiout.h win_midiout.cpp

SUBDIRS = 

