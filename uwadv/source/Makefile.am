#
# Makefile.am for Underworld Adventures
#
# $Id$
#

SUBDIRS = tools resource lua

# compiler options
CXXFLAGS = -I. $(SDL_CFLAGS) $(X_CFLAGS) -Wall
LDFLAGS = $(SDL_LIBS) $(X_LIBS) -lSDL_mixer


# automake build rules

bin_PROGRAMS = uwadv

# Makefile.common defines uwadv_SOURCES
include Makefile.common

uwadv_LDADD = \
	$(OPENGL_LIBS) \
	$(SDL_LIBS) \
	lua/liblualib.la \
	resource/zziplib/libzzip.la \
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
	win32/uaconfig.dsp \
	win32/License.installer.txt
	win32/source/game_win32.cpp \
	win32/source/game_win32.hpp \
	win32/source/main.cpp \
	win32/source/resource.h \
	win32/source/Uus.ico \
	win32/source/uwadv_res.rc \
	win32/source/uaconfig/Corp.ico \
	win32/source/uaconfig/resource.h \
	win32/source/uaconfig/uaconfig.cpp \
	win32/source/uaconfig/uaconfig.hpp \
	win32/source/uaconfig/uaconfig_res.rc

cfgdir = $(datadir)/games/uwadv
cfg_DATA = uwadv.cfg

uwadv.cfg: linux/uwadv.cfg.in
	rm -f $@ $@.tmp $@.tmptmp
	sed 's,@UW1PATH\@,@UW1PATH@,g' $< >$@.tmp
	sed 's,@UADATAPATH\@,@UADATAPATH@,g' $@.tmp >$@.tmptmp
	rm -f $@.tmp
	mv $@.tmptmp $@
