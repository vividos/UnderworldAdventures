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

noinst_LTLIBRARIES = libresource.la

libresource_la_SOURCES = codeloader.cpp fontloader.cpp \
	gamestrings.cpp imageloader.cpp maploader.cpp \
	settings.cpp texloader.cpp \
	fread_endian.hpp

EXTRA_DIST = 

SUBDIRS = 

