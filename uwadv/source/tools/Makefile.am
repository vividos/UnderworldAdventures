#
# Makefile.am for Underworld Adventures
#
# $Id$
#

AUTOMAKE_OPTIONS = foreign

# ugly hack to let the makefile know where configure.in etc. is
top_srcdir = ../linux
top_builddir = ../linux


# SDL stuff
SDL_CFLAGS = `sdl-config --cflags`
SDL_LIBS = `sdl-config --libs` -lSDLmain

CPPFLAGS = -I.. $(SDL_CFLAGS)
LDFLAGS = $(SDL_LIBS) -lGL -lGLU


#
tools: cnvdasm cnvdbg xmi2mid


EXTRA_PROGRAMS = cnvdasm cnvdbg xmi2mid

cnvdasm_SOURCES = cnvdasm.cpp cnvdasm.hpp instr.hpp

cnvdbg_SOURCES = cnvdbg.cpp cnvdbg.hpp instr.hpp

xmi2mid_SOURCES = xmi2mid.cpp

EXTRA_DIST = 

SUBDIRS = 

