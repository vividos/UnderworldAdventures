INCLUDES = -I$(srcdir)/src -I$(srcdir)/include

noinst_LTLIBRARIES = liblua.la liblualib.la

# Makefile.common defines lublua_la_SOURCES and liblualib_la_SOURCES
include Makefile.common

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

EXTRA_DIST = \
	Makefile.common \
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
	src/lib/ldblib.c \
	src/lib/liolib.c
