#include <stdio.h>
#include <string.h>
#include "SDL.h"
#include "SDL_pnglite.h"
#include "SDL_image.h"

#if defined(_WIN32)
# include <stdlib.h>
char *get_basename(const char *fname, char *buf) {
    char ext[FILENAME_MAX + 1];
    strcpy(buf, fname);
    ext[0] = 0;
    _splitpath(fname, NULL, NULL, buf, ext);
    strcpy(buf + strlen(buf), ext);
    return buf;
}
#else
# include <libgen.h>
char *get_basename(const char *fname, char *buf) {
    strcpy(buf, fname);
    return basename(buf);
}
#endif

void dump_buf(const char *fname, const void *rwo_buf, const Sint64 sz) {
    FILE *fp;
    char buf[FILENAME_MAX + 1], *bn;

    bn = get_basename(fname, buf);
    if (NULL != (fp = fopen(bn, "w"))) {
        fwrite(rwo_buf, (size_t)sz, 1, fp);
        fclose(fp);
        fprintf(stderr, "wrote %s\n", bn);
    } else {
        fprintf(stderr, "can't write %s\n", bn);
    }
}

int expected_ok(const char *fname) {
    char buf[FILENAME_MAX + 1], *bn;

    bn = get_basename(fname, buf);
    if ( (bn[0] == 'x') || (NULL != strstr(bn, "16.png")) )
        return 0;
    return 1;
}
/* consumes both surfaces */
int compare_surfaces(const char *fname, SDL_Surface *si_surf, SDL_Surface *spl_surf , int loud) {
    int rv = 0, pixel_format_mismatch = 0;
    SDL_Surface *si_converted = NULL;
    if (!spl_surf || !si_surf) {
        if (loud) {
            fprintf(stderr, "compare_surfaces(%s): spl_surf is %p si_surf is %p", fname, (void *)spl_surf, (void *)si_surf);
            rv += 1;
            goto done;
        }
    }
    if (spl_surf->format->format != si_surf->format->format) {
        if ((spl_surf->format->format == SDL_PIXELFORMAT_RGBA8888) &&
            (si_surf->format->format == SDL_PIXELFORMAT_ABGR8888)) {
            si_converted = SDL_ConvertSurface(si_surf, spl_surf->format, 0);
            if (NULL == si_converted) {
                fprintf(stderr, "compare_surfaces(%s): pixel format convert failed: %s\n", fname, SDL_GetError());
                rv += 1;
                goto done;
            } else {
                si_surf = si_converted;
            }
        } else {
            fprintf(stderr, "compare_surfaces(%s): pixel format mismatch spl %s si %s\n", fname,
                SDL_GetPixelFormatName(spl_surf->format->format),
                SDL_GetPixelFormatName(si_surf->format->format));
            rv += 1;
            pixel_format_mismatch = 1; /* delay error reporting a bit */
        }
    }
    if (spl_surf->w != si_surf->w) {
        fprintf(stderr, "compare_surfaces(%s): w mismatch spl %d si %d\n", fname, spl_surf->w, si_surf->w);
        rv += 1;
    }
    if (spl_surf->h != si_surf->h) {
        fprintf(stderr, "compare_surfaces(%s):  h mismatch spl %d si %d\n", fname, spl_surf->h, si_surf->h);
        rv += 1;
    }
    if (spl_surf->pitch != si_surf->pitch) {
        fprintf(stderr, "compare_surfaces(%s): pitch mismatch spl %d si %d\n", fname, spl_surf->pitch, si_surf->pitch);
        rv += 1;
    }
    if (pixel_format_mismatch) {
        rv += 1;
        goto done;
    }
    if (spl_surf->format->palette) {
        if (spl_surf->format->palette->ncolors != si_surf->format->palette->ncolors) {
            fprintf(stderr, "compare_surfaces(%s): palette ncolors mismatch spl %d si %d\n", fname,
                    spl_surf->format->palette->ncolors, si_surf->format->palette->ncolors);
            rv += 1;
        }
        {
            int ci, cm = 0;
            for (ci = 0; ci < si_surf->format->palette->ncolors; ci++) {
                if (ci == spl_surf->format->palette->ncolors)
                    break;

                if ((spl_surf->format->palette->colors[ci].r != si_surf->format->palette->colors[ci].r) ||
                    (spl_surf->format->palette->colors[ci].g != si_surf->format->palette->colors[ci].g) ||
                    (spl_surf->format->palette->colors[ci].b != si_surf->format->palette->colors[ci].b))
                        cm += 1;
            }
            if (cm > 0) {
                fprintf(stderr, "compare_surfaces(%s): palette mismatch %d colors\n", fname, cm);
                rv += cm;
            }
        }
    }
    if (si_surf && spl_surf) {
        Uint32 spl_key = 0, si_key = 42;
        int si_rv, spl_rv;
        spl_rv = SDL_GetColorKey(spl_surf, &spl_key);
        si_rv = SDL_GetColorKey(si_surf, &si_key);
        if (si_rv != spl_rv) {
            fprintf(stderr, "compare_surfaces(%s): SDL_GetColorKey() rv mismatch spl %d si %d \n", fname, spl_rv, si_rv);
            rv += 1;
        } else {
            if ((si_rv == 0) && (spl_key != si_key)) {
                fprintf(stderr, "compare_surfaces(%s): colorkey mismatch spl %x si %x\n", fname, spl_key, si_key);
                rv += 1;
            }
        }
        int i, fails = 0, j;
        Uint8 *spl_row = spl_surf->pixels;
        Uint8 *si_row = si_surf->pixels;
        for (i = 0; i < spl_surf->h ; i++) {
            if ( 0 != memcmp(spl_row + i*spl_surf->pitch, si_row +  i*si_surf->pitch, si_surf->format->BytesPerPixel * si_surf->w)) {
                fails += 1;
                fprintf(stderr, "pnglite %03d: ", i);
                for(j=0; j<spl_surf->pitch; j++)
                    fprintf(stderr, "%02x ", (unsigned) (*(spl_row + i*spl_surf->pitch + j)));
                fprintf(stderr, "\n");
                fprintf(stderr, "image   %03d: ", i);
                for(j=0; j<si_surf->pitch; j++)
                    fprintf(stderr, "%02x ", (unsigned) (*(si_row + i*si_surf->pitch + j)));
                fprintf(stderr, "\n");
            }
        }
        if (fails > 0) {
            for (i = 0; i < spl_surf->h ; i++) {
                fprintf(stderr, "pnglite %03d: ", i);
                for(j=0; j<spl_surf->pitch; j++) {
                    fprintf(stderr, "%02x ", (unsigned) (*(spl_row + i*spl_surf->pitch + j)));
                }
                fprintf(stderr, "\n");
            }
            for (i = 0; i < spl_surf->h ; i++) {
                fprintf(stderr, "image   %03d: ", i);
                for(j=0; j<si_surf->pitch; j++) {
                    fprintf(stderr, "%02x ", (unsigned) (*(si_row + i*si_surf->pitch + j)));
                }
                fprintf(stderr, "\n");
            }
            fprintf(stderr, "compare_surfaces(%s): pixel data mismatch (%d rows)\n", fname, fails);
            rv += fails;
        }
    }
done:
    if (si_converted) { SDL_FreeSurface(si_converted); }
    return rv;
}

int test_load(const char *fname, int expected_ok, int loud, int no_si) {
    SDL_Surface *si_surf = NULL, *spl_surf = NULL;
    int rv = 0;

    if (loud) {
        fprintf(stderr, "SDL_LoadPNG():\n");
    }
    spl_surf = SDL_LoadPNG(fname);
    if (NULL == spl_surf) {
        if (loud || expected_ok) {
            fprintf(stderr, "%s\n", SDL_GetError());
        }
        rv = 1;
    } else if (loud) {
        fprintf(stderr, "    size %dx%d pitch=%d pf=%s\n",
            spl_surf->w, spl_surf->h, spl_surf->pitch, SDL_GetPixelFormatName(spl_surf->format->format));
    }
    if (!no_si) {
        if (loud) {
            fprintf(stderr, "IMG_Load():\n");
        }
        si_surf = IMG_Load(fname);
        if (NULL == si_surf) {
            fprintf(stderr, "    IMG_Load(%s): %s\n", fname , SDL_GetError());
            if (expected_ok) { rv += 1; }
        }
        if (si_surf && loud) {
            fprintf(stderr, "    size %dx%d pitch=%d pf=%s\n",
                si_surf->w, si_surf->h, si_surf->pitch, SDL_GetPixelFormatName(si_surf->format->format));
        }
        if (spl_surf && si_surf) {
            rv += compare_surfaces(fname, si_surf, spl_surf ,loud);
        }
    }
    if (si_surf) { SDL_FreeSurface(si_surf); }
    if (spl_surf) { SDL_FreeSurface(spl_surf); }
    return expected_ok ? rv : 0;
}

int test_save(const char *fname, int expected_ok, int loud) {
    SDL_Surface *si_surf = NULL, *spl_surf = NULL;
    SDL_RWops *rwo = NULL;
    void *rwo_buf = NULL;
    const int rwo_buf_sz = 1048576;
    int rv = 0;
    Sint64 sz;

    spl_surf = SDL_LoadPNG(fname);
    if (NULL == spl_surf) {
        fprintf(stderr, "SDL_LoadPNG(%s): %s\n", fname , SDL_GetError());
        rv = 1;
        goto exit;
    }
    if (loud) {
        Uint32 colorkey = 0;
        SDL_GetColorKey(spl_surf, &colorkey);
        fprintf(stderr, "%s/spl: w %d h %d pitch %d pf %s ck %x\n", fname,
            spl_surf->w, spl_surf->h, spl_surf->pitch,
            SDL_GetPixelFormatName(spl_surf->format->format), colorkey);
    }
    rwo_buf = SDL_malloc(rwo_buf_sz);
    if (NULL == rwo_buf) {
        fprintf(stderr, "SDL_malloc(): %s\n", SDL_GetError());
        goto exit;
    }
    rwo = SDL_RWFromMem(rwo_buf, rwo_buf_sz);
    if (NULL == rwo) {
        fprintf(stderr, "SDL_RWFromMem(): %s\n", SDL_GetError());
        rv = 1;
        goto exit;
    }

    if (-1 == SDL_SavePNG_RW(spl_surf, rwo, 0)) {
        fprintf(stderr, "%s: SDL_SavePNG_RW(): %s\n", fname, SDL_GetError());
        rv = 1;
        goto exit;
    }
    sz = SDL_RWtell(rwo);
    SDL_RWseek(rwo, 0, RW_SEEK_SET);

    si_surf = IMG_LoadPNG_RW(rwo);
    if (NULL == si_surf) {
        fprintf(stderr, "IMG_Load(%s): %s\n", fname , SDL_GetError());
        dump_buf(fname, rwo_buf, sz);
        rv = 1;
        goto exit;
    }

    rv += compare_surfaces(fname, si_surf, spl_surf, loud);
    if (rv)
        dump_buf(fname, rwo_buf, sz);

  exit:
    if (si_surf) { SDL_FreeSurface(si_surf); }
    if (spl_surf) { SDL_FreeSurface(spl_surf); }
    if (rwo) { SDL_FreeRW(rwo); }
    if (rwo_buf) { SDL_free(rwo_buf); }
    return expected_ok ? rv : 0;
}

int main(int argc, char *argv[]) {
    int i, fails = 0, loud = 0, failcount = 0, no_si = 0;
    char *fname;

    loud = getenv("LOUD") != NULL;
    no_si = getenv("NOSI") != NULL;

    fprintf(stderr, "=== TEST LOAD =====================================\n");
    for (i = 1; i < argc; i++) {
        fname = argv[i];
        if (loud) { fprintf(stderr, "%s : \n", fname); }
        fails = test_load(fname, expected_ok(fname), loud, no_si);
        failcount += fails ? 1 : 0;
        if (loud) {
            if (fails == 0) {
                fprintf(stderr, "%s: OK\n", fname);
            } else {
                fprintf(stderr, "%s: FAIL\n", fname);
            }
        }
    }
    fprintf(stderr, "=== TEST SAVE =====================================\n");
    for (i = 1; i < argc; i++) {
        fname = argv[i];
        if (loud) { fprintf(stderr, "%s : \n", fname); }
        fails = test_save(fname, expected_ok(fname), loud);
        failcount += fails ? 1 : 0;
        if (loud) {
            if (fails == 0) {
                fprintf(stderr, "%s: OK\n", fname);
            } else {
                fprintf(stderr, "%s: FAIL\n", fname);
            }
        }
    }
    fprintf(stderr, "=== TEST FAILURES: %d =====================================\n", failcount);
    IMG_Quit();
#if defined(_WIN32)
    SDL_Delay(100500);
#endif
    SDL_Quit();
    return failcount;
}

