// trdecode.cpp
//

#include "../hacking.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <io.h>

int main(int argc, char* argv[])
{
   time_t starttime = time(NULL);

   char fname[256];


   // get 256 colors palette
   char palette[256*3];
   {
      FILE *pal = fopen(UWPATH"data\\pals.dat","rb");

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
   long hnd = _findfirst(UWPATH"data\\*.tr",&find);

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
      sprintf(fname,UWPATH"data\\%s.tr",basename);

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
