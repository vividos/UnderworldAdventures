#
# Underworld Adventures - an Ultima Underworld hacking project
# Copyright (c) 2002,2003 Underworld Adventures Team
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
# Makefile.am for Underworld Adventures
#
# $Id$
#

SUBDIRS = base tools resource lua debug

# compiler options
INCLUDES = -I$(srcdir)/base -I$(srcdir)/audio -I$(srcdir)/ui -I$(srcdir)/renderer -I$(srcdir)/underw -I$(srcdir)/import
CXXFLAGS = @CXXFLAGS@
LDFLAGS = @LIBS@


# automake build rules

bin_PROGRAMS = uwadv

# Makefile.common defines uwadv_SOURCES
include Makefile.common

uwadv_LDADD = \
	lua/liblualib.la \
	base/zziplib/libzzip.la \
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

base/zziplib/libzzip.la:
	cd base/zziplib; $(MAKE)
