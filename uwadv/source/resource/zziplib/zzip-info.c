#include "zzip.h"
#include "zzip-file.h"
#include "zzipformat.h"

#ifdef ZZIP_HAVE_SYS_STAT_H
#include <sys/stat.h>
#else
#include <stdlib.h>
#include <stdio.h>
#endif

/** 
 *  return dir->errcode 
 */
int 
zzip_error(ZZIP_DIR * dir)   
{   
    return dir->errcode; 
}

/** 
 *  dir->errcode = errcode 
 */
void 
zzip_seterror(ZZIP_DIR * dir, int errcode)   
{ dir->errcode = errcode; }

/** 
 * return fp->dir 
 */
ZZIP_DIR * 
zzip_dirhandle(ZZIP_FILE * fp)   
{ 
    return fp->dir; 
}

/**
 *  return dir->fd 
 */
int 
zzip_dirfd(ZZIP_DIR* dir)   
{ 
    return dir->fd; 
}

/**
 * return static const string of the known compression methods, 
 * otherwise "zipped" is returned
 */
zzip_char_t*
zzip_compr_str(int compr)
{
    switch(compr)
    {
    case ZZIP_IS_STORED:		return "stored";
    case ZZIP_IS_SHRUNK:		return "shrunk";
    case ZZIP_IS_REDUCEDx1:
    case ZZIP_IS_REDUCEDx2:
    case ZZIP_IS_REDUCEDx3:
    case ZZIP_IS_REDUCEDx4:		return "reduced";
    case ZZIP_IS_IMPLODED:		return "imploded";
    case ZZIP_IS_TOKENIZED:		return "tokenized";
    case ZZIP_IS_DEFLATED:		return "deflated";
    case ZZIP_IS_DEFLATED_BETTER:	return "deflatedX";
    case ZZIP_IS_IMPLODED_BETTER:	return "implodedX";
    default:
        if (0 < compr && compr < 256)   return "zipped"; 
        else
        {
#	ifdef S_ISDIR
            if (S_ISDIR(compr))		return "directory";
#	endif
#	ifdef S_ISCHR
            if (S_ISCHR(compr))		return "is/chr";
#	endif
#	ifdef S_ISBLK
            if (S_ISBLK(compr))		return "is/blk";
#	endif
#	ifdef S_ISFIFO
            if (S_ISFIFO(compr))	return "is/fifo";
#	endif
#	ifdef S_ISSOCK
            if (S_ISSOCK(compr))	return "is/sock";
#	endif
#	ifdef S_ISLNK
            if (S_ISLNK(compr))		return "is/lnk";
#	endif
            return "special";
        }
    }/*switch*/
}

/**
 * return if the DIR-handle is wrapping a real directory
 * or a zip-archive. Returns 1 for a stat'able directory,
 * and 0 for a handle to zip-archive.
 */ 
int
zzip_dir_real(ZZIP_DIR* dir)
{
    return dir->realdir != 0;
}

/**
 * return if the FILE-handle is wrapping a real file
 * or a zip-contained file. Returns 1 for a stat'able file,
 * and 0 for a file inside a zip-archive.
 */
int
zzip_file_real(ZZIP_FILE* fp)
{
    return fp->dir == 0; /* ie. not dependent on a zip-arch-dir  */
}

/**
 * return the posix DIR* handle (if one exists).
 * Check before with => zzip_dir_real if the
 * the ZZIP_DIR points to a real directory.
 */
void*
zzip_realdir(ZZIP_DIR* dir)
{
    return dir->realdir;
}

/**
 * return the posix file descriptor (if one exists).
 * Check before with => zzip_file_real if the
 * the ZZIP_FILE points to a real file.
 */
int
zzip_realfd(ZZIP_FILE* fp)
{
    return fp->fd;
}

/* 
 * Local variables:
 * c-file-style: "stroustrup"
 * End:
 */
