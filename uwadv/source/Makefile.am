#
# Makefile.am for Underworld Adventures
#
# $Id$
#

SUBDIRS = tools

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
	lua/src/lundump.c lua/src/lvm.c lua/src/lzio.c \
	lua/src/lib/lauxlib.c lua/src/lib/lbaselib.c \
	lua/src/lib/lmathlib.c lua/src/lib/lstrlib.c \
	lua/src/lapi.h lua/src/lcode.h lua/src/ldebug.h \
	lua/src/ldo.h lua/src/lfunc.h lua/src/lgc.h \
	lua/src/llex.h lua/src/llimits.h lua/src/lmem.h \
	lua/src/lobject.h lua/src/lopcodes.h lua/src/lparser.h \
	lua/src/lstate.h lua/src/lstring.h lua/src/ltable.h \
	lua/src/ltm.h lua/src/lundump.h lua/src/lvm.h \
	lua/src/lzio.h \
	lua/include/lauxlib.h lua/include/lua.h \
	lua/include/luadebug.h lua/include/lualib.h

liblua_a_CFLAGS = -Ilua/include -Wall


libzziplib_a_SOURCES = resource/zziplib/SDL_rwops_zzip.c \
	resource/zziplib/zzip-dir.c \
	resource/zziplib/zzip-err.c \
	resource/zziplib/zzip-file.c \
	resource/zziplib/zzip-info.c \
	resource/zziplib/zzip-io.c \
	resource/zziplib/zzip-stat.c \
	resource/zziplib/zzip-zip.c \
	resource/zziplib/SDL_rwops_zzip.h \
	resource/zziplib/zzip-conf.h \
	resource/zziplib/zzip-config.h \
	resource/zziplib/zzip-file.h \
	resource/zziplib/zzip-io.h \
	resource/zziplib/zzip-msvc.h \
	resource/zziplib/zzip-stdint.h \
	resource/zziplib/zzip.h \
	resource/zziplib/zzipformat.h \
	resource/zziplib/zziplib.h


libzziplib_a_CFLAGS = -Iresource/zziplib $(SDL_CFLAGS) -Wall


bin_PROGRAMS = uwadv

uwadv_SOURCES = font.cpp image.cpp inventory.cpp level.cpp \
	player.cpp quadtree.cpp texture.cpp underworld.cpp uwadv.cpp \
	objects.cpp cutscene.cpp physics.cpp files.cpp keymap.cpp \
	uwscript.cpp settings.cpp \
	audio.hpp common.hpp core.hpp font.hpp gamestrings.hpp \
	image.hpp inventory.hpp level.hpp player.hpp quadtree.hpp \
	screen.hpp settings.hpp texture.hpp uatypes.hpp \
	underworld.hpp uwadv.hpp objects.hpp cutscene.hpp physics.hpp \
	files.hpp keymap.hpp uwscript.hpp uamath.hpp settings.hpp \
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
	resource/maploader.cpp resource/settingsloader.cpp \
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
	screens/map_view.hpp screens/cutscene_view.hpp

uwadv_LDADD = $(OPENGL_LIBS) $(SDL_LIBS) liblua.a libzziplib.a -lz

uwadv_DEPENDENCIES = liblua.a libzziplib.a

EXTRA_DIST = audio/midi_driver/win_midiout.cpp \
	audio/midi_driver/win_midiout.h \
	linux/uwadv.spec.in \
	linux/uwadv.cfg.in \
	linux/ua-lowercase-all.pl \
	win32/animview.dsp \
	win32/cnvdasm.dsp \
	win32/cnvdbg.dsp \
	win32/mapdisp.dsp \
	win32/strpak.dsp \
	win32/tools.dsw \
	win32/uwadv.cfg \
	win32/uwadv.dsp \
	win32/uwadv.nsi \
	win32/xmi2mid.dsp \
	win32/install/License.installer.txt \
	win32/source/game_win32.cpp \
	win32/source/game_win32.hpp \
	win32/source/main.cpp \
	win32/source/resource.h \
	win32/source/Uuw.ico \
	win32/source/uwadv.rc

cfgdir = $(datadir)/games/uwadv
cfg_DATA = uwadv.cfg

uwadv.cfg: linux/uwadv.cfg.in
	rm -f $@ $@.tmp $@.tmptmp
	sed 's,@UW1PATH\@,@UW1PATH@,g' $< >$@.tmp
	sed 's,@UADATAPATH\@,@UADATAPATH@,g' $@.tmp >$@.tmptmp
	rm -f $@.tmp
	mv $@.tmptmp $@
