#
# Makefile.am for Underworld Adventures
#
# $Id$
#

# compiler options
CXXFLAGS = -I.. $(SDL_CFLAGS) $(X_CFLAGS)
LDFLAGS = $(SDL_LIBS) $(X_LIBS)


noinst_PROGRAMS = cnvdasm cnvdbg xmi2mid mapdisp animview strpak


cnvdasm_SOURCES = cnvdasm.cpp cnvdasm.hpp instr.hpp \
	../conv/codevm.cpp ../resource/codeloader.cpp \
	../resource/gamestrings.cpp ../settings.cpp \
	cnvdasm.hpp instr.hpp ../conv/codevm.hpp

cnvdasm_LDADD = $(SDL_LIBS)


cnvdbg_SOURCES = cnvdbg.cpp cnvdbg.hpp instr.hpp \
	../conv/codevm.cpp ../resource/codeloader.cpp \
	../resource/gamestrings.cpp ../settings.cpp \
	cnvdbg.hpp instr.hpp ../conv/codevm.hpp

cnvdbg_LDADD = $(SDL_LIBS)


xmi2mid_SOURCES = xmi2mid.cpp ../audio/xmidi.cpp ../audio/xmidi.hpp

xmi2mid_LDADD = $(SDL_LIBS)


mapdisp_SOURCES = mapdisp.cpp ../level.cpp ../resource/maploader.cpp \
	../quadtree.cpp ../resource/texloader.cpp ../texture.cpp \
	../resource/imageloader.cpp ../resource/objloader.cpp \
	../image.cpp ../objects.cpp ../resource/codeloader.cpp \
	../settings.cpp \
	../common.hpp ../resource/fread_endian.hpp ../image.hpp \
	../level.hpp ../objects.hpp ../quadtree.hpp ../settings.hpp \
	../texture.hpp ../uatypes.hpp

mapdisp_LDADD = -lGL -lGLU $(SDL_LIBS)


animview_SOURCES = animview.cpp ../cutscene.cpp ../resource/cutsloader.cpp \
	../image.cpp ../texture.cpp ../settings.cpp \
	../image.hpp ../texture.hpp \
	../common.hpp ../cutscene.hpp ../settings.hpp ../uatypes.hpp \
	../resource/fread_endian.hpp

animview_LDADD = -lGL -lGLU $(SDL_LIBS)


strpak_SOURCES = strpak.cpp ../resource/gamestrings.cpp ../settings.cpp \
	../common.hpp ../resource/fread_endian.hpp ../gamestrings.hpp \
	../settings.hpp ../uatypes.hpp

strpak_LDADD = $(SDL_LIBS)


EXTRA_DIST =

