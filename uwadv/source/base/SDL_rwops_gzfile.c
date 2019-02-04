/*
   Underworld Adventures - an Ultima Underworld remake project
   Copyright (c) 2006,2019 Michael Fink

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

   $Id$

*/
/*! \file SDL_rwops_gzfile.c

   \brief SDL_RWops wrapper for zlib's gzFile APIs implementation

*/

/* needed includes */
#include <SDL_rwops_gzfile.h>
#include <zlib.h>
#include <errno.h>

/*! access to gzFile pointer in SDL_RWops struct */
#define SDL_RWOPS_GZFILE(_context) \
             ((gzFile) (_context)->hidden.unknown.data1)

/*! l-value access to gzFile pointer in SDL_RWops struct */
#define SDL_RWOPS_GZFILE_LVALUE(_context) \
             ((_context)->hidden.unknown.data1)

/*! wrapper for gzseek */
static Sint64 gzip_seek(SDL_RWops* context, Sint64 offset, int whence)
{
   if (offset == 0 && whence == RW_SEEK_CUR)
      return gztell64(SDL_RWOPS_GZFILE(context));

   return gzseek64(SDL_RWOPS_GZFILE(context), offset, whence);
}

/*! wrapper for gzread */
static size_t gzip_read(SDL_RWops* context, void* ptr, size_t size, size_t maxnum)
{
   return gzread(SDL_RWOPS_GZFILE(context), ptr, size*maxnum);
}

/*! wrapper for gzwrite */
static size_t gzip_write(SDL_RWops* context, const void* ptr, size_t size, size_t num)
{
   return gzwrite(SDL_RWOPS_GZFILE(context), ptr, size*num);
}

/*! wrapper for gzclose; automatically frees SDL_RWops ptr */
static int gzip_close(SDL_RWops* context)
{
   int iRet;
   if (!context)
      return -1;

   iRet = gzclose(SDL_RWOPS_GZFILE(context));
   SDL_FreeRW(context);
   return iRet;
}

SDL_RWops* SDL_RWFromGzFile(const char* filename, const char* mode)
{
   SDL_RWops* rwops = NULL;
   gzFile gz;

   // open file
   gz = gzopen(filename, mode);
   if (!gz)
      return 0;

   // alloc struct
   rwops = SDL_AllocRW();
   if (!rwops)
   {
      errno = ENOMEM;
      gzclose(gz);
      return 0;
   }

   // init struct
   SDL_RWOPS_GZFILE_LVALUE(rwops) = gz;
   rwops->read = gzip_read;
   rwops->write = gzip_write;
   rwops->seek = gzip_seek;
   rwops->close = gzip_close;

   return rwops;
}
