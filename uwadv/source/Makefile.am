#
# Makefile.am for Underworld Adventures
#
# $Id$
#

SUBDIRS = tools resource lua

# compiler options
CXXFLAGS = -I. $(SDL_CFLAGS) -Wall
LDFLAGS = $(SDL_LIBS) -lSDL_mixer


# automake build rules

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

uwadv_LDADD = \
	$(OPENGL_LIBS) \
	$(SDL_LIBS) \
	lua/liblualib.la \
	resource/zziplib/libzziplib.la \
	-lz

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
	win32/source/Uus.ico \
	win32/source/uwadv.rc

cfgdir = $(datadir)/games/uwadv
cfg_DATA = uwadv.cfg

uwadv.cfg: linux/uwadv.cfg.in
	rm -f $@ $@.tmp $@.tmptmp
	sed 's,@UW1PATH\@,@UW1PATH@,g' $< >$@.tmp
	sed 's,@UADATAPATH\@,@UADATAPATH@,g' $@.tmp >$@.tmptmp
	rm -f $@.tmp
	mv $@.tmptmp $@
