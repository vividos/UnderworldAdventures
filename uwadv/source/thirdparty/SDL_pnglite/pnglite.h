/*  pnglite.h - Interface for pnglite library
    Copyright (c) 2007 Daniel Karling
        Copyright (c) 2019 Alexander Sabourenkov

    This software is provided 'as-is', without any express or implied
    warranty. In no event will the authors be held liable for any damages
    arising from the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
       claim that you wrote the original software. If you use this software
       in a product, an acknowledgment in the product documentation would be
       appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
       misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
       distribution.

    Daniel Karling
    daniel.karling@gmail.com
 */

#ifndef _PNGLITE_H_
#define _PNGLITE_H_

#ifdef __cplusplus
extern "C" {
#endif

/*  Enumerations for pnglite. */


/*  Return values.
    Negative numbers are error codes and 0 and up are okay responses. */
enum
{
    PNG_DONE                =  1,
    PNG_NO_ERROR            =  0,
    PNG_FILE_ERROR          = -1,
    PNG_HEADER_ERROR        = -2,
    PNG_IO_ERROR            = -3,
    PNG_EOF_ERROR           = -4,
    PNG_CRC_ERROR           = -5,
    PNG_MEMORY_ERROR        = -6,
    PNG_ZLIB_ERROR          = -7,
    PNG_UNKNOWN_FILTER      = -8,
    PNG_TRNS_WRONG_COLORTYPE = -9,
    PNG_NOT_SUPPORTED_16    = -10,
    PNG_CORRUPTED           = -11,
    PNG_WRONG_ARGUMENTS     = -12,
    PNG_IMAGE_TOO_BIG       = -13,
    PNG_OVERSIZE_CHUNK      = -14
};

/* The five different kinds of color storage in PNG files. */
enum {
    PNG_GREYSCALE           = 0,
    PNG_TRUECOLOR           = 2,
    PNG_INDEXED             = 3,
    PNG_GREYSCALE_ALPHA     = 4,
    PNG_TRUECOLOR_ALPHA     = 6
};

/* PNG filter types */
enum {
    PNG_FILTER_NONE         = 0,
    PNG_FILTER_SUB          = 1,
    PNG_FILTER_UP           = 2,
    PNG_FILTER_AVERAGE      = 3,
    PNG_FILTER_PAETH        = 4
};

/* Typedefs for callbacks. */
typedef size_t (*pnglite_read_callback_t)(void* output, size_t size, size_t numel, void* user_pointer);
typedef size_t (*pnglite_write_callback_t)(void* input, size_t size, size_t numel, void* user_pointer);
typedef void * (*pnglite_alloc_t)(size_t s);
typedef void   (*pnglite_free_t)(void* p);

typedef struct {
    void*                   zs;             /* pointer to z_stream */
    int                     zerr;           /* last zlib call status */
    const char*             zmsg;           /* message for the last err or NULL */

    pnglite_read_callback_t     read;
    pnglite_write_callback_t    write;
    pnglite_alloc_t             alloc;
    pnglite_free_t              free;
    size_t                  chunk_size_limit;
    size_t                  image_data_limit;
    void*                   user_pointer;

    unsigned char*          png_data;
    unsigned                png_datalen;

    unsigned char           palette[4*256];
    unsigned char           colorkey[6];

    unsigned                width;
    unsigned                height;
    unsigned                palette_size;
    unsigned char           depth;
    unsigned char           color_type;
    unsigned char           transparency_present;
    unsigned char           compression_method;
    unsigned char           filter_method;
    unsigned char           interlace_method;
    unsigned char           stride;
    unsigned                pitch;
} pnglite_t;

/**
 * Initializes a png_t object.
 *
 * @param png -       A png_t structure to init.
 * @param user_pointer - what to pass to reader/writer
 * @param read_fun -  a reader . may be 0 if writing
 * @param write_fun - a writer. may be 0 if reading
 * @param pngalloc -  Pointer to custom allocation routine.
 *                   If 0 is passed, malloc from libc will be used.
 * @param pngfree -   Pointer to custom free routine. If 0 is passed,
 *                   free from libc will be used.
 * @param chunk_size_limit - reject PNGs with chunks sized over this limit 0 = (1<<31)-1
 * @param image_data_limit - reject PNGs where resulting image is over this limit. same as above.
 *
 * @return PNG_WRONG_ARGUMENTS if both reader and writer are 0; PNG_NO_ERROR otherwise.
 */

int pnglite_init(pnglite_t *png, void* user_pointer,
             pnglite_read_callback_t read_fun, pnglite_read_callback_t write_fun,
             pnglite_alloc_t pngalloc, pnglite_free_t pngfree,
             size_t chunk_size_limit, size_t image_data_limit);

/**
 * Reads and checks a header from the stream.
 *
 * @param png png_t object set for reading.
 * @return PNG_NO_ERROR on success, otherwise an error code.
 */
int pnglite_read_header(pnglite_t* png);

/**
 * Writes decoded image data into given buffer.
 *
 * @param png the png_t object
 * @param data the output buffer,
 *    not less than width*height*(bytes per pixel) bytes.
 *
 * @return PNG_NO_ERROR on success, otherwise an error code.
 */
int pnglite_read_image(pnglite_t* png, unsigned char* data);

/**
 * Writes out given image data.
 *
 * @param width
 * @param height
 * @param depth
 * @param color
 * @param transparency
 * @param data
 *
 * @return PNG_NO_ERROR on success, otherwise an error code.
 */
int pnglite_write_image(pnglite_t* png, unsigned width, unsigned height, char depth, int color, int transparency, unsigned char* data);

/**
 * Returns a string representation of an error code
 *
 * @param error - Error code.
 *
 * @return pointer to string.
 */

const char* pnglite_error_string(int error);

#ifdef __cplusplus
}
#endif
#endif
