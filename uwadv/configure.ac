#
# Underworld Adventures - an Ultima Underworld hacking project
# Copyright (c) 2002,2003,2004 Underworld Adventures Team
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
# configure.ac for Underworld Adventures
#
# $Id$
#

# initial stuff
AC_PREREQ(2.53)
AC_INIT(source/uwadv.cpp)


# version info
PACKAGE=uwadv
VERSION=0.9_mojito


AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE($PACKAGE,$VERSION,no-define)
AM_CONFIG_HEADER(source/config.h)

AC_CONFIG_SUBDIRS(source/base/zziplib)


# initial variable values
CFLAGS="$CFLAGS"
CXXFLAGS="$CXXFLAGS"


# this is technically OS-specific, but for now all OS-es that use configure
# do have a home-dir (specifically that the 'HOME' environment var. is set)

AC_DEFINE(HAVE_HOME,,[Users have home directories on this OS])


# find and test the compiler
AC_PROG_CC
AC_PROG_CPP
AC_C_INLINE
AC_C_CONST

AC_PROG_CXX
AC_LANG_CPLUSPLUS


# other needed programs
AC_PROG_MAKE_SET
AC_PROG_INSTALL
AC_PROG_RANLIB

AM_PROG_LIBTOOL


# check standard C headers
AC_HEADER_STDC

# check other headers
AC_CHECK_HEADERS(glob.h)

dnl is it really necessary to check for these? it slows down configure a lot
dnl maybe only do bitset/sstream?
dnl AC_CHECK_HEADERS(cstdio cstdlib cstring cmath cctype)
dnl AC_CHECK_HEADERS(vector string map iostream set bitset)
dnl AC_CHECK_HEADERS(utility algorithm exception fstream)
dnl AC_CHECK_HEADERS(sstream iomanip)
AC_CHECK_HEADERS(bitset sstream,,[AC_MSG_ERROR([A required C++ header file is not found.])])


# checking for --enable-debug
AC_MSG_CHECKING(for debug options)

DEBUG_FLAGS="-O2"
AC_ARG_ENABLE(debug,
  [  --enable-debug          enable debugging (disables optimizations)],
  [ DEBUG_FLAGS="-O -g -Wall -DHAVE_DEBUG"
    AC_MSG_RESULT(yes)
  ], AC_MSG_RESULT(no))


# checking for SDL
AM_PATH_SDL(1.2.3,,[AC_MSG_ERROR([
Underworld Adventures needs at least SDL 1.2.3 or higher installed.
SDL is available here: http://www.libsdl.org/
])])

# check for SDL_mixer
AC_CHECK_HEADER(SDL/SDL_mixer.h,,[AC_MSG_ERROR([
SDL_mixer isn't installed. Please install SDL_mixer, available from
http://www.libsdl.org/projects/SDL_mixer/])])

SDL_LIBS="$SDL_LIBS -lSDL_mixer"

# checking for OpenGL
save_CPPFLAGS="$CPPFLAGS"
CPPFLAGS="$CXXFLAGS"

case "$host_os" in
   darwin*)
      OPENGL_LIBS=""
      ;;
   *)
      OPENGL_LIBS="-lGL -lGLU"
      AC_CHECK_HEADER(GL/gl.h,,[AC_MSG_ERROR([
OpenGL header not found. An OpenGL graphics library is required
to compile and run Underworld Adventures.
Check http://mesa3d.sourceforge.net/ for a good one.])])
      ;;

esac
CPPFLAGS="$save_CPPFLAGS"


# checking for zlib
AC_CHECK_HEADER(zlib.h,,[AC_MSG_ERROR([
zlib header not found. Zlib is required to compile and run
Underworld Adventures.])])
ZLIB_LIBS="-lz"


# Build the tools?
AC_ARG_ENABLE(tools,    [  --enable-tools          Don't build the tools],,
   enable_tools=no)
AC_MSG_CHECKING(whether to build the tools)
if test x$enable_tools = xno; then
   AC_MSG_RESULT(no)
   AM_CONDITIONAL(BUILD_TOOLS, false)
else
   AC_MSG_RESULT(yes)
   AM_CONDITIONAL(BUILD_TOOLS, true)
fi


# Build the debugger?
AC_ARG_ENABLE(uadebug,    [  --enable-uadebug        Enables building the Underworld Debugger;
note that wxWindows is needed],,enable_uadebug=no)

AC_MSG_CHECKING(whether to build the underworld debugger)
if test x$enable_uadebug = xyes; then
   AC_MSG_RESULT(yes)

   # check for wxWindows
   AM_OPTIONS_WXCONFIG

   AM_PATH_WXCONFIG(2.4.0, wxWin=1)
   if test "$wxWin" != 1; then
      AC_MSG_ERROR([
         wxWindows must be installed on your system
         but wx-config script couldn't be found.

         Please check that wx-config is in path, the directory
         where wxWindows libraries are installed (returned by
         'wx-config --libs' command) is in LD_LIBRARY_PATH or
         equivalent variable and wxWindows version is 2.3.4 or above.
      ])
   fi

   CPPFLAGS="$CPPFLAGS $WX_CPPFLAGS"
   CXXFLAGS="$CXXFLAGS $WX_CXXFLAGS_ONLY"
   CFLAGS="$CFLAGS $WX_CFLAGS_ONLY"

   # add compiler flags dependent on host system
   case "$host_os" in
      *mingw32)
         CFLAGS="$CFLAGS -D__GNUWIN32__"
         CXXFLAGS="$CXXFLAGS -D__GNUWIN32__"
         ;;
   esac

   # wxwin.m4 suggested putting $WX_LIBS to LDFLAGS, which clearly is the
   # wrong place. it belongs into LIBS, because it appears after the object
   # files in the linker command line
   LIBS="$LIBS $WX_LIBS"

   AC_MSG_WARN([ building the Underworld Debugger is currently not implemented!
      Nothing is built. ])

   AM_CONDITIONAL(BUILD_UADEBUG, false)
else
   AC_MSG_RESULT(no)
   AM_CONDITIONAL(BUILD_UADEBUG, false)
fi


#where to put configuration files?
AC_ARG_WITH(confdir, [  --with-confdir=DIR          directory where UWADV configuration files will be placed], CONFDIR=$with_confdir, CONFDIR=${datadir}/games/uwadv/)

# determining uwadv.cfg entries
AC_ARG_WITH(uw1, [  --with-uw1=DIR          directory where UW1 is installed ],UW1PATH=$with_uw1,UW1PATH=${datadir}/games/uw1/)
AC_SUBST(UW1PATH)


# see if we have xsltproc/docbook stylesheets so we can build docs
# this sets the 'have_xsltproc' automake conditional
AX_CHECK_DOCBOOK


UADATAPATH=${datadir}/games/uwadv/
AC_SUBST(UADATAPATH)


# set compiler variables
CPPFLAGS="$CPPFLAGS -DCONFIGDIR=\\\"$CONFDIR\\\""
CXXFLAGS="$CXXFLAGS -I. -I${srcdir} $DEBUG_FLAGS  $SDL_CFLAGS $X_CFLAGS"
CFLAGS="$CXXFLAGS"
LIBS="$LIBS $SDL_LIBS $OPENGL_LIBS $X_LIBS $ZLIB_LIBS"


# produce output files
AC_SUBST(OPENGL_LIBS)
AC_CONFIG_FILES([
Makefile
source/Makefile
source/debug/Makefile
source/tools/Makefile
source/tools/uwdump/Makefile
source/base/Makefile
source/script/Makefile
source/script/lua/Makefile
uadata/Makefile
hacking/Makefile
uwadv.spec:linux/uwadv.spec.in
])
AC_OUTPUT

# output status
echo
echo " Underworld Adventures is now ready to be built. Your current settings are:"
echo
echo "    host_os = $host_os"
echo "    prefix = ${prefix}"
echo "    datadir = ${datadir}"
echo
echo "    CXXFLAGS = $CXXFLAGS"
echo "    LIBS = $LIBS"
echo
echo "    CONFDIR = $CONFDIR"
echo "    UW1PATH = $UW1PATH"
echo "    UADATAPATH = $UADATAPATH"
echo
echo " start compiling with the command:"
echo "    make"
echo

