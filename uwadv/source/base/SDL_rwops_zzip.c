/*
 *      Copyright (c) 2001 Guido Draheim <guidod@gmx.de>
 *      Use freely under the restrictions of the ZLIB License
 *
 *      (this example uses errno which might not be multithreaded everywhere)
 */

#include <SDL_rwops_zzip.h>
#include <zzip/zzip.h>
#include <string.h> /* strchr */

/* MSVC can not take a casted variable as an lvalue ! */
#define SDL_RWOPS_ZZIP_DATA(_context) \
             ((_context)->hidden.unknown.data1)
#define SDL_RWOPS_ZZIP_FILE(_context)  (ZZIP_FILE*) \
             ((_context)->hidden.unknown.data1)

static Sint64 _zzip_seek(SDL_RWops *context, Sint64 offset, int whence)
{
    return zzip_seek(SDL_RWOPS_ZZIP_FILE(context), (int)offset, whence);
}

#undef _zzip_read

static size_t _zzip_read(SDL_RWops *context, void *ptr, size_t size, size_t maxnum)
{
    return zzip_read(SDL_RWOPS_ZZIP_FILE(context), ptr, size*maxnum) / size;
}

#undef _zzip_write

static size_t _zzip_write(SDL_RWops *context, const void *ptr, size_t size, size_t num)
{
    (void)context; (void)ptr; (void)size; (void)num;
    return 0; /* ignored */
}

static int _zzip_close(SDL_RWops *context)
{
    if (! context) return 0; /* may be SDL_RWclose is called by atexit */

    zzip_close (SDL_RWOPS_ZZIP_FILE(context));
    SDL_FreeRW (context);
    return 0;
}

static Sint64 _zzip_size(SDL_RWops* context)
{
   if (!context) return -1;

   ZZIP_STAT stat = { 0 };
   int ret = zzip_file_stat(SDL_RWOPS_ZZIP_FILE(context), &stat);

   return ret == 0 ? stat.st_size : -1;
}

SDL_RWops *SDL_RWFromZZIP(const char* file, const char* mode)
{
    register SDL_RWops* rwops;
    register ZZIP_FILE* zzip_file;

    if (! strchr (mode, 'r'))
      return SDL_RWFromFile(file, mode);

    zzip_file = zzip_fopen (file, mode);
    if (! zzip_file) return 0;

    rwops = SDL_AllocRW ();
    if (! rwops) { errno=ENOMEM; zzip_close (zzip_file); return 0; }

    SDL_RWOPS_ZZIP_DATA(rwops) = zzip_file;
    rwops->type = SDL_RWOPS_UNKNOWN;
    rwops->size = _zzip_size;
    rwops->read = _zzip_read;
    rwops->write = _zzip_write;
    rwops->seek = _zzip_seek;
    rwops->close = _zzip_close;
    return rwops;
}
