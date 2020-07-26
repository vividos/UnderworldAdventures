/*  pnglite.c - pnglite library
    For conditions of distribution and use, see copyright notice in pnglite.h
*/

#include <stdlib.h>
#include <string.h>

#include "zlib.h"
#include "pnglite.h"

static size_t
file_read(pnglite_t* png, void* out, size_t size, size_t numel)
{
    size_t result = 0;
    if(png->read) {
        result = png->read(out, size, numel, png->user_pointer);
    }
    return result;
}

static size_t
file_write(pnglite_t* png, void* p, size_t size, size_t numel)
{
    size_t result = 0;

    if(png->write) {
        result = png->write(p, size, numel, png->user_pointer);
    }
    return result;
}

static int
file_read_ul(pnglite_t* png, unsigned *out)
{
    unsigned char buf[4];

    if(file_read(png, buf, 1, 4) != 4)
        return PNG_FILE_ERROR;

    *out = (buf[0]<<24) | (buf[1]<<16) | (buf[2]<<8) | buf[3];

    return PNG_NO_ERROR;
}

static int
file_write_ul(pnglite_t* png, unsigned in)
{
    unsigned char buf[4];

    buf[0] = (in>>24) & 0xff;
    buf[1] = (in>>16) & 0xff;
    buf[2] = (in>>8) & 0xff;
    buf[3] = (in) & 0xff;

    if(file_write(png, buf, 1, 4) != 4)
        return PNG_FILE_ERROR;

    return PNG_NO_ERROR;
}


static unsigned
get_ul(unsigned char* buf)
{
    unsigned result;
    unsigned char foo[4];

    memcpy(foo, buf, 4);

    result = (foo[0]<<24) | (foo[1]<<16) | (foo[2]<<8) | foo[3];

    return result;
}

static unsigned
set_ul(unsigned char* buf, unsigned in)
{
    buf[0] = (in>>24) & 0xff;
    buf[1] = (in>>16) & 0xff;
    buf[2] = (in>>8) & 0xff;
    buf[3] = (in) & 0xff;

    return PNG_NO_ERROR;
}

int
pnglite_init(pnglite_t *png, void* user_pointer,
         pnglite_read_callback_t read_fun, pnglite_read_callback_t write_fun,
         pnglite_alloc_t pngalloc, pnglite_free_t pngfree, size_t csl, size_t idl)
{
    if(pngalloc)
        png->alloc = pngalloc;
    else
        png->alloc = malloc;

    if(pngfree)
        png->free = pngfree;
    else
        png->free = free;

    png->read = read_fun;
    png->write = write_fun;
    const size_t chunk_size_max = (1UL<<31) - 1;
    png->chunk_size_limit = (csl != 0 && csl < chunk_size_max) ? csl: chunk_size_max;
    png->image_data_limit = (idl != 0 && idl < chunk_size_max) ? idl: chunk_size_max;
    png->user_pointer = user_pointer;

    return PNG_NO_ERROR;
}

static int
png_init_copy(const pnglite_t *src, pnglite_t *dst)
{
    return pnglite_init(dst, src->user_pointer, src->read, src->write, src->alloc, src->free, src->chunk_size_limit, src->image_data_limit);
}

static int
pot_align(int value, int pot)
{
    return (value + (1<<pot) - 1) & ~((1<<pot) -1);
}

const int channels[] = { 1, 0, 3, 1, 2, 0, 4 };
#ifdef TRACE
const char *ctnames[] = { "Y", "(undefined)", "RGB", "INDEXED", "YA", "(undefined)", "RGBA" };
#endif

static int
bytes_per_pixel(int depth, int color_type) {
    return pot_align(depth * channels[color_type], 3) >> 3;
}
static int
bytes_per_scanline(int width, int depth, int color_type)
{
    return pot_align(width * depth * channels[color_type], 3) >> 3;
}

static int
get_decompressed_data_size(pnglite_t *png)
{
/*
   zlib output buffer size for interlaced pngs:
   we have more scanlines than h would indicate:

   1 6 4 6 2 6 4 6
   7 7 7 7 7 7 7 7
   5 6 5 6 5 6 5 6
   7 7 7 7 7 7 7 7
   3 6 4 6 3 6 4 6
   7 7 7 7 7 7 7 7
   5 6 5 6 5 6 5 6
   7 7 7 7 7 7 7 7

   h/2 7th pass w   scanlines  : at least one for h>=2 images.
   h/2 6th pass w/2 scanlines  : at least one for w>=2 images
   h/4 5th pass w/2 scanlines  : at least one for h>=3 images.
   h/4 4th pass w/4 scanlines  : at least one for w>=3 images.
   h/8 3rd pass w/4 scanlines  : at least one for h>=5 images.
   h/8 2nd pass w/8 scanlines  : at least one for w>=5 images.
   h/8 1st pass w/8 scanlines  : at least one at all times.

   this function overestimates needed buffer size
   but hopefully not by much
*/
/* 1bpp 4x4.
   1 6 4 6
   7 7 7 7
   5 6 5 6
   7 7 7 7


   1 = 1sl 1px = 2 bytes
   4 = 1sl 1px = 2 bytes
   5 = 1sl 2px = 2 bytes
   6 = 2sl 2px ea = 4 bytes
   7 = 2sl 4px ea = 4 bytes
   total: 14 bytes.
 */
    int rv;
    if (png->interlace_method == 0) {
        rv = png->height * (png->pitch + 1);
    } else {
        int width = png->width;
        int height = png->height;
        int depth = png->depth;
        int ct = png->color_type;

        rv =
            (bytes_per_scanline( width,      depth, ct) + 1) * (1 + height/2) +
            (bytes_per_scanline((width+1)/2, depth, ct) + 1) * (1 + height/2) +
            (bytes_per_scanline((width+1)/2, depth, ct) + 1) * (1 + height/4) +
            (bytes_per_scanline((width+1)/4, depth, ct) + 1) * (1 + height/4) +
            (bytes_per_scanline((width+1)/4, depth, ct) + 1) * (1 + height/8) +
            (bytes_per_scanline((width+1)/8, depth, ct) + 1) * (1 + height/8) +
            (bytes_per_scanline((width+1)/8, depth, ct) + 1) * (1 + height/8);
    }
#ifdef TRACE
    fprintf(stderr, "png_decompressed_data_size(): decompressed_data_size estimated at %d\n", rv);
#endif
    return rv;
}

static int
png_check_png(pnglite_t* png)
{
    if (png->width == 0 || png->height == 0) {
        return PNG_CORRUPTED;
    }
    switch(png->depth) {
    case 1:
    case 2:
    case 4:
    case 8:
        break;
    case 16:
        return PNG_NOT_SUPPORTED_16;
    default:
        return PNG_CORRUPTED;
    }

    switch(png->color_type) {
    case PNG_GREYSCALE:
        break;
    case PNG_TRUECOLOR:
    case PNG_GREYSCALE_ALPHA:
    case PNG_TRUECOLOR_ALPHA:
        if (png->depth < 8)
            return PNG_CORRUPTED;
        break;
    case PNG_INDEXED:
        if (png->depth > 8)
            return PNG_CORRUPTED;
        break;
    default:
        return PNG_CORRUPTED;
    }

    if (png->compression_method != 0) {
        return PNG_CORRUPTED;
    }
    if (png->filter_method != 0) {
        return PNG_CORRUPTED;
    }

    switch(png->interlace_method) {
    case 0:
    case 1:
        break;
    default:
        return PNG_CORRUPTED;
    }

    /* bytes per pixel or one (unpacked) */
    png->stride = bytes_per_pixel(png->depth, png->color_type);

    /* bytes per scanline (packed) */
    png->pitch = bytes_per_scanline(png->width, png->depth, png->color_type);

#ifdef TRACE
    fprintf(stderr, "png_check_png(): %dx%d depth=%d ctype=%s channels=%d interlace=%d stride=%d pitch=%d\n",
           png->width, png->height, png->depth, ctnames[png->color_type],
           channels[png->color_type], png->interlace_method, png->stride, png->pitch);
#endif

    if (png->stride * png->width * png->height > png->image_data_limit) {
#ifdef TRACE
        fprintf(stderr, "png_check_png(): image size over limit (%d), aborting.\n", png->image_data_limit);
#endif
        return PNG_IMAGE_TOO_BIG;
    }

    return PNG_NO_ERROR;
}

static unsigned
png_calc_crc(char *name, unsigned char *chunk, unsigned length)
{
    unsigned crc;

    crc = crc32(0L, Z_NULL, 0);
    crc = crc32(crc, (unsigned char *)name, 4);
    crc = crc32(crc, chunk, length);

    return crc;
}

static int
png_read_check_crc(pnglite_t *png, char *name, unsigned char *chunk, unsigned length)
{
    unsigned crc;

    if (file_read_ul(png, &crc) != PNG_NO_ERROR)
        return PNG_EOF_ERROR;

    if(crc != png_calc_crc(name, chunk, length))
        return PNG_CRC_ERROR;

    return PNG_NO_ERROR;
}

static int
png_calc_write_crc(pnglite_t *png, char *name, unsigned char *chunk, unsigned length)
{
    unsigned crc;

    crc = png_calc_crc(name, chunk, length);

    if (file_write_ul(png, crc) != PNG_NO_ERROR)
        return PNG_IO_ERROR;

    return PNG_NO_ERROR;
}

static int
png_read_ihdr(pnglite_t* png)
{
    int rv;
    unsigned length;
    unsigned char ihdr[13 + 4]; /* length should be 13, make room for type (IHDR) */

    if ((rv = file_read_ul(png, &length)) != PNG_NO_ERROR)
        return rv;

    if(length != 13)
        return PNG_CRC_ERROR;

    if(file_read(png, ihdr, 1, 13 + 4) != 13 + 4)
        return PNG_EOF_ERROR;

    if (png_read_check_crc(png, "IHDR", ihdr + 4, 13) != PNG_NO_ERROR)
        return PNG_CRC_ERROR;

    png->width = get_ul(ihdr+4);
    png->height = get_ul(ihdr+8);
    png->depth = ihdr[12];
    png->color_type = ihdr[13];
    png->compression_method = ihdr[14];
    png->filter_method = ihdr[15];
    png->interlace_method = ihdr[16];

    return png_check_png(png);
}

static int
png_write_ihdr(pnglite_t* png)
{
    unsigned char ihdr[13 + 4];
    unsigned char *p = ihdr;

    file_write(png, "\x89\x50\x4E\x47\x0D\x0A\x1A\x0A", 1, 8);

    file_write_ul(png, 13);

    *p++ = 'I';
    *p++ = 'H';
    *p++ = 'D';
    *p++ = 'R';
    set_ul(p, png->width);
    p += 4;
    set_ul(p, png->height);
    p += 4;
    *p++ = png->depth;
    *p++ = png->color_type;
    *p++ = 0;
    *p++ = 0;
    *p++ = 0;

    file_write(png, ihdr, 1, 13 + 4);

    return png_calc_write_crc(png, "IHDR", ihdr + 4, 13);
}

int
pnglite_read_header(pnglite_t* png)
{
    char header[8];
    int result;

    if (!png->read)
        return PNG_WRONG_ARGUMENTS;

    if (file_read(png, header, 1, 8) != 8)
        return PNG_EOF_ERROR;

    if (memcmp(header, "\x89\x50\x4E\x47\x0D\x0A\x1A\x0A", 8) != 0)
        return PNG_HEADER_ERROR;

    result = png_read_ihdr(png);

    return result;
}

static void *
z_alloc_func(void *png, uInt items, uInt size)
{
    return ((pnglite_t *)png)->alloc(items*size);
}

static void
z_free_func(void *png, void *ptr)
{
    ((pnglite_t *)png)->free(ptr);
}

static int
png_init_inflate(pnglite_t* png)
{
    z_stream *stream;
    png->zs = png->alloc(sizeof(z_stream));

    stream = png->zs;

    if(!stream)
        return PNG_MEMORY_ERROR;

    memset(stream, 0, sizeof(z_stream));
    stream->opaque = png;
    stream->zalloc = z_alloc_func;
    stream->zfree = z_free_func;

    if( (png->zerr= inflateInit(stream)) != Z_OK) {
        return PNG_ZLIB_ERROR;
    }

    stream->next_out = png->png_data;
    stream->avail_out = png->png_datalen;

    return PNG_NO_ERROR;
}

static int
png_end_inflate(pnglite_t* png)
{
    int result = PNG_NO_ERROR;
    z_stream *stream = png->zs;

    if(!stream) { return PNG_MEMORY_ERROR; }

#ifdef TRACE
    fprintf(stderr, "png_end_inflate(): decompressed total_out: %lu\n", stream->total_out);
#endif
    if((png->zerr = inflateEnd(stream)) != Z_OK) {
        png->zmsg = stream->msg;
        result = PNG_ZLIB_ERROR;
    }

    png->free(png->zs);

    return result;
}

static int
png_inflate(pnglite_t* png, unsigned char* data, int len)
{
    z_stream *stream = png->zs;

    if(!stream)
        return PNG_MEMORY_ERROR;

    stream->next_in = (unsigned char*)data;
    stream->avail_in = len;

    png->zerr = inflate(stream, Z_SYNC_FLUSH);

    if(png->zerr != Z_STREAM_END && png->zerr != Z_OK) {
#ifdef TRACE
        fprintf(stderr, "png_inflate(): zlib error: %s\n", stream->msg);
#endif
        png->zmsg = stream->msg;
        return PNG_ZLIB_ERROR;
    }

    if(stream->avail_in != 0) { /* FIXME: not an error exactly. a warning ? */
#ifdef TRACE
        fprintf(stderr, "png_inflate(): zlib error: stream->avail_in != 0 : %d\n", stream->avail_in);
        fprintf(stderr, "png_inflate(): len was %d; total_out = %lu\n", len, stream->total_out);
        unsigned char buf[4096];
        stream->next_out = buf;
        stream->avail_out = 4096;
        int res = inflate(stream, Z_SYNC_FLUSH);
        fprintf(stderr, "png_inflate(): another pass; res=%d (%s), total_out=%lu\n", res,stream->msg, stream->total_out);
#endif
        return PNG_ZLIB_ERROR;
    }

    return PNG_NO_ERROR;
}


static int
png_write_idats(pnglite_t* png, unsigned char* data)
{
    unsigned char *idat;
    unsigned long  written;
    unsigned crc;
    unsigned size;
    int err;

    size = 8 + compressBound(png->height * png->pitch) + 4;
    idat = png->alloc(size);

    if (!idat) {
        err = PNG_MEMORY_ERROR;
        goto done;
    }

    memcpy(idat + 4, "IDAT", 4);

    written = size - 12;

    png->zerr = compress(idat + 8, &written, data, png->height * ( png->pitch + 1));
    if (png->zerr != Z_OK) {
        err = PNG_ZLIB_ERROR;
        goto done;
    }

    set_ul(idat, written);
    crc = png_calc_crc("IDAT", idat + 8, written);

    if (file_write(png, idat, written + 8, 1) != 1) {
        err = PNG_IO_ERROR;
        goto done;
    }
    if (PNG_NO_ERROR != file_write_ul(png, crc)) {
        err = PNG_IO_ERROR;
        goto done;
    }

    /* write IEND chunk */
    file_write_ul(png, 0);
    file_write(png, "IEND", 1, 4);
    crc = crc32(0L, (const unsigned char *)"IEND", 4);
    file_write_ul(png, crc);

    err =  PNG_NO_ERROR;

  done:
    png->free(idat);
    return err;
}

static int
png_read_idat(pnglite_t* png, unsigned firstlen)
{
    unsigned type = 0;
    unsigned char *chunk;
    int result;
    unsigned length = firstlen;
    unsigned old_len = length;

    chunk = png->alloc(firstlen);

    if (!chunk)
        return PNG_MEMORY_ERROR;

    result = png_init_inflate(png);

    if(result != PNG_NO_ERROR) {
        png_end_inflate(png);
        png->free(chunk);
        return result;
    }

    do {
        if(file_read(png, chunk, 1, length) != length) {
            png_end_inflate(png);
            png->free(chunk);
            return PNG_FILE_ERROR;
        }

        if (png_read_check_crc(png, "IDAT", chunk, length)) {
            result = PNG_CRC_ERROR;
            break;
        }

        result = png_inflate(png, chunk, length);

        if (result != PNG_NO_ERROR)
            break;

        if ((result = file_read_ul(png, &length)) != PNG_NO_ERROR)
            break;

        if (length > png->chunk_size_limit) {
#ifdef TRACE
            fprintf(stderr, "png_read_idat(): aborting on chunk size %d (over user limit of %d)\n", length, chunk_size_limit);
#endif
            png->free(chunk);
            return PNG_OVERSIZE_CHUNK;

        }

        if(length > old_len) {
            png->free(chunk);
            chunk = png->alloc(length);
            if (!chunk) {
                result = PNG_MEMORY_ERROR;
                break;
            }
            old_len = length;
        }

        if(file_read(png, &type, 1, 4) != 4) {
            result = PNG_IO_ERROR;
            break;
        }

    } while(type == *(unsigned int*)"IDAT");

    if(type == *(unsigned int*)"IEND") {
        result = PNG_DONE;
    } else {
#ifdef TRACE
        char *chtype = (char *)(&type);
        fprintf(stderr, "png_read_idat() unexpected chunk '%c%c%c%c' after an IDAT\n", chtype[0], chtype[1], chtype[2], chtype[3]);
#endif
        result = PNG_CORRUPTED;
    }

    png->free(chunk);
    png_end_inflate(png);

    return result;
}

static int
png_process_chunk(pnglite_t* png)
{
    int result = PNG_NO_ERROR;
    unsigned type;
    unsigned length;

    if (file_read_ul(png, &length) != PNG_NO_ERROR) { return PNG_EOF_ERROR; }

    if (file_read(png, &type, 4, 1) != 1) { return PNG_EOF_ERROR; }

    if (length > png->chunk_size_limit) {
#ifdef TRACE
        char *chtype = (char *)(&type);
        fprintf(stderr, "png_process_chunk() aborting on chunk '%c%c%c%c' length %u : over chunk size limit %u\n",
               chtype[0], chtype[1], chtype[2], chtype[3], length, chunk_size_limit);
#endif
        return PNG_OVERSIZE_CHUNK;
    }
    if (type == *(unsigned int *) "PLTE") {
        if (length % 3)
            return PNG_CORRUPTED;

        png->palette_size = length / 3;

        memset(png->palette, 255, 1024);

        if (file_read(png, png->palette, length, 1) != 1)
            return PNG_EOF_ERROR;

        if (png_read_check_crc(png, "PLTE", png->palette, length) != PNG_NO_ERROR)
            return PNG_CRC_ERROR;

    } else if (type == *(unsigned int*)"tRNS") {
        png->transparency_present = 1;
        switch (png->color_type) {
        case PNG_INDEXED:
            /* no PLTE seen before tRNS */
            if (png->palette_size == 0)
                return PNG_CORRUPTED;

            if (length > 256)
                return PNG_CORRUPTED;

            if (file_read(png, png->palette + 768, length, 1) != 1)
                return PNG_EOF_ERROR;

            return png_read_check_crc(png, "tRNS", png->palette + 768, length);

        case PNG_TRUECOLOR:
            if (length != 6)
                return PNG_CORRUPTED;

            if (file_read(png, png->colorkey, length, 1) != 1)
                return PNG_EOF_ERROR;

            if (png_read_check_crc(png, "tRNS", png->colorkey, length) != PNG_NO_ERROR)
                return PNG_CRC_ERROR;

            return PNG_NO_ERROR;

        case PNG_GREYSCALE:
            if (length != 2)
                return PNG_CORRUPTED;

            file_read(png, png->colorkey, length, 1);

            if (png_read_check_crc(png, "tRNS", png->colorkey, length) != PNG_NO_ERROR)
                return PNG_CRC_ERROR;

            return PNG_NO_ERROR;

        default:
            return PNG_CORRUPTED;
        }
    } else if (type == *(unsigned int*)"IDAT") {
        /* PNG_INDEXED has to have PLTE before IDAT */
        if ((png->color_type == PNG_INDEXED) && (png->palette_size == 0)) {
#ifdef TRACE
            fprintf(stderr, "No PLTE before IDAT in PNG_INDEXED\n");
#endif
            return PNG_CORRUPTED;
        }

        /*  if we found an idat, all other idats should follow
            with no other chunks in between */
        png->png_datalen = get_decompressed_data_size(png);
        png->png_data = png->alloc(png->png_datalen);

        if(!png->png_data)
            return PNG_MEMORY_ERROR;

        return png_read_idat(png, length);
    } else if (type == *(unsigned int*)"IEND") {
        return PNG_DONE;
    } else {
#ifdef TRACE
        char *chunk = (char*)(&type);
        fprintf(stderr, "png_process_chunk(): skipping '%c%c%c%c' of %d bytes\n", chunk[0],  chunk[1], chunk[2], chunk[3], length);
#endif
        if (file_read(png, 0, length + 4, 1) != 1) /* unknown chunk */
            return PNG_EOF_ERROR;
    }

    return result;
}

static int
png_write_plte(pnglite_t *png)
{
    unsigned char plte[4 + 4 + 768 + 4];
    unsigned i;
    const unsigned length = png->palette_size * 3;

    if ((png->palette_size > 256) || (png->palette_size == 0))
        return PNG_CORRUPTED;

    memcpy(plte + 4, "PLTE", 4);
    for(i = 0 ; i < png->palette_size; i++) {
        plte[8 + 3*i + 0] = png->palette[4*i + 0];
        plte[8 + 3*i + 1] = png->palette[4*i + 1];
        plte[8 + 3*i + 2] = png->palette[4*i + 2];
    }

    set_ul(plte, length);

    if (file_write(png, plte, length + 8, 1) != 1)
        return PNG_IO_ERROR;

    return png_calc_write_crc(png, "PLTE", plte + 8, length);
}

static int
png_write_trns(pnglite_t *png)
{
    unsigned char trns[4 + 4 + 256 + 4];
    unsigned i;
    unsigned length;

    switch (png->color_type) {
    case PNG_INDEXED:
        length = png->palette_size;
        for(i = 0 ; i < png->palette_size; i++)
            trns[8 + i] = png->palette[4*i + 3];
        break;

    case PNG_GREYSCALE:
        length = 2;
        trns[8 + 0] = 0;
        trns[8 + 1] = png->colorkey[1];
        break;

    case PNG_TRUECOLOR:
        length = 6;
        trns[8 + 0] = 0;
        trns[8 + 1] = png->colorkey[1];
        trns[8 + 2] = 0;
        trns[8 + 3] = png->colorkey[3];
        trns[8 + 4] = 0;
        trns[8 + 5] = png->colorkey[5];
        break;

    default:
        return PNG_TRNS_WRONG_COLORTYPE;
    }
    memmove(trns + 4, "tRNS", 4);
    set_ul(trns, length);

    if (file_write(png, trns, length + 8, 1) != 1)
        return PNG_IO_ERROR;

    return png_calc_write_crc(png, "tRNS", trns + 8, length);
}

static unsigned char
png_paeth_predictor(unsigned char a, unsigned char b, unsigned char c)
{
    int p = (int)a + b - c;
    int pa = abs(p - a);
    int pb = abs(p - b);
    int pc = abs(p - c);
    int pr;

    if(pa <= pb && pa <= pc)
        pr = a;
    else if(pb <= pc)
        pr = b;
    else
        pr = c;

    return (char)pr;
}

static int
png_filter(pnglite_t* png, unsigned char* data)
{
    (void)(png); (void)(data);
    return PNG_NO_ERROR;
}

static int
png_unfilter(pnglite_t* png, unsigned char* data)
{
    unsigned i, p, t;
    unsigned char a, b, c;
    unsigned char *filtered;
    unsigned char *reconstructed;
    unsigned char *up_reconstructed;
    unsigned char filter_type;
    for(i = 0; i < png->height ; i++) {
        filtered = png->png_data + (png->pitch + 1) * i;
        reconstructed = data + png->pitch * i;
        up_reconstructed = i > 0 ? data + png->pitch * (i - 1) : 0;
        filter_type = *filtered++;
        switch(filter_type) {
        case PNG_FILTER_NONE:
            memcpy(reconstructed, filtered, png->pitch);
            break;

        case PNG_FILTER_SUB:
            memcpy(reconstructed, filtered, png->stride);
            for (p = png->stride; p < png->pitch ; p++)
                reconstructed[p] = filtered[p]
                                        + reconstructed[p - png->stride];
            break;

        case PNG_FILTER_UP:
            if (up_reconstructed)
                for (p = 0; p < png->pitch ; p++)
                    reconstructed[p] = filtered[p] + up_reconstructed[p];
            else
                memcpy(reconstructed, filtered, png->pitch);
            break;

        case PNG_FILTER_AVERAGE:
            for (p = 0; p < png->pitch ; p++) {
                b = up_reconstructed ? up_reconstructed[p] : 0;
                a = p >= png->stride ? reconstructed[p - png->stride] : 0;
                t = a + b;
                reconstructed[p] = filtered[p] + t/2;
            }
            break;

        case PNG_FILTER_PAETH:
            for (p = 0; p < png->pitch ; p++) {
                c = (up_reconstructed && (p >= png->stride)) ?
                                    up_reconstructed[p - png->stride] : 0;
                b = up_reconstructed ? up_reconstructed[p] : 0;
                a = p >= png->stride ? reconstructed[p - png->stride] : 0;
                reconstructed[p] = filtered[p] + png_paeth_predictor(a, b, c);
            }
            break;

        default:
#ifdef TRACE
            fprintf(stderr, "png_unfilter sl=%d unknown filter type %d\n", i, (int)filter_type);
#endif
            return PNG_UNKNOWN_FILTER;
        }
    }
    return PNG_NO_ERROR;
}

static void
png_unpack_byte(unsigned char *dst, unsigned char *src, int depth)
{
    switch (depth) {
    case 1:
        *dst++ = (*src & 0x80) ? 1 : 0;
        *dst++ = (*src & 0x40) ? 1 : 0;
        *dst++ = (*src & 0x20) ? 1 : 0;
        *dst++ = (*src & 0x10) ? 1 : 0;
        *dst++ = (*src & 0x08) ? 1 : 0;
        *dst++ = (*src & 0x04) ? 1 : 0;
        *dst++ = (*src & 0x02) ? 1 : 0;
        *dst++ = (*src & 0x01) ? 1 : 0;
        break;
    case 2:
        *dst++ = (*src & 0xC0) >> 6;
        *dst++ = (*src & 0x30) >> 4;
        *dst++ = (*src & 0x0C) >> 2;
        *dst++ = (*src & 0x03);
        break;
    case 4:
        *dst++ = (*src & 0xF0) >> 4;
        *dst++ = (*src & 0x0F);
        break;
    default:
        /* can't be, we caught this in read_ihdr */
        break;
    }
}

static int
png_unfilter_unpack(pnglite_t *png, unsigned char *data)
{
    int result;
    unsigned char *packed_pixels;
    unsigned char *packed_data;
    unsigned char *unpacked_row;
    unsigned row, offset;
    unsigned char tail[8];
    const unsigned pipeby = 8 / png->depth; /* pixels per byte */
#ifdef TRACE
    fprintf(stderr, "png_unfilter_unpack(): got data=%p\n", data);
#endif
    if (png->depth < 8) {
        packed_data = png->alloc(png->height * png->pitch);

        if (!packed_data)
            return PNG_MEMORY_ERROR;

        result = png_unfilter(png, packed_data);

        if (result != PNG_NO_ERROR) {
            png->free(packed_data);
            return result;
        }

        for (row = 0; row < png->height; row++) {
            packed_pixels = packed_data + row * png->pitch;
            unpacked_row = data + png->width*row;

            if (png->width % pipeby) {
                for (offset = 0; offset + pipeby < png->width; offset += pipeby)
                    png_unpack_byte(unpacked_row + offset, packed_pixels++,
                                    png->depth);

                /*  here *packed_pixels is the last byte of data for the row,
                    unpacked_row + offset points to the last unpacked pixels
                    in the row. There are png->width % pipeby of them. */
                png_unpack_byte(tail, packed_pixels, png->depth);
#ifdef TRACE
                fprintf(stderr, "png_unfilter_unpack(): copy unpacked tail at offs %d len %d unpacked_row %p: ",
                        offset, png->width % pipeby, (void *)unpacked_row);
#endif
                memcpy(unpacked_row + offset, tail, png->width % pipeby);
#ifdef TRACE
                for (unsigned ii = 0; ii < png->width % pipeby; ii++) { fprintf(stderr, "%02x ", (int)(tail[ii])); }
                fprintf(stderr, "\n");
#endif
            } else {
                for (offset = 0; offset < png->width; offset += pipeby)
                    png_unpack_byte(unpacked_row + offset, packed_pixels++,
                                    png->depth);
            }
        }

        png->free(packed_data);
    } else {
        result = png_unfilter(png, data);
    }
    return result;
}

/* Intentionally done the dumbest way possible. Who ever interlaces PNGs nowadays? Focus on correctness */
static int
png_deinterlace(pnglite_t* png, unsigned char *data)
{
    int result = PNG_NO_ERROR;
    pnglite_t subpng;
    /* set up invariants for subimages */
    png_init_copy(png, &subpng);
    subpng.color_type = png->color_type;
    subpng.filter_method = png->filter_method;
    subpng.compression_method = 0;
    subpng.depth = png->depth;
    subpng.interlace_method = 0;
    subpng.stride = png->stride;
    int stride = subpng.stride; /* bytes per unpacked pixel */

/* Adam7
   1 6 4 6 2 6 4 6
   7 7 7 7 7 7 7 7
   5 6 5 6 5 6 5 6
   7 7 7 7 7 7 7 7
   3 6 4 6 3 6 4 6
   7 7 7 7 7 7 7 7
   5 6 5 6 5 6 5 6
   7 7 7 7 7 7 7 7
 */
    /* pass no                       1  2  3  4  5  6  7 */
    const unsigned int hstride[] = { 8, 8, 4, 4, 2, 2, 1 };
    const unsigned int vstride[] = { 8, 8, 8, 4, 4, 2, 2 };
    const unsigned int hshift[]  = { 0, 4, 0, 2, 0, 1, 0 };
    const unsigned int vshift[]  = { 0, 0, 4, 0, 2, 0, 1 };

    unsigned int offset = 0, x, y;
    unsigned char* subdata = NULL;
    int pass = 0;
#ifdef TRACE
    int n = 0, maxoffs = 0;
# ifndef TRACE_DEINTERLACE
    memset(data, 0x23, png->width * png->height * png->stride);
# endif
#endif
    /* allocate subdata for the last pass, that would be all the most we need for any of the passes */
    int subdata_max = (png->width * bytes_per_pixel(png->depth, png->color_type)) * ( png->height/2 + 1);
    subdata = png->alloc(subdata_max);
    if (subdata == NULL) {
        return PNG_MEMORY_ERROR;
    }
    do {
#ifdef TRACE_DEINTERLACE_DESTRUCTIVE
        /* make it so that deinterlace passes are not additive; breaks output */
        memset(data, 0x23, png->width * png->height * png->stride);
#endif

        /* see if we're to skip this pass if the image is too small */
        if ((hshift[pass] >= png->width) || (vshift[pass] >= png->height)) {
            /* this way we don't get to have a scanline */
            pass += 1;
            continue;
        }
        /* now, (png->width - hshift[pass] + hstride[pass] - 1) / hstride[pass]
         *     would that be the number of expected columns? Yup.
         *  same for scanlines.
         *
         * set up a temporary png_t for this subimage
         */
        subpng.width = (png->width - hshift[pass] + hstride[pass] - 1) / hstride[pass];
        subpng.height = (png->height - vshift[pass] + vstride[pass] - 1) / vstride[pass];
        /* make sure we get at least one scanline and column */
        subpng.width = subpng.width > 0 ? subpng.width : 1;
        subpng.height = subpng.height > 0 ? subpng.height : 1;
        /* the rest is trivial */
        subpng.pitch = bytes_per_scanline(subpng.width, subpng.depth, subpng.color_type);
        subpng.png_datalen = subpng.height * (subpng.pitch + 1);
        subpng.png_data = png->png_data + offset;
#ifdef TRACE
        fprintf(stderr, "png_deinterlace() pass %d: subimage %dx%d pitch=%d datalen=%d offset=%d from %p\n",
                pass + 1, subpng.width, subpng.height, subpng.pitch , subpng.png_datalen,
                offset, (void *)subpng.png_data);
#endif
        offset += subpng.png_datalen;
        if (offset > png->png_datalen) {
#ifdef TRACE
            fprintf(stderr, "png_deinterlace() pass %d: not enough data (end-of-pass-data at %u > png_datalen %u)\n",
                   pass + 1, offset, png->png_datalen);
#endif
            png->free(subdata);
            return PNG_CORRUPTED;
        }
        result = png_unfilter_unpack(&subpng, subdata);
        if (PNG_NO_ERROR != result) {
#ifdef TRACE
            fprintf(stderr, "png_deinterlace() pass %d: freeing subdata at %p\n", pass + 1, (void *)subdata);
#endif
            png->free(subdata);
            return result;
        }
        /* not optimizing it - not worth the time */
        for (y = 0; y < subpng.height; y++) {
#ifdef TRACE_DEINTERLACE
            fprintf(stderr, "pass %d unp %d:", pass + 1, y);
#endif
            for (x = 0; x < subpng.width; x++) {
                for (int bi = 0; bi < png->stride; bi++) {
                    int destx = x * hstride[pass] + hshift[pass];
                    int desty = y * vstride[pass] + vshift[pass];
                    int desti = desty*stride*png->width + destx*stride + bi;
#ifdef TRACE_DEINTERLACE
                    fprintf(stderr, "desti = %d * %d * %d + %d * %d + %d = %d\n",
                            desty, stride, png->width, destx, stride,  bi, desti);
#endif
                    int srci = y*stride*subpng.width + x*stride + bi;
                    data[desti] = subdata[srci];
#ifdef TRACE_DEINTERLACE
                    if (desti > maxoffs) { maxoffs = desti; }
                    n += 1;
                    fprintf(stderr, "    set (%d,%d) -> (%d,%d) to %d from %d at %d\n", x, y, destx, desty, (int)subdata[srci], srci, desti);
#endif
                }
            }
#ifdef TRACE_DEINTERLACE
            fprintf(stderr, "\n");
#endif
        }
#ifdef TRACE_DEINTERLACE_DESTRUCTIVE
        /* without the memset above this causes tons of uninit value errors from valgrind */
        for (y = 0; y < png->height ; y++) {
            fprintf(stderr, "png_deinterlace() pass %d row %03d: ", pass + 1, y);
            for (x= 0 ; x < png->width ; x++) {
                if (data[x*stride + y*png->height*stride] != 0x23) {
                    fprintf(stderr, "%02x ", data[x*stride + y*png->height*stride]);
                } else {
                    fprintf(stderr, "__ ");
                }
            }
            fprintf(stderr, "\n");
        }
#endif
        pass += 1;
    } while (pass < 7);
#ifdef TRACE
    fprintf(stderr, "png_deinterlace(): %d pixels set of %dx%d =%d  at %p maxoffs=%d \n", n, png->width, png->height, png->width * png->height, data, maxoffs);
#endif
    png->free(subdata);
    return result;
}

int
pnglite_read_image(pnglite_t* png, unsigned char* data)
{
    int result = PNG_NO_ERROR;

    png->transparency_present = 0;
    png->palette_size = 0;
    png->png_data = NULL;

    while(result == PNG_NO_ERROR) {
        result = png_process_chunk(png);
    }
    if(result != PNG_DONE) {
        if (png->png_data) {
            png->free(png->png_data);
        }
        return result;
    }
    if (png->png_data == NULL) {
#ifdef TRACE
        fprintf(stderr, "png_get_data() : no IDAT chunk in file.\n");
#endif
        /* no IDAT chunk in file */
        return PNG_CORRUPTED;
    }
    if (png->interlace_method) {
        result = png_deinterlace(png, data);
    } else {
        result = png_unfilter_unpack(png, data);
    }
    png->free(png->png_data);
    return result;
}

int
pnglite_write_image(pnglite_t* png, unsigned width, unsigned height, char depth,
                        int color, int transparency, unsigned char* data)
{
    unsigned i;
    int err;
    unsigned char *filtered;

    if (!png->write) { return PNG_WRONG_ARGUMENTS; }

    png->width = width;
    png->height = height;
    png->depth = depth;
    png->color_type = color;
    png->filter_method = 0;
    png->interlace_method = 0;
    png->compression_method = 0;

    int rv_pcp = png_check_png(png);
    if (rv_pcp)
        return rv_pcp;

    filtered = png->alloc((png->pitch + 1) * height);
    if (!filtered)
        return PNG_MEMORY_ERROR;

    for(i = 0; i < png->height; i++) {
        filtered[i * (png->pitch + 1)] = 0;
        memcpy(filtered + i*(png->pitch + 1) + 1,
                        data + i*png->pitch, png->pitch);
    }
    png_filter(png, filtered);
    png_write_ihdr(png);

    if (png->color_type == PNG_INDEXED) {
        if ((err = png_write_plte(png))) {
            png->free(filtered);
            return err;
        }
    }

    if (transparency) {
        if ((err = png_write_trns(png))) {
            png->free(filtered);
            return err;
        }
    }

    png_write_idats(png, filtered);
    png->free(filtered);

    return PNG_NO_ERROR;
}

const char* pnglite_error_string(int error)
{
    switch(error) {
    case PNG_NO_ERROR:
        return "No error";
    case PNG_FILE_ERROR:
        return "Unknown file error.";
    case PNG_HEADER_ERROR:
        return "No PNG header found.";
    case PNG_IO_ERROR:
        return "Failure while reading/writing file.";
    case PNG_EOF_ERROR:
        return "Reached end of file.";
    case PNG_CRC_ERROR:
        return "CRC or chunk length error.";
    case PNG_MEMORY_ERROR:
        return "Could not allocate memory.";
    case PNG_ZLIB_ERROR:
        return "zlib reported an error.";
    case PNG_UNKNOWN_FILTER:
        return "Unknown filter method used in scanline.";
    case PNG_DONE:
        return "PNG done";
    case PNG_NOT_SUPPORTED_16:
        return "16 bits per channel PNGs are not supported.";
    case PNG_TRNS_WRONG_COLORTYPE:
        return "Transparency supplied for a color type with alpha.";
    case PNG_WRONG_ARGUMENTS:
        return "Wrong combination of arguments passed to png_open. You must"
               " use either a read_function or supply a file pointer to use.";
    case PNG_CORRUPTED:
        return "PNG data does not follow the specification or is corrupted.";
    case PNG_IMAGE_TOO_BIG:
        return "PNG image data size is over the user-set limit";
    case PNG_OVERSIZE_CHUNK:
        return "PNG chunk size is over the user-set limit";
    default:
        return "Unknown error.";
    };
}
