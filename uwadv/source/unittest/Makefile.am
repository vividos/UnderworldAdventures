#
# Underworld Adventures - an Ultima Underworld remake project
# Copyright (c) 2006 Michael Fink
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
# Makefile.am for unittest folder
#
# $Id$
#

check_PROGRAMS = unittest

TESTS = unittest

INCLUDES = -I$(srcdir)/../base -I$(srcdir)/../audio -I$(srcdir)/../import -I$(srcdir)/../underw

unittest_LDADD = \
   ../base/libbase.la \
   ../audio/libaudio.la \
   ../import/libimport.la \
   ../underw/libunderw.la

# Makefile.common defines unittest_SOURCES
include Makefile.common

EXTRA_DIST = Makefile.common
