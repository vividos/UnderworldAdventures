#ifndef _ZZIP_STDINT_H
#define _ZZIP_STDINT_H 1
/* this file ensures that we have some kind of typedef declarations for
   unsigned C9X typedefs. The ISO C 9X: 7.18 Integer types file is stdint.h
 */

/*

  updated for Ultima Adventures: use SDL_types.h instead

*/

#include <zzip-conf.h> 
#include "SDL_types.h"

typedef Uint8 uint8_t;
typedef Sint8 int8_t;
typedef Uint16 uint16_t;
typedef Sint16 int16_t;
typedef Uint32 uint32_t;
typedef Sint32 int32_t;

#endif /*_ZZIP_STDINT_H*/

