/*
 * Author: 
 *	Guido Draheim <guidod@gmx.de>
 *	Tomi Ollila <Tomi.Ollila@iki.fi>
 *
 *	Copyright (c) 1999,2000,2001,2002 Guido Draheim
 * 	    All rights reserved,
 *	    use under the restrictions of the
 *	    Lesser GNU General Public License
 *          note the additional license information 
 *          that can be found in COPYING.ZZIP
 */

#include "zzip.h"
#include "zzip-file.h"
#include <stddef.h> /*offsetof*/
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifdef ZZIP_HAVE_SYS_STAT_H
#include <sys/stat.h>
#else
#include <stdio.h>
#endif

#ifdef ZZIP_HAVE_DIRENT_H
#include <dirent.h>
#include <stdio.h>
#endif

#ifndef offsetof
#pragma warning had to DEFINE offsetof as it was not in stddef.h
#define offsetof(T,M) ((unsigned)(& ((T*)0)->M)))
#endif

/**
 * see => rewinddir(2), rewinds the DIR returned by => zzip_opendir
 */
void 
zzip_rewinddir(ZZIP_DIR * dir)
{
    if (! dir) return;

#  ifdef ZZIP_HAVE_DIRENT_H
    if (dir->realdir) 
    {
        rewinddir(dir->realdir);
        return;
    }
#  endif

    if (dir->hdr0)
        dir->hdr = dir->hdr0;
    else
        dir->hdr = 0;
}

#ifdef ZZIP_HAVE_DIRENT_H
static int
real_readdir(ZZIP_DIR* dir)
{
    struct stat st = { 0 };
    char filename[PATH_MAX];
    struct dirent* dirent = readdir(dir->realdir);
    if (! dirent) return 0;

    dir->dirent.d_name = dirent->d_name;
    strcpy(filename, dir->realname);
    strcat(filename, "/");
    strcat(filename, dirent->d_name);

    if (stat(filename, &st) == -1)
        perror(filename);

    dir->dirent.d_csize = dir->dirent.st_size = st.st_size;

    if (st.st_mode)
    {
        if (! S_ISREG(st.st_mode))
        {
            dir->dirent.d_compr = st.st_mode;
            dir->dirent.d_compr |= 0x80000000; 
                	/* makes it effectively negative, but can still be fed to S_ISXXX(x) */
        }else
        {
            dir->dirent.d_compr = 0; /* stored */
        }
    }else
    {
            dir->dirent.d_compr = 0; /* stored */
    }

    return 1;
}
#endif

/**
 * see => readdir(2), reads the DIR returned by => zzip_opendir
 */
ZZIP_DIRENT* 
zzip_readdir(ZZIP_DIR * dir)
{
    if (! dir) { errno=EBADF; return 0; }

#  ifdef ZZIP_HAVE_DIRENT_H
    if (dir->realdir)
    {
        if (! real_readdir(dir))
            return 0;
    }else
#  endif
    {
        if (! dir->hdr) return 0;

        dir->dirent.d_name  = dir->hdr->d_name;
        dir->dirent.d_compr = dir->hdr->d_compr;

        dir->dirent.d_csize = dir->hdr->d_csize;
        dir->dirent.st_size = dir->hdr->d_usize;

        if (! dir->hdr->d_reclen) dir->hdr = 0;
        else  dir->hdr = (struct zzip_dir_hdr *)((char *)dir->hdr + dir->hdr->d_reclen);
    }
    return &dir->dirent;
}

/**
 * see => telldir(2)
 */
zzip_off_t 
zzip_telldir(ZZIP_DIR* dir)
{
    if (! dir) { errno=EBADF; return -1; }

#  ifdef ZZIP_HAVE_DIRENT_H
    if (dir->realdir)
    {
        return telldir(dir->realdir);
    }else
#  endif
    {
    	return ((zzip_off_t) dir->hdr);
    }
}

/**
 * see => seekdir(2)
 */
void
zzip_seekdir(ZZIP_DIR* dir, zzip_off_t offset)
{
    if (! dir) return; 
    
#  ifdef ZZIP_HAVE_DIRENT_H
    if (dir->realdir)
    {
        seekdir(dir->realdir, offset);
    }else
#  endif
    {
        if (! offset)
            dir->hdr = dir->hdr0 ? dir->hdr0 : 0;
        else
            dir->hdr = (struct zzip_dir_hdr*) offset;
    }
}

/**
 * see => opendir(3). 
 * <p>
 * This function has some magic - if the given argument-path
 * is a directory, it will wrap a real => opendir(3) into the ZZIP_DIR
 * structure. Otherwise it will divert to => zzip_dir_open which 
 * can also attach a .zip extension if needed to find the archive.
 * <p>
 * the error-code is mapped to => errno(3).
 */
ZZIP_DIR* 
zzip_opendir(zzip_char_t* filename)
{
    return zzip_opendir_ext_io (filename, 0, 0);
}

/**
 * see => zzip_opendir magic function for => opendir(3)
 * <p>
 * here we use explicit ext and io instead of the internal defaults
 */
ZZIP_DIR* 
zzip_opendir_ext_io(zzip_char_t* filename, 
		    zzip_strings_t* ext, zzip_plugin_io_t io)
{
    zzip_error_t e;
    ZZIP_DIR* dir;

#  ifdef ZZIP_HAVE_SYS_STAT_H
    struct stat st;
    
    if (stat(filename, &st) >= 0
#ifdef _MSC_VER
     && (st.st_mode & S_IFDIR))
#else
     && S_ISDIR(st.st_mode))
#endif
    {
#     ifdef ZZIP_HAVE_DIRENT_H
    	DIR* realdir = opendir(filename);
        if (realdir)
        {
            if (! (dir = (ZZIP_DIR *)calloc(1, sizeof (*dir))))
            { 
                closedir(realdir); 
                return 0; 
            }else
            { 
                dir->realdir = realdir; 
                dir->realname = strdup(filename);
                return dir; 
            }
        }
#     endif
        return 0;
    }else
#  endif
    {
        dir = zzip_dir_open_ext_io(filename, &e, ext, io);
        if (! e) return dir;
        errno = zzip_errno(e); return dir;
    }
}

/**
 * see => closedir(3).
 * <p>
 * This function is magic - if the given arg-ZZIP_DIR
 * is a real directory, it will call the real => closedir(3) and then
 * free the wrapping ZZIP_DIR structure. Otherwise it will divert 
 * to => zzip_dir_close which will free the ZZIP_DIR structure.
 */
int
zzip_closedir(ZZIP_DIR* dir)
{
    if (! dir) { errno = EBADF; return -1; }

#  ifdef ZZIP_HAVE_DIRENT_H
    if (dir->realdir)
    {
        closedir(dir->realdir);
        free(dir->realname);
        free(dir);
        return 0;
    }else
#  endif
    {
        zzip_dir_close(dir);
        return 0;
    }
}

/* 
 * Local variables:
 * c-file-style: "stroustrup"
 * End:
 */
