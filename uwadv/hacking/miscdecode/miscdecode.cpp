// miscdecode.cpp
//

#pragma warning(disable:4786)

#include "../hacking.h"
#include <stdio.h>
#include <string.h>
#include <io.h>

#include "common.hpp"
#include "settings.hpp"
#include "gamestrings.hpp"

#undef SDL_main

int main(int argc, char* argv[])
{
   // decoding *.byt, try out all 8 palettes
   for(int palnr=0; palnr<8; palnr++)
   {
      // get 256 colors palette
      char palette[256*3];
      {
         FILE *pal = fopen(UWPATH"data\\pals.dat","rb");

         fseek(pal,256*3*palnr,SEEK_SET);

         fread(palette,1,256*3,pal);

         for(int j=0; j<256*3; j++) palette[j]<<=2;

         for(int k=0; k<256; k++)
         {
            char h = palette[k*3+0];
            palette[k*3+0] = palette[k*3+2];
            palette[k*3+2] = h;
         }

         fclose(pal);
      }

      {
         _finddata_t find;
         long hnd = _findfirst(UWPATH"data\\*.byt",&find);

         if (hnd==-1)
         {
            printf("no *.byt files found!\n");
            return 0;
         }

         do
         {
            char basename[64];

            // get basename
            strcpy(basename,find.name);

            char *pos = strrchr(basename,'.');
            if (pos != NULL)
               *pos = 0;

            char fname[64];

            // construct name
            sprintf(fname,UWPATH"data\\%s.byt",basename);

            FILE *fd = fopen(fname,"rb");

            if (fd==NULL)
            {
               printf("could not open file %s\n",fname);
               break;
            }


            // now open and write a tga file

            sprintf(fname,"%s-pal%02u.tga",basename,palnr);

            FILE *tga = fopen(fname,"wb");

            tga_writeheader(tga,320,200,1,1);

            // write palette
            fwrite(palette,1,256*3,tga);

            char buffer[1024];
            int datalen = 64000;

            while (datalen>0)
            {
               int size = datalen > 1024 ? 1024 : datalen;
               fread(buffer,1,size,fd);
               fwrite(buffer,1,size,tga);
               datalen-=size;
            }

            fclose(fd);
            fclose(tga);

         } while(0==_findnext(hnd,&find));


         _findclose(hnd);

      }

   } // end pal for

   // decoding cmb.dat
   {
      ua_gamestrings gs;
      gs.load(UWPATH"data\\strings.pak");

      FILE *fd = fopen(UWPATH"data\\cmb.dat","rb");

      FILE *out = fopen("uw-combinations.txt","w");

      while(1)
      {
         unsigned short item1,item2,resitem;

         fread(&item1,2,1,fd);
         fread(&item2,2,1,fd);
         fread(&resitem,2,1,fd);

         if (item1==0 && item2==0 && resitem==0)
            break;

         fprintf(out,"%04x + %04x = %04x:  \"%s\"%s + \"%s\"%s = \"%s\"%s\n",
            item1,item2,resitem,
            gs.get_string(3,item1&0x1ff).c_str(), (item1&0x8000)==0 ? "" : "(*)",
            gs.get_string(3,item2&0x1ff).c_str(), (item2&0x8000)==0 ? "" : "(*)",
            gs.get_string(3,resitem&0x1ff).c_str(), (resitem&0x8000)==0 ? "" : "(*)"
         );
      }

      fclose(fd);
      fclose(out);
   }

   return 0;
}
