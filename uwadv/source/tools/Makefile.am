#
# Makefile.am for Underworld Adventures
#
# $Id$
#

AUTOMAKE_OPTIONS = foreign

# ugly hack to let the makefile know where configure.in etc. is
top_srcdir = ../linux
top_builddir = ../linux


# compiler options
CXXFLAGS = -I.. $(SDL_CFLAGS)
LDFLAGS = $(SDL_LIBS)


# tools build rule
TOOLS = cnvdasm cnvdbg xmi2mid mapdisp animview
tools: $(TOOLS)


# automake build rules

EXTRA_PROGRAMS = $(TOOLS)

CLEANFILES = $(TOOLS)


cnvdasm_SOURCES = cnvdasm.cpp cnvdasm.hpp instr.hpp \
	../conv/codevm.cpp ../resource/codeloader.cpp \
	../resource/gamestrings.cpp \
	cnvdasm.hpp instr.hpp ../conv/codevm.hpp

cnvdbg_SOURCES = cnvdbg.cpp cnvdbg.hpp instr.hpp \
	../conv/codevm.cpp ../resource/codeloader.cpp \
	../resource/gamestrings.cpp \
	cnvdbg.hpp instr.hpp ../conv/codevm.hpp

xmi2mid_SOURCES = xmi2mid.cpp ../audio/xmidi.cpp ../audio/xmidi.hpp

mapdisp_SOURCES = mapdisp.cpp ../level.cpp ../resource/maploader.cpp \
	../quadtree.cpp ../resource/texloader.cpp ../texture.cpp \
	../resource/imageloader.cpp ../resource/objloader.cpp \
	../image.cpp ../objects.cpp \
	../common.hpp ../resource/fread_endian.hpp ../image.hpp \
	../level.hpp ../objects.hpp ../quadtree.hpp ../settings.hpp \
	../texture.hpp ../uatypes.hpp

mapdisp_LDADD = -lGL -lGLU

animview_SOURCES = animview.cpp ../cutscene.cpp ../resource/cutsloader.cpp \
	../common.hpp ../cutscene.hpp ../settings.hpp ../uatypes.hpp \
	../resource/fread_endian.hpp

EXTRA_DIST =

