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
	resource/model/wrllexer.l \
	resource/model/flex.skl \
	resource/model/import3ds.cpp \
	win32/debug.cpp \
	win32/game_win32.cpp \
	win32/game_win32.hpp \
	win32/main.cpp \
	win32/resource.h \
	win32/Uus.ico \
	win32/uwadv_res.rc \
	win32/uaconfig/Corp.ico \
	win32/uaconfig/resource.h \
	win32/uaconfig/uaconfig.cpp \
	win32/uaconfig/uaconfig.hpp \
	win32/uaconfig/uaconfig_res.rc

lua/liblualib.la:
	cd lua; $(MAKE)

resource/zziplib/libzzip.la:
	cd resource/zziplib; $(MAKE)
