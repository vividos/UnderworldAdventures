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
SDL_LIBS = `sdl-config --libs`

CPPFLAGS = -I.. $(SDL_CFLAGS)
LDFLAGS = $(SDL_LIBS)


# tools build rule
tools: cnvdasm cnvdbg xmi2mid mapdisp


# automake rules
EXTRA_PROGRAMS = cnvdasm cnvdbg xmi2mid mapdisp

cnvdasm_SOURCES = cnvdasm.cpp cnvdasm.hpp instr.hpp \
	../conv/codevm.cpp ../resource/codeloader.cpp \
	../resource/gamestrings.cpp

cnvdbg_SOURCES = cnvdbg.cpp cnvdbg.hpp instr.hpp \
	../conv/codevm.cpp ../resource/codeloader.cpp \
	../resource/gamestrings.cpp

xmi2mid_SOURCES = xmi2mid.cpp ../audio/xmidi.cpp

mapdisp_SOURCES = mapdisp.cpp ../level.cpp ../resource/maploader.cpp \
	../quadtree.cpp ../resource/texloader.cpp ../texture.cpp

mapdisp_LDADD = -lGL -lGLU

EXTRA_DIST = 

SUBDIRS = 

