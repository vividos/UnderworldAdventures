#
# Makefile.am for Underworld Adventures
#
# $Id$
#

AUTOMAKE_OPTIONS = foreign

# ugly hack to let the makefile know where configure.in etc. is
top_srcdir = linux
top_builddir = linux

# SDL stuff
SDL_CFLAGS = `sdl-config --cflags`
SDL_LIBS = `sdl-config --libs` -lSDLmain

CPPFLAGS = -I.. $(SDL_CFLAGS)
LDFLAGS = $(SDL_LIBS) -lGL -lGLU

bin_PROGRAMS = uwadv

uwadv_SOURCES = font.cpp image.cpp inventory.cpp level.cpp \
	player.cpp quadtree.cpp texture.cpp underworld.cpp uwadv.cpp \
	audio.hpp common.hpp core.hpp font.hpp gamestrings.hpp \
	image.hpp inventory.hpp level.hpp player.hpp quadtree.hpp \
	screen.hpp settings.hpp texture.hpp uatypes.hpp \
	underworld.hpp uwadv.hpp \
	linux/main.cpp \
	audio/audio.cpp audio/midi.cpp audio/xmidi.cpp \
	audio/midi.hpp audio/xmidi.hpp \
	audio/midi_driver/uni_fmod.cpp audio/midi_driver/uni_fmod.hpp \
	conv/codevm.cpp conv/codevm.hpp \
	resource/codeloader.cpp resource/fontloader.cpp \
	resource/gamestrings.cpp resource/imageloader.cpp \
	resource/maploader.cpp resource/settings.cpp \
	resource/texloader.cpp \
	resource/fread_endian.hpp \
	screens/ingame_orig.cpp screens/start_splash.cpp \
	screens/ingame_orig.hpp screens/start_splash.hpp

EXTRA_DIST = audio/midi_driver/win_midiout.cpp \
	audio/midi_driver/win_midiout.hpp \
	linux/uwadv.cfg

