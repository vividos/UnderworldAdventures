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
VERSION=0.7_hot_chili


AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE($PACKAGE,$VERSION,no-define)
AM_CONFIG_HEADER(source/config.h)

AC_CONFIG_SUBDIRS(source/resource/zziplib)


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
AC_CHECK_HEADERS(bitset sstream,,AC_MSG_ERROR([A required C++ header file is not found.]))


# checking for --enable-debug
AC_MSG_CHECKING(for debug options)

DEBUG_FLAGS="-O2"
AC_ARG_ENABLE(debug,
  [  --enable-debug          enable debugging (disables optimizations)],
  [ DEBUG_FLAGS="-O -g -Wall -DHAVE_DEBUG"
    AC_MSG_RESULT(yes)
  ], AC_MSG_RESULT(no))


# checking for X11
#case "$host_os" in
#   darwin*)
#      X_CFLAGS=""
#      X_LIB=""
#      ;;
#   *)
#      AC_PATH_X
#      AC_PATH_XTRA
#      ;;
#
#esac


# checking for SDL
AM_PATH_SDL(1.2.3,,AC_MSG_ERROR([
Underworld Adventures needs at least SDL 1.2.3 or higher installed.
SDL is available here: http://www.libsdl.org/
]))

# todo: check for SDL_mixer
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
      AC_CHECK_HEADER(GL/gl.h,,AC_MSG_ERROR([
OpenGL header not found. An OpenGL graphics library is required
to compile and run Underworld Adventures.
Check http://mesa3d.sourceforge.net/ for a good one.]))
      ;;

esac
CPPFLAGS="$save_CPPFLAGS"


# checking for zlib
AC_CHECK_HEADER(zlib.h,,AC_MSG_ERROR([
zlib header not found. Zlib is required to compile and run
Underworld Adventures.]))
ZLIB_LIBS="-lz"


# Build the tools?
AC_ARG_ENABLE(tools,    [  --disable-tools         Don't build the tools],,enable_tools=yes)
AC_MSG_CHECKING(whether to build the tools)
if test x$enable_tools = xno; then
        AC_MSG_RESULT(yes)
        AM_CONDITIONAL(BUILD_TOOLS, false)
else
        AC_MSG_RESULT(no)
        AM_CONDITIONAL(BUILD_TOOLS, true)
fi


# Build the debugger?
AC_ARG_ENABLE(uadebug,    [  --enable-uadebug        Enables building the Underworld Debugger;
note that wxWindows is needed],,enable_uadebug=no)
AC_MSG_CHECKING(whether to build the underworld debugger)
if test x$enable_uadebug = xyes; then
        AC_MSG_RESULT(yes)
        AC_MSG_WARN([ building the Underworld Debugger is currently not implemented!
        Nothing is built. ])
        AM_CONDITIONAL(BUILD_UADEBUG, false)
else
        AC_MSG_RESULT(no)
        AM_CONDITIONAL(BUILD_UADEBUG, false)
fi


# determining uwadv.cfg entries
AC_ARG_WITH(uw1, [  --with-uw1=DIR          directory where UW1 is installed ],UW1PATH=$with_uw1,UW1PATH=${datadir}/games/uw1/)
AC_SUBST(UW1PATH)

UADATAPATH=${datadir}/games/uwadv/
AC_SUBST(UADATAPATH)


# set compiler variables
CPPFLAGS="$CPPFLAGS -DCONFIGDIR=\\\"${datadir}/games/uwadv/\\\""
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
source/resource/Makefile
source/resource/zziplib/Makefile
source/lua/Makefile
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
echo "    UW1PATH = $UW1PATH"
echo "    UADATAPATH = $UADATAPATH"
echo
echo " start compiling with the command:"
echo "    make"
echo

