INCLUDES = -Isrc -Iinclude

noinst_LTLIBRARIES = liblua.la liblualib.la

liblua_la_SOURCES = \
	src/lapi.c src/lapi.h \
	src/lcode.c src/lcode.h \
	src/ldebug.c src/ldebug.h \
	src/ldo.c src/ldo.h \
	src/lfunc.c src/lfunc.h \
	src/lgc.c src/lgc.h \
	src/llex.c src/llex.h \
	src/lmem.c src/lmem.h \
	src/lobject.c src/lobject.h \
	src/lparser.c src/lparser.h \
	src/lstate.c src/lstate.h \
	src/lstring.c src/lstring.h \
	src/ltable.c src/ltable.h \
	src/ltests.c \
	src/ltm.c src/ltm.h \
	src/lundump.c src/lundump.h \
	src/lvm.c src/lvm.h \
	src/lzio.c src/lzio.h \
        src/llimits.h \
        src/lopcodes.h \
	include/lauxlib.h \
	include/luadebug.h \
	include/lua.h \
	include/lualib.h

liblualib_la_SOURCES = \
	src/lib/lauxlib.c \
	src/lib/lbaselib.c \
	src/lib/liolib.c \
	src/lib/lmathlib.c \
	src/lib/lstrlib.c

liblualib_la_LIBADD = liblua.la
liblualib_la_DEPENDENCIES = liblua.la

noinst_PROGRAMS = luac

luac_SOURCES = \
	src/luac/dump.c \
	src/luac/luac.c src/luac/luac.h \
	src/luac/opt.c \
	src/luac/print.c src/luac/print.h \
	src/luac/stubs.c

luac_LDADD = liblua.la
luac_DEPENDENCIES = liblua.la

# lua configuration
#include config

EXTRA_DIST = \
	config \
	COPYRIGHT \
	HISTORY \
	lua-4.0-4.0.1.diff \
	README \
	src/lib/README \
	src/luac/README	\
	src/luac/Makefile \
	src/lib/Makefile \
	src/Makefile \
	src/lib/ldblib.c
