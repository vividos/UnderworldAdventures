#
# Makefile.am for Underworld Adventures
#
# $Id$
#

AUTOMAKE_OPTIONS = foreign

top_srcdir = ../linux
top_builddir = ../linux

SDL_CFLAGS = `sdl-config --cflags`
INCLUDES = -I$(srcdir)/.. $(SDL_CFLAGS)

noinst_LTLIBRARIES = libaudio.la

libaudio_la_SOURCES = audio.cpp midi.cpp xmidi.cpp \
	midi.hpp xmidi.hpp

EXTRA_DIST = 

SUBDIRS = midi_driver

