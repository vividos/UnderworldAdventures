// fontdecode.cpp
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
   char fname[256];


   // get 256 colors palette
   char palette[256*3];
   {
      FILE *pal = fopen("..\\..\\uw1\\data\\pals.dat","rb");

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


   _finddata_t find;
   long hnd = _findfirst("..\\..\\uw1\\data\\font*.sys",&find);

   if (hnd==-1)
   {
      printf("no files found!\n");
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


      // construct name
      sprintf(fname,"..\\..\\uw1\\data\\%s.sys",basename);

      FILE *fd = fopen(fname,"rb");

      if (fd==NULL)
      {
         printf("could not open file\n");
         return 0;
      }

      fseek(fd,0,SEEK_END);
      unsigned long flen = ftell(fd);
      fseek(fd,0,SEEK_SET);

      unsigned short dummy,charsize,spacewidth,charheight,rowwidth,maxwidth;
      fread(&dummy,1,2,fd);
      fread(&charsize,1,2,fd);
      fread(&spacewidth,1,2,fd);
      fread(&charheight,1,2,fd);
      fread(&rowwidth,1,2,fd);
      fread(&maxwidth,1,2,fd);

      int entries = (flen-12) / (charsize+1);

      printf("file %s.sys: %u characters\n",basename,entries);

      printf(" height: %u, maxwidth: %u, spacewidth: %u\n",
         charheight,maxwidth,spacewidth);

      printf(" rowwidth: %u, charsize: %u bytes\n",rowwidth,charsize);

      if ((flen-12) % (charsize+1) != 0)
         printf("odd number of chars!\n");

      // load bitmap patterns

      unsigned int *allcharsizes = new unsigned int[entries];
      unsigned char *allpatterns = new unsigned char[entries*charsize];

      for(int i=0; i<entries; i++)
      {
         // read bit pattern
         fread(&allpatterns[i*charsize],1,charsize,fd);

         // read length of character
         unsigned char onecharsize;
         fread(&onecharsize,1,1,fd);
         allcharsizes[i] = onecharsize;
      }

      unsigned char *allindices = new unsigned char[entries*charheight*maxwidth];

      // convert bit pattern to byte palette indices
      for(int j=0; j<entries; j++)
      {
         unsigned char *curbits = &allpatterns[j*charsize];
         unsigned char *curindices = &allindices[j*charheight*maxwidth];
         int raw = *curbits;
         int bits = 8;

         for(unsigned int b=0; b<charheight; b++)
         {
            for(unsigned int k=0; k<maxwidth; k++)
            {
               if (bits==0)
               {
                  curbits++;
                  bits=8;
                  raw = *curbits;
               }

               if ((raw&0x80)==0)
               {
                  // background
                  if (k>=allcharsizes[j])
                     *curindices = 253; // for 'outside' bits, draw in other color
                  else
                     *curindices = 11;
               }
               else
               {
                  *curindices = 0;
               }
               curindices++;
               raw <<= 1;
               bits--;
            }
            bits=0; // reset bit reservoir every line
         }
      }

      {
         // now open and write a tga file

         sprintf(fname,"%s.tga",basename);

         FILE *tga = fopen(fname,"wb");

         tga_writeheader(tga,entries*maxwidth,charheight,1,1);

         // write palette
         fwrite(palette,1,256*3,tga);

         for(int n=0; n<charheight; n++)
         {
            for(int m=0; m<entries; m++)
            {
               for(int z=0; z<maxwidth; z++)
               {
                  int index = m*charheight*maxwidth + z +
                     n*maxwidth;

                  fputc(allindices[index],tga);
               }
            }
         }

         fclose(tga);
      }

      delete[] allindices;
      delete[] allcharsizes;
      delete[] allpatterns;

      fclose(fd);


   } while(0==_findnext(hnd,&find));

   _findclose(hnd);

   return 0;
}
