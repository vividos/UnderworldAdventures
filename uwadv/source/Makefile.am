#
# Makefile.am for Underworld Adventures
#
# $Id$
#

AUTOMAKE_OPTIONS = foreign

# ugly hack to let the makefile know where configure.in etc. is
top_srcdir = linux
top_builddir = linux


# compiler options
CXXFLAGS = -I.. $(SDL_CFLAGS)
LDFLAGS = $(SDL_LIBS)


# automake build rules

noinst_LIBRARIES = liblua.a

liblua_a_SOURCES = lua/src/lapi.c lua/src/lcode.c lua/src/ldebug.c \
	lua/src/ldo.c lua/src/lfunc.c lua/src/lgc.c \
	lua/src/llex.c lua/src/lmem.c lua/src/lobject.c \
	lua/src/lparser.c lua/src/lstate.c lua/src/lstring.c \
	lua/src/ltable.c lua/src/ltests.c lua/src/ltm.c \
	lua/src/lundump.c lua/src/lvm.c lua/src/lzio.c

liblua_a_CFLAGS = -Ilua/include


bin_PROGRAMS = uwadv

uwadv_SOURCES = font.cpp image.cpp inventory.cpp level.cpp \
	player.cpp quadtree.cpp texture.cpp underworld.cpp uwadv.cpp \
	objects.cpp cutscene.cpp physics.cpp \
	audio.hpp common.hpp core.hpp font.hpp gamestrings.hpp \
	image.hpp inventory.hpp level.hpp player.hpp quadtree.hpp \
	screen.hpp settings.hpp texture.hpp uatypes.hpp \
	underworld.hpp uwadv.hpp objects.hpp cutscene.hpp physics.hpp \
	linux/main.cpp \
	audio/audio.cpp audio/midi.cpp audio/xmidi.cpp \
	audio/midi.hpp audio/xmidi.hpp \
	audio/midi_driver/uni_fmod.cpp audio/midi_driver/uni_fmod.hpp \
	conv/codevm.cpp conv/codevm.hpp \
	resource/codeloader.cpp resource/fontloader.cpp \
	resource/gamestrings.cpp resource/imageloader.cpp \
	resource/maploader.cpp resource/settings.cpp \
	resource/texloader.cpp resource/objloader.cpp \
	resource/cutsloader.cpp \
	resource/fread_endian.hpp \
	screens/ingame_orig.cpp screens/start_splash.cpp \
	screens/start_menu.cpp \
	screens/ingame_orig.hpp screens/start_splash.hpp \
	screens/start_menu.hpp

uwadv_LDADD = -lGL -lGLU $(SDL_LIBS) liblua.a

uwadv_DEPENDENCIES = liblua.a

EXTRA_DIST = audio/midi_driver/win_midiout.cpp \
	audio/midi_driver/win_midiout.hpp \
	linux/uwadv.cfg

SUBDIRS = tools

