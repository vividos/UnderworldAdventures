// miscdecode.cpp
//

#include <stdio.h>
#include <string.h>
#include <io.h>


void tga_writeheader(FILE *fd, int width, int height, int type=2, int colmap=0)
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
      0, 0, width, height, colmap==1?8:32, 0x20
   };
#pragma pack(pop)

   fwrite(&tgaheader,1,18,fd);
}


int main(int argc, char* argv[])
{
   // decoding *.byt, try out all 8 palettes
   for(int palnr=0; palnr<8; palnr++)
   {
      // get 256 colors palette
      char palette[256*3];
      {
         FILE *pal = fopen("..\\..\\uw1\\data\\pals.dat","rb");

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
         long hnd = _findfirst("..\\..\\uw1\\data\\*.byt",&find);

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
            sprintf(fname,"..\\..\\uw1\\data\\%s.byt",basename);

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
