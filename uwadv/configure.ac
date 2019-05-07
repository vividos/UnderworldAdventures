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
# configure.ac for Underworld Adventures
#
# $Id$
#

# initial stuff
AC_PREREQ(2.53)
AC_INIT(source/uwadv.cpp)


# version info
PACKAGE=uwadv
VERSION=0.10_test


AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE($PACKAGE,$VERSION,no-define)
AM_CONFIG_HEADER(source/config.h)


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
AC_PROG_LIBTOOL

# check standard C headers
AC_HEADER_STDC


# check other headers
AC_HEADER_DIRENT
AC_CHECK_HEADERS(stdint.h unistd.h windows.h zlib.h sys/int_types.h io.h)
AC_CHECK_HEADERS(sys/types.h sys/stat.h sys/param.h)
AC_CHECK_HEADERS(glob.h)

# standard C++ headers
# only check some that are rather new
AC_CHECK_HEADERS(bitset sstream,,[AC_MSG_ERROR([A required Standard C++ Library header file is not found.])])


# checking for --enable-debug
AC_MSG_CHECKING(for debug options)

OPT_FLAGS="-O3"
AC_ARG_ENABLE(debug,
  [  --enable-debug          enable debugging (disables optimizations)],
  [ OPT_FLAGS="-O -g -Wall -DHAVE_DEBUG"
    AC_MSG_RESULT(yes)
  ], AC_MSG_RESULT(no))


# checking for SDL
AM_PATH_SDL2(2.0.9,,[AC_MSG_ERROR([
Underworld Adventures needs at least SDL 2.0.9 or higher installed.
SDL is available here: http://www.libsdl.org/
])])


# checking for SDL_mixer
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
   *mingw32)
      OPENGL_LIBS="-lopengl32 -lglu32"
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


# build the tools?
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


# where to put configuration files?
AC_ARG_WITH(configdir, [  --with-configdir=DIR          directory where UWADV configuration files will be placed], CONFIGDIR=$with_confdir, CONFIGDIR=${datadir}/games/uwadv/)

# determining uwadv.cfg entries
AC_ARG_WITH(uw1, [  --with-uw1=DIR          directory where UW1 is installed ],UW1PATH=$with_uw1,UW1PATH=${datadir}/games/uw1/)

AC_ARG_WITH(uw2, [  --with-uw2=DIR          directory where UW2 is installed ],UW2PATH=$with_uw2,UW2PATH=${datadir}/games/uw2/)

UADATAPATH=${datadir}/games/uwadv/



# set up compiler flags
CFLAGS="$CFLAGS -I. -I${srcdir} $OPT_FLAGS $SDL_CFLAGS"
CXXFLAGS="$CXXFLAGS -I. -I${srcdir} $OPT_FLAGS $SDL_CFLAGS"
LDFLAGS="$LDFLAGS $SDL_LDFLAGS"
LIBS="$LIBS $SDL_LIBS"


# substitute all variables needed in automake files
AC_SUBST(OPENGL_LIBS)
AC_SUBST(ZLIB_LIBS)
AC_SUBST(CONFIGDIR)
AC_SUBST(UW1PATH)
AC_SUBST(UW2PATH)
AC_SUBST(UADATAPATH)


# produce output files
AC_CONFIG_FILES([
Makefile
source/Makefile
source/audio/Makefile
source/base/Makefile
source/base/zziplib/Makefile
source/import/Makefile
source/underw/Makefile
source/unittest/Makefile
source/win32/Makefile
uadata/Makefile
hacking/Makefile
uwadv.spec:linux/uwadv.spec.in
])
AC_OUTPUT

# output status
echo
echo " Underworld Adventures is now ready to be built. Your current settings are:"
echo
echo "    host = $host"
echo "    target = $target"
echo "    prefix = ${prefix}"
echo "    datadir = ${datadir}"
echo
echo "    CXXFLAGS = $CXXFLAGS"
echo "    LIBS = $LIBS"
echo "    LDFLAGS = $LDFLAGS"
echo
echo "    CONFIGDIR = $CONFIGDIR"
echo "    UW1PATH = $UW1PATH"
echo "    UW2PATH = $UW2PATH"
echo "    UADATAPATH = $UADATAPATH"
echo
echo " start compiling with the command:"
echo "    make"
echo
