// hacking.h
// specifies common hacking stuff

#ifndef __uwadv_hacking_h_
#define __uwadv_hacking_h_

#include <stdio.h>

#define UWPATH "d:\\projekte\\uwadv\\uw1\\"

inline void tga_writeheader(FILE *fd, int width, int height, int type=2, int colmap=0, bool bottomup=false)
{
   #pragma pack(push,1)

   // tga header struct
   struct tgaheader
   {
      unsigned char idlength;     // length of id field after header
      unsigned char colormaptype; // 1 if a color map is present
      unsigned char tgatype;      // tga type

      // colormap not used
      unsigned short colormaporigin;
      unsigned short colormaplength;
      unsigned char colormapdepth;

      // x and y origin
      unsigned short xorigin,yorigin;
      // width and height
      unsigned short width,height;

      // bits per pixel, either 16, 24 or 32
      unsigned char bitsperpixel;
      unsigned char imagedescriptor;
   } tgaheader =
   {
      0, colmap, type,   0, (colmap==1?256:0), (colmap==1?24:0),
      0, 0, width, height, colmap==1?8:32, bottomup ? 0x00 : 0x20
   };
#pragma pack(pop)

   fwrite(&tgaheader,1,18,fd);
}

#endif
