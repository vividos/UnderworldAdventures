// trdecode.cpp
//

#include <stdio.h>
#include <string.h>
#include <time.h>
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
   time_t starttime = time(NULL);

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
   long hnd = _findfirst("..\\..\\uw1\\data\\*.tr",&find);

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
      sprintf(fname,"..\\..\\uw1\\data\\%s.tr",basename);

      FILE *fd = fopen(fname,"rb");
      if (fd==NULL)
      {
         printf("could not open file\n");
         return 0;
      }

      fseek(fd,0,SEEK_END);
      unsigned long flen = ftell(fd);
      fseek(fd,0,SEEK_SET);

      int val = fgetc(fd); // this always seems to be 2
      int xyres = fgetc(fd); // x and y resolution (square textures)

      unsigned short entries;
      fread(&entries,1,2,fd);

      printf("file %s: %u entries, %ux%u\n",basename,entries,xyres,xyres);

      unsigned int *offsets = new unsigned int[entries];
      fread(offsets,sizeof(unsigned int),entries,fd);

      for(int i=0; i<entries; i++)
      {
         fseek(fd,offsets[i],SEEK_SET);

         char tganame[256];

         // open new tga file
         sprintf(tganame,"%s-tex%02u.tga",basename,i);
         FILE *tga = fopen(tganame,"wb");

         tga_writeheader(tga,xyres,xyres,1,1);

         fwrite(palette,1,256*3,tga);

         // copy remaining bytes
         int datalen = xyres*xyres;
         char buffer[1024];

         while (datalen>0)
         {
            int size = datalen > 1024 ? 1024 : datalen;
            fread(buffer,1,size,fd);
            fwrite(buffer,1,size,tga);
            datalen-=size;
         }

         fclose(tga);
      }

      delete[] offsets;

      fclose(fd);

   } while(0==_findnext(hnd,&find));

   _findclose(hnd);

   printf("needed %u seconds to extract\n",time(NULL)-starttime);

   return 0;
}
