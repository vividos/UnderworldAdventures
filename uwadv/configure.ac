dnl
dnl configure.ac for Underworld Adventures
dnl
dnl $Id$
dnl

dnl initial stuff
AC_PREREQ(2.53)
AC_INIT(source/uwadv.cpp)


# version info
PACKAGE=uwadv
VERSION=0.5_butterfinger


AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE($PACKAGE,$VERSION,no-define)
AM_CONFIG_HEADER(source/config.h)

AC_CONFIG_SUBDIRS(source/resource/zziplib)

dnl initial variable values
CFLAGS="$CFLAGS -O2"
CXXFLAGS="$CXXFLAGS -O2"
OPENGL_LIBS=""

# OS specific options
case "$host_os" in
   darwin*)
      OPENGL_LIBS=""
      ;;
   *)
      OPENGL_LIBS="-lGL -lGLU"
      ;;

esac

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

# X config
AC_PATH_X
AC_PATH_XTRA
CFLAGS="$CFLAGS $X_CFLAGS"
CXXFLAGS="$CXXFLAGS $X_CFLAGS"
LIBS="$LIBS $X_LIBS"


# check standard C headers
AC_HEADER_STDC

# check other headers

# TODO: check which ones are required, and which ones are optional
AC_CHECK_HEADERS(glob.h)
AC_CHECK_HEADERS(cstdio cstdlib cstring cmath cctype)
AC_CHECK_HEADERS(vector string map iostream set bitset)
AC_CHECK_HEADERS(utility algorithm exception fstream)
AC_CHECK_HEADERS(sstream iomanip)


# checking for --enable-debug
AC_MSG_CHECKING(for debug options)

AC_ARG_ENABLE(debug,
  [  --enable-debug          enable debugging (disables optimizations)],
  [ CXXFLAGS="-O -g -Wall $CXXFLAGS"
    AC_MSG_RESULT(yes)
  ], AC_MSG_RESULT(no))


# checking for SDL
AM_PATH_SDL(1.2.3,,AC_MSG_ERROR([
Underworld Adventures needs at least SDL 1.2.3 or higher installed.
SDL is available here: http://www.libsdl.org/
]))


# checking for working OpenGL
save_CPPFLAGS="$CPPFLAGS"
CPPFLAGS="$CXXFLAGS"
AC_CHECK_HEADER(GL/gl.h,,AC_MSG_ERROR([
OpenGL header not found. An OpenGL graphics library is required
to compile and run Underworld Adventures.
Check http://mesa3d.sourceforge.net/ for a good one.]))
CPPFLAGS="$save_CPPFLAGS"

# checking for zlib
AC_CHECK_HEADER(zlib.h,,AC_MSG_ERROR([
zlib header not found. Zlib is required to compile and run
Underworld Adventures.]))



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


# determining uwadv.cfg entries
AC_ARG_WITH(uw1, [  --with-uw1=DIR          directory where UW1 is installed ],UW1PATH=$with_uw1,UW1PATH=${datadir}/games/uw1/)
AC_SUBST(UW1PATH)

UADATAPATH=${datadir}/games/uwadv/
AC_SUBST(UADATAPATH)

CPPFLAGS="$CPPFLAGS -DCONFIGDIR=\\\"${datadir}/games/uwadv/\\\""

# produce output files
AC_SUBST(OPENGL_LIBS)
AC_CONFIG_FILES([
Makefile
source/Makefile
source/tools/Makefile
source/resource/Makefile
source/resource/zziplib/Makefile
source/lua/Makefile
uadata/Makefile
hacking/Makefile
uwadv.spec:source/linux/uwadv.spec.in
source/linux/uwadv.6
])
AC_OUTPUT

