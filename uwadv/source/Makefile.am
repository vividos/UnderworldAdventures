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
CXXFLAGS = -I.. $(SDL_CFLAGS) -Wall
LDFLAGS = $(SDL_LIBS) -lSDL_mixer


# automake build rules

noinst_LIBRARIES = liblua.a libzziplib.a


liblua_a_SOURCES = lua/src/lapi.c lua/src/lcode.c lua/src/ldebug.c \
	lua/src/ldo.c lua/src/lfunc.c lua/src/lgc.c \
	lua/src/llex.c lua/src/lmem.c lua/src/lobject.c \
	lua/src/lparser.c lua/src/lstate.c lua/src/lstring.c \
	lua/src/ltable.c lua/src/ltests.c lua/src/ltm.c \
	lua/src/lundump.c lua/src/lvm.c lua/src/lzio.c

liblua_a_CFLAGS = -Ilua/include -Wall


libzziplib_a_SOURCES = resource/zziplib/SDL_rwops_zzip.c \
	resource/zziplib/zzip-dir.c \
	resource/zziplib/zzip-err.c \
	resource/zziplib/zzip-file.c \
	resource/zziplib/zzip-info.c \
	resource/zziplib/zzip-io.c \
	resource/zziplib/zzip-stat.c \
	resource/zziplib/zzip-zip.c

libzziplib_a_CFLAGS = -Iresource/zziplib $(SDL_CFLAGS) -Wall


bin_PROGRAMS = uwadv

uwadv_SOURCES = font.cpp image.cpp inventory.cpp level.cpp \
	player.cpp quadtree.cpp texture.cpp underworld.cpp uwadv.cpp \
	objects.cpp cutscene.cpp physics.cpp files.cpp keymap.cpp \
	audio.hpp common.hpp core.hpp font.hpp gamestrings.hpp \
	image.hpp inventory.hpp level.hpp player.hpp quadtree.hpp \
	screen.hpp settings.hpp texture.hpp uatypes.hpp \
	underworld.hpp uwadv.hpp objects.hpp cutscene.hpp physics.hpp \
	files.hpp keymap.hpp \
	linux/main.cpp \
	audio/audio.cpp audio/midi.cpp audio/xmidi.cpp \
	audio/midi.hpp audio/xmidi.hpp \
	audio/midi_driver/uni_fmod.cpp \
	audio/midi_driver/linux_sdl_mixer.cpp \
	audio/midi_driver/uni_fmod.h \
	audio/midi_driver/linux_sdl_mixer.h \
	conv/codevm.cpp conv/codevm.hpp \
	resource/codeloader.cpp resource/fontloader.cpp \
	resource/gamestrings.cpp resource/imageloader.cpp \
	resource/maploader.cpp resource/settings.cpp \
	resource/texloader.cpp resource/objloader.cpp \
	resource/cutsloader.cpp \
	resource/fread_endian.hpp \
	screens/ingame_orig.cpp screens/start_splash.cpp \
	screens/start_menu.cpp screens/cutscene_view.cpp \
	screens/acknowledgements.cpp screens/conversation.cpp \
	screens/create_character.cpp screens/map_view.cpp \
	screens/ingame_orig.hpp screens/start_splash.hpp \
	screens/start_menu.hpp screens/acknowledgements.hpp \
	screens/conversation.hpp screens/create_character.hpp \
	screens/map_view.hpp

uwadv_LDADD = $(OPENGL_LIBS) $(SDL_LIBS) liblua.a libzziplib.a -lz

uwadv_DEPENDENCIES = liblua.a libzziplib.a

EXTRA_DIST = audio/midi_driver/win_midiout.cpp \
	audio/midi_driver/win_midiout.h

SUBDIRS = tools

