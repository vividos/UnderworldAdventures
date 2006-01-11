#ifndef _ZZIP_CONFIG_H
#define _ZZIP_CONFIG_H 1

/* edited for MSVC usage, derived from mingw32 automated test. */ 
/* zzip-config.h. Generated automatically at end of configure. */
/* config.h.  Generated automatically by configure.  */
/* config.h.in.  Generated automatically from configure.ac by autoheader.  */

/* Define if you have the <dirent.h> header file, and it defines `DIR'. */
//#ifndef HAVE_DIRENT_H 
//#define HAVE_DIRENT_H  1 
//#endif

/* Define if you have the <inttypes.h> header file. */
/* #undef HAVE_INTTYPES_H */

/* Define if you have the <memory.h> header file. */
#ifndef HAVE_MEMORY_H 
#define HAVE_MEMORY_H  1 
#endif

/* Define if you have the <stdint.h> header file. */
/* #undef HAVE_STDINT_H */

/* Define if you have the <stdlib.h> header file. */
#ifndef HAVE_STDLIB_H 
#define HAVE_STDLIB_H  1 
#endif

/* Define if you have the <strings.h> header file. */
/* #undef HAVE_STRINGS_H */

/* Define if you have the <string.h> header file. */
#ifndef HAVE_STRING_H 
#define HAVE_STRING_H  1 
#endif

/* Define if you have the <sys/int_types.h> header file. */
/* #undef HAVE_SYS_INT_TYPES_H */

/* Define if you have the <sys/param.h> header file. */
/* #undef HAVE_SYS_PARAM_H */

/* Define if you have the <sys/stat.h> header file. */
#ifndef HAVE_SYS_STAT_H 
#define HAVE_SYS_STAT_H  1 
#endif

/* Define if you have the <sys/types.h> header file. */
#ifndef HAVE_SYS_TYPES_H 
#define HAVE_SYS_TYPES_H  1 
#endif

/* Define if you have the <unistd.h> header file. */
/* #undef HAVE_UNISTD_H */

/* Define if you have the <windows.h> header file. */
#ifndef HAVE_WINDOWS_H 
#define HAVE_WINDOWS_H  1
#endif

/* Define if you have the <zlib.h> header file. */
#ifndef HAVE_ZLIB_H 
#define HAVE_ZLIB_H  1
#endif

/* Name of package */
#ifndef ZZIP_PACKAGE 
#define ZZIP_PACKAGE  "zziplib-msvc"           /* yes, make it known */
#endif

/* The number of bytes in type int */
#ifndef ZZIP_SIZEOF_INT 
#define ZZIP_SIZEOF_INT  4
#endif

/* The number of bytes in type long */
#ifndef ZZIP_SIZEOF_LONG 
#define ZZIP_SIZEOF_LONG  4 
#endif

/* The number of bytes in type short */
#ifndef ZZIP_SIZEOF_SHORT 
#define ZZIP_SIZEOF_SHORT  2 
#endif

/* Define if you have the ANSI C header files. */
#ifndef ZZIP_STDC_HEADERS 
#define ZZIP_STDC_HEADERS  1 
#endif

/* Version number of package */
/* #undef ZZIP_VERSION  "0.10.17" */ /* no automatic update -> disable */

/* Define to empty if `const' does not conform to ANSI C. */
/* #undef _zzip_const */

/* Define as `__inline' if that's what the C compiler calls it, or to nothing
   if it is not supported. */
#define _zzip_inline __inline

/* Define to `long' if <sys/types.h> does not define. */
#define _zzip_off_t long
 
/* _ZZIP_CONFIG_H */
#endif

