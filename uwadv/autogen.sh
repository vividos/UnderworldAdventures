#! /bin/sh
#
# Underworld Adventures - an Ultima Underworld remake project
# Copyright (c) 2002,2003,2004,2005,2006 Willem Jan Palenstijn
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
# autogen.sh for Underworld Adventures
#
# $Id$
#

DIE=0

# Check for availability
(autoconf --version) < /dev/null > /dev/null 2>&1 || {
  echo "**Error**: You must have 'autoconf' installed to compile Underworld Adventures."
  DIE=1
}

(automake --version) < /dev/null > /dev/null 2>&1 || {
  echo "**Error**: You must have 'automake' installed to compile Underworld Adventures."
  DIE=1
  NO_AUTOMAKE=yes
}
# if no automake, don't bother testing for aclocal
test -n "$NO_AUTOMAKE" || (aclocal --version) < /dev/null > /dev/null 2>&1 || {
  echo "**Error**: Missing 'aclocal'.  The version of 'automake'"
  echo "installed doesn't appear recent enough."
  DIE=1
}

if test "$DIE" -eq 1; then
  exit 1
fi

aclocalincludes=""
if test -d "/usr/local/share/aclocal"; then
  if test "/usr/local/share/aclocal" != `aclocal --print-ac-dir`; then
    aclocalincludes="$aclocalincludes -I /usr/local/share/aclocal"
  fi
fi

#if test -d "$HOME/share/aclocal"; then
#  if test "$HOME/share/aclocal" != `aclocal --print-ac-dir`; then
#    aclocalincludes="$aclocalincludes -I $HOME/share/aclocal"
#  fi
#fi

echo "Cleaning up..."

# clean up generated files
rm -f configure config.log config.guess config.sub config.status
rm -f depcomp compile
rm -f aclocal.m4
rm -f install-sh ltmain.sh missing mkinstalldirs
rm -f uwadv.spec source/uwadv.cfg
rm -f Makefile.in Makefile
rm -f hacking/Makefile.in hacking/Makefile
rm -f source/Makefile.in source/Makefile 
rm -f source/audio/Makefile.in source/audio/Makefile
rm -f source/base/Makefile.in source/base/Makefile
rm -f source/base/zziplib/Makefile.in source/base/zziplib/Makefile
rm -f source/import/Makefile.in source/import/Makefile
rm -f source/underw/Makefile.in source/underw/Makefile
rm -f source/unittest/Makefile.in source/unittest/Makefile
rm -f source/win32/Makefile.in source/win32/Makefile
rm -f uadata/Makefile.in uadata/Makefile
rm -rf source/.deps
rm -rf source/audio/.deps source/audio/.libs
rm -rf source/base/.deps source/base/.libs
rm -rf source/base/zziplib/.deps source/base/zziplib/.libs
rm -rf source/import/.deps source/import/.libs
rm -rf source/underw/.deps source/underw/.libs
rm -rf source/unittest/.deps

echo "Generating files..."

aclocal $aclocalincludes
libtoolize --automake --copy --force
autoheader
automake --copy --force --add-missing --foreign
autoconf

echo "You are now ready to run ./configure"
