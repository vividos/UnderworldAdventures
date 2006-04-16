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

SUBDIRS = base audio import underw unittest win32
# conv debug renderer resource screens script tools uastudio ui 

bin_PROGRAMS = uwadv

INCLUDES = -I$(srcdir)/audio -I$(srcdir)/base -I$(srcdir)/import -I$(srcdir)/underw

uwadv_LDADD = \
	audio/libaudio.la \
	base/libbase.la \
	import/libimport.la \
	underw/libunderw.la

# Makefile.common defines uwadv_SOURCES
include Makefile.common

EXTRA_DIST = \
	Makefile.common \
	resource/model/wrllexer.l \
	resource/model/flex.skl \
	resource/model/import3ds.cpp

#script/lua/liblualib.la:
#	cd script/lua; $(MAKE)
