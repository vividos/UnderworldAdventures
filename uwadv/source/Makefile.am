#
# Makefile.am for Underworld Adventures
#
# $Id$
#

SUBDIRS = tools resource lua

# compiler options
CXXFLAGS = @CXXFLAGS@
LDFLAGS = @LIBS@


# automake build rules

bin_PROGRAMS = uwadv
bin_SCRIPTS = linux/ua-lowercase-all.pl

# Makefile.common defines uwadv_SOURCES
include Makefile.common

uwadv_LDADD = \
	lua/liblualib.la \
	resource/zziplib/libzzip.la \
	-lz

EXTRA_DIST = \
	Makefile.common \
	audio/midi_driver/win_midiout.cpp \
	audio/midi_driver/win_midiout.h \
	linux/uwadv.spec.in \
	linux/uwadv.cfg.in \
	linux/ua-lowercase-all.pl \
	linux/uwadv.6.in \
	linux/ua-lowercase-all.pl.6 \
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
	win32/uaopt.dsp \
	win32/License.installer.txt \
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
cfg_DATA = linux/uwadv.cfg

man6_MANS = linux/uwadv.6 \
	linux/ua-lowercase-all.pl.6

linux/uwadv.cfg linux/uwadv.6: %: %.in
	rm -f $@
	sed -e 's,@UW1PATH\@,@UW1PATH@,g' \
            -e 's,@UADATAPATH\@,@UADATAPATH@,g' \
            -e 's,@UADOCPATH\@,@datadir@/doc/uwadv,g' $< >$@
