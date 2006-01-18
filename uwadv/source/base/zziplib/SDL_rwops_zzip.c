/*
 *      Copyright (c) 2001 Guido Draheim <guidod@gmx.de>
 *      Use freely under the restrictions of the ZLIB License
 *
 *      (this example uses errno which might not be multithreaded everywhere)
 */

#include <SDL_rwops_zzip.h>
#include <zziplib.h>

#define SDL_RWOPS_ZZIP_FILE(_context) \
             ((ZZIP_FILE*) (_context)->hidden.unknown.data1)

#define SDL_RWOPS_ZZIP_FILE_LVALUE(_context) \
             ((_context)->hidden.unknown.data1)

static int _zzip_seek(SDL_RWops* context, int offset, int whence)
{
    return zzip_seek(SDL_RWOPS_ZZIP_FILE(context), offset, whence);
}

static int _zzip_read(SDL_RWops* context, void* ptr, int size, int maxnum)
{
    return zzip_read(SDL_RWOPS_ZZIP_FILE(context), ptr, size*maxnum);
}

static int _zzip_write(SDL_RWops* context, const void* ptr, int size, int num)
{
    (context); (ptr); (size); (num);
    return 0; /* ignored */
}

#ifndef O_BINARY
#define O_BINARY 0
#endif

/* some ugly include file seemed to define this :( */
#undef read

static int _zzip_close(SDL_RWops* context)
{
    if (! context) return 0; /* may be SDL_RWclose is called by atexit */

    zzip_close (SDL_RWOPS_ZZIP_FILE(context));
    SDL_FreeRW (context);
    return 0;
}

SDL_RWops* SDL_RWFromZZIP(const char* file, const char* mode)
{
    register SDL_RWops* rwops;
    register ZZIP_FILE* zzip_file;

    /* extension list; to open zip files using their full name */
    zzip_strings_t exts[] = { "", 0 };

    (mode);

    /* open file, only opening zip files */
    zzip_file = zzip_open_ext_io (file, O_RDONLY|O_BINARY, ZZIP_ONLYZIP, exts, 0);
    if (!zzip_file) return 0;

    rwops = SDL_AllocRW ();
    if (!rwops) { errno=ENOMEM; zzip_close (zzip_file); return 0; }

    SDL_RWOPS_ZZIP_FILE_LVALUE(rwops) = zzip_file;
    rwops->read = _zzip_read;
    rwops->write = _zzip_write;
    rwops->seek = _zzip_seek;
    rwops->close = _zzip_close;
    return rwops;
}
