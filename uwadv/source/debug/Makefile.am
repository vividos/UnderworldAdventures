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
# Makefile.am for the Underworld Debugger
#
# $Id$
#

# compiler options
CXXFLAGS = @CXXFLAGS@
LDFLAGS = @LIBS@

if BUILD_UADEBUG
 lib_LTLIBRARIES = libuadebug.la
endif

# Makefile.common defines libuadebug_la_SOURCES
include Makefile.common


if BUILD_UADEBUG
 dist_add =
else
 dist_add = $(libuadebug_la_SOURCES) 
endif

EXTRA_DIST = \
	$(dist_add) \
	Makefile.common
