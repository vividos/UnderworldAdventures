// miscdecode.cpp
//

#include "../hacking.h"
#include <stdio.h>
#include <string.h>
#include <io.h>

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


   return 0;
}
