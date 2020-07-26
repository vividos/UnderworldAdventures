libpnglite with indexed color types support
*******************************************

http://www.w3.org/TR/PNG/

Each push is tested against the PngSuite and Google Image Test Suite 1.01.

Two images of the latter do cause libpng 1.6.34 to enter endless loop,
while this library correctly reports them as broken.

Reading:
========


PNG ancillary chunks
--------------------

Of ancillary chunks only tRNS is parsed. Others are silently ignored.
No access to them is provided. Spec violations in those chunks contents
or in their order are also ignored.


Image and chunk sizes
---------------------

As per spec, maximum size of chunks is limited to (1<<31)-1 bytes.

Maximum size of resulting image data not counting palette is also
limited to the same value.


PNG per-channel depth
----------------------

Everything that's less than 8 bits is expanded to 8 bits.

16 bit per channel images are not supported.

Reason for not supporting: SDL does not support 16bit depth anyway, and
the possible colorkey becomes ambigous.


PNG color types and transparency:
---------------------------------

- PNG_INDEXED, no transparency:
    - png_get_data() returns I bytestream; required buffer size is width*height
    - png_t::palette contains png_t::palette_size RGB entries, unused entries are set to #FFF
    - png_t::transparency_present is 0

- PNG_INDEXED, with transparency:
    - png_get_data() returns I bytestream; required buffer size is width*height
    - png_t::palette contains png_t::palette_size RGB entries, unused entries are set to #FFF.
    - png_t::palette[768...1024] contains png_t::palette_size alpha values, unused entries are set to FF.
    - png_t::transparency_present is 1

- PNG_GREYSCALE, no transparency:
    - png_get_data() returns RGB bytestream; required buffer size is width*height*3
    - png_t::transparency_present is 0

- PNG_GREYSCALE, with transparency:
    - png_get_data() returns RGBA bytestream; required buffer size is width*height*4
    - png_t::colorkey[0..1] contains the transparent sample value. Since 16 bit depth
      is not supported, only png_t::colorkey[1] value should be used.
    - png_t::transparency_present is 1

- PNG_TRUECOLOR, no transparency:
    - png_get_data() returns RGB bytestream; required buffer size is width*height*3
    - png_t::transparency_present is 0

- PNG_TRUECOLOR, with transparency:
    - png_get_data() returns RGBA bytestream; required buffer size is width*height*4
    - png_t::colorkey[0..5] contains the transparent RGB sample values. Since 16 bit depth
      is not supported, only png_t::colorkey[1,3,5] values should be used.
    - png_t::transparency_present is 1

- PNG_GREYSCALE_ALPHA:
    - png_get_data() returns RGBA bytestream; required buffer size is width*height*4
    - png_t::transparency_present is not defined

- PNG_TRUECOLOR_ALPHA:
    - png_get_data() returns RGBA bytestream; required buffer size is width*height*4
    - png_t::transparency_present is not defined


Writing:
========

When png_t::color_type is set to:

- PNG_INDEXED:
    - supplied buffer must contain widht*height bytes of palette indices.
    - png_t::transparency_present may be 0 or 1.
    - png_t::palette must be initialized to RGBX or RGBA palette and png_t::palette_size
      must be set to number of colors in the palette.

- PNG_TRUECOLOR:
    - supplied buffer must contain widht*height*3 bytes of RGB samples.
    - png_t::transparency_present may be 0 or 1.
    - png_t::colorkey may be initialized to an RGB sample values.

- PNG_TRUECOLOR_ALPHA:
    - supplied buffer must contain widht*height*4 bytes of RGBA samples.

Other color types are not supported.

Filtering is not done - patches are welcome.

Compression is to be assumed suboptimal.


Thread safety:
==============

This code is thread-safe as long as png_t objects are kept thread-local and
the supplied callbacks are thread-safe themselves.


SDL_Surface wrapper for the above
*********************************

Depends on SDL2.

All above caveats apply.


SDL_LoadPNG() / SDL_LoadPNG_RW():
=================================

- Attempts to load a png from given filename / RWops object.
- Indexed-color images without transparency are returned as paletted surfaces.
- Indexed-color images with transparency are always returned as paletted surfaces.
  First fully-transparent color's index is set as colorkey.
- Truecolor+alpha and grayscale+alpha are returned as RGBA32.
- Truecolor (no alpha) are returned as RGB24 (transparency results in colorkey).
- Grayscale images are returned as indexed color (transparency results in colorkey).


SDL_SavePNG() / SDL_SavePNG_RW():
=================================

- Attemps to save given surface as png image to given filename / RWops object.
- Paletted surfaces with or without colorkey are saved as indexed color.
- RGB surfaces are saved as 8bpc RGB preserving colorkey.
- All other surfaces are converted to and saved as 8bpc RGBA ones.


Notable differences from IMG_LoadPNG_RW():
==========================================

- 16 bit per channel images are not accepted.


Notable differences from IMG_SavePNG_RW():
==========================================

- Palettes and colorkeys are preserved as much as possible within the format
  (IMG_SavePNG_RW() doesn't attempt this at all)


Thread safety:
==============

The wrapper is thread-safe as long as the supplied RWops object is.


Test suite (test-suite.c):
==========================

Test strategy for loading:
--------------------------

- For each image in the test suite, load it both with SDL_LoadPNG() and IMG_Load().
  Pixelformats and image data must be mostly identical.

Test strategy for saving:
-------------------------

- For each image in the test suite, load it, then save to a memory buffer,
  then load from the buffer with IMG_LoadPNG_RW(). Compare pixelformats and pixel data.

Test image sets:
----------------

- get PngSuite from http://www.schaik.com/pngsuite/
- google up the Google Image Test Suite PNG part.
- submit the rest to the test suite:  ``./test-suite /path/to/pngsuite/*.png``
- remove or rename images ``m1-71915ab0b1cc7350091ef7073a312d16.png`` and ``m1-7dc9db3d3e510156c619273f8f913cbe.png``
  to something not ending in .png or this won't end well.
- submit the rest to the test suite:  ``./test-suite /path/to/googlesuite/*.png``
- valgrind it, read the code, etc.

Known issues:
-------------

- SDL2 can have colorkeyed RGBA surfaces. PNG does not support colorkeys on RGBA data, thus
  the colorkey is lost on save. Alternative would be to lose alpha channel on matching pixels.
- ``tbbn0g04.png: pixel format mismatch spl SDL_PIXELFORMAT_INDEX8 si SDL_PIXELFORMAT_RGB565``
  reason is SDL_image + libpng 1.6 converts this 4-bit grayscale image to RGB565
  while SDL_pnglite converts it to a paletted surface.
- ``tm3n3p02.png: pixel format mismatch spl SDL_PIXELFORMAT_INDEX8 si SDL_PIXELFORMAT_ABGR8888``
  SDL_image converts this 2-bit paletted image to a completely bogus format and I don't know why.
