#
# Underworld Adventures - an Ultima Underworld remake project
# Copyright (c) 2002,2003,2004,2005,2006 Michael Fink
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
# Makefile.am for source folder
#
# $Id$
#

SUBDIRS = base unittest win32
#	audio base conv debug import macosx renderer resource \
#	screens script tools uastudio ui underw 

# compiler options
#INCLUDES = -I$(srcdir)/base -I$(srcdir)/conv -I$(srcdir)/audio -I$(srcdir)/ui -I$(srcdir)/renderer -I$(srcdir)/underw -I$(srcdir)/import -I$(srcdir)/script -I$(srcdir)/script/lua/include


# automake build rules

#bin_PROGRAMS = uwadv

# Makefile.common defines uwadv_SOURCES
#include Makefile.common

#uwadv_LDADD = \
#	base/libbase.la

EXTRA_DIST = \
	Makefile.common \
	audio/midi_driver/win_midiout.cpp \
	audio/midi_driver/win_midiout.h \
	resource/model/wrllexer.l \
	resource/model/flex.skl \
	resource/model/import3ds.cpp

#script/lua/liblualib.la:
#	cd script/lua; $(MAKE)
