CFLAGS = $(SDL_CFLAGS)

VERSIONINFO=@VERSIONINFO@
RELEASEINFO=@RELEASEINFO@
THREAD_SAFE=@THREAD_SAFE@


noinst_LTLIBRARIES = libzzip.la

# Makefile.common defines libzzip_la_SOURCES
include Makefile.common

libzzip_la_LDFLAGS= @ZZIPLIB_LDFLAGS@ $(RELEASEINFO) $(VERSIONINFO) \
                    $(THREAD_SAFE)
libzzip_la_LIBADD= -lz @RESOLVES@


EXTRA_DIST = \
	COPYING.LGPL \
	COPYING.ZZIP \
	Makefile.common
