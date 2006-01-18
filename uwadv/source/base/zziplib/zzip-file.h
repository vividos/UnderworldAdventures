/*
 * Author:
 *      Guido Draheim <guidod@gmx.de>
 *      Tomi Ollila <Tomi.Ollila@tfi.net>
 *
 *      Copyright (c) 1999,2000,2001,2002 Guido Draheim
 *          All rights reserved,
 *          use under the restrictions of the
 *          Lesser GNU General Public License
 *          note the additional license information
 *          that can be found in COPYING.ZZIP
 */

#ifndef _ZZIP_FILE_H
#define _ZZIP_FILE_H 1

#include <zzip-conf.h>
#include <zzip.h>

#include <zlib.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#else
#include <stdio.h>
# ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
# endif
#endif

#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h> /* PATH_MAX */
#endif

#ifndef PATH_MAX
# ifdef  MAX_PATH /* windows */
# define PATH_MAX MAX_PATH
# else
# define PATH_MAX 512
# endif
#endif
/*
 * ZZIP_FILE structure... currently no need to unionize, since structure needed
 * for inflate is superset of structure needed for unstore.
 *
 * Don't make this public. Instead, create methods for needed operations.
 */

struct zzip_file
{
  struct zzip_dir* dir;
  int fd;
  int method;
  int restlen;
  int crestlen;
  int usize;
  int csize;
  /* added dataoffset member - data offset from start of zipfile*/
  zzip_off_t dataoffset;
  char* buf32k;
  zzip_off_t offset; /* offset from the start of zipfile... */
  z_stream d_stream;
  zzip_plugin_io_t io;
};

#endif /* _ZZIP_FILE_H */

