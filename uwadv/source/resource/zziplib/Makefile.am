CFLAGS = $(SDL_CFLAGS)

VERSIONINFO=@VERSIONINFO@
RELEASEINFO=@RELEASEINFO@
THREAD_SAFE=@THREAD_SAFE@


noinst_LTLIBRARIES = libzzip.la

libzzip_la_SOURCES = \
	SDL_rwops_zzip.c SDL_rwops_zzip.h \
	zzip-dir.c \
	zzip-err.c \
	zzip-file.c zzip-file.h \
	zzip-info.c \
	zzip-io.c zzip-io.h \
	zzip-stat.c \
	zzip-zip.c \
	zzip-conf.h \
	zzip-config.h \
	zzip-msvc.h \
	zzip-stdint.h \
	zzip.h \
	zzipformat.h \
	zziplib.h

libzzip_la_LDFLAGS= @ZZIPLIB_LDFLAGS@ $(RELEASEINFO) $(VERSIONINFO) \
                    $(THREAD_SAFE)
libzzip_la_LIBADD= -lz @RESOLVES@


EXTRA_DIST = \
	COPYING.LGPL \
	COPYING.ZZIP
