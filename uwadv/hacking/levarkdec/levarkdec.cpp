// levarkdec.cpp
//

#include "../hacking.h"
#include <stdio.h>

const char *filename = UWPATH"Save3\\lev.ark";

int main(int argc, char* argv[])
{
   FILE *fd = fopen(filename,"rb");

   if (fd==NULL)
   {
      printf("could not open lev.ark\n");
      return 0;
   }

   fseek(fd,0,SEEK_END);
   unsigned int flen = ftell(fd);
   fseek(fd,0,SEEK_SET);

   FILE *out = fopen("uw-levelobjs.txt","w");

   // read in toc

   unsigned short entries;
   fread(&entries,sizeof(unsigned short),1,fd);

   printf("lev.ark: %u entries\n",entries);

   unsigned int *offsets = new unsigned int[entries];

   // read all offset
   fread(offsets,sizeof(unsigned int),entries,fd);

   int lastpos=-1;
   for(int i=0; i<entries; i++)
   {
      if (offsets[i]!=0)
      {
         if (lastpos>0)
         {
            int size = offsets[i]-lastpos;
            if (size<0) size = -size;
            printf(", size=%08x\n",size);
         }

         printf("entry %02u: offset=%08x",i,offsets[i]);
         lastpos = offsets[i];
      }
   }
   printf(", size=%08x\n",flen-lastpos);

   for(int j=0; j<9; j++)
   {
      fseek(fd,offsets[j],SEEK_SET);

      char fname[256];

      unsigned int buffer[64*64];
      fread(buffer,4,64*64,fd);

      // now open and write tga files

      // write tile type map
      {
         sprintf(fname,"level%03u-type.tga",j);
         FILE *tga = fopen(fname,"wb");
         tga_writeheader(tga,64,64,2,0,true);

         int mapping[16] =
         {
            0, 255, 192, 192, 192, 192, 64, 64, 64, 64, 128, 128, 128, 128, 128, 128
         };

         for(int i=0; i<64*64; i++)
         {
            int c = mapping[buffer[i] & 0x0f];
            fputc(c,tga); fputc(c,tga); fputc(c,tga);
            fputc(0,tga);
         }
         fclose(tga);
      }

      // write floor height map
      {
         sprintf(fname,"level%03u-floor.tga",j);
         FILE *tga = fopen(fname,"wb");
         tga_writeheader(tga,64,64,2,0,true);

         for(int i=0; i<64*64; i++)
         {
            int h = (buffer[i] & 0xF0);
            int t = (buffer[i] & 0x003F0000) >> 14; // 6 bit wide
            int t2 = (buffer[i] & 0x00003C00) >> 10; // 4 bits

            fputc(h,tga); //fputc(t,tga); fputc(t2,tga);
            fputc(h,tga); fputc(h,tga);
            fputc(0,tga);
         }
         fclose(tga);
      }

      // write index map
      {
         sprintf(fname,"level%03u-index.tga",j);
         FILE *tga = fopen(fname,"wb");
         tga_writeheader(tga,64,64,2,0,true);

         for(int i=0; i<64*64; i++)
         {
            int idx = (buffer[i] & 0xFFC00000) >> 22;

            fputc(idx&0xff,tga); fputc(idx>>8,tga); fputc(idx&0xff,tga);
            fputc(0,tga);
         }
         fclose(tga);
      }

      // write unknowns map
      {
         sprintf(fname,"level%03u-unk.tga",j);
         FILE *tga = fopen(fname,"wb");
         tga_writeheader(tga,64,64,2,0,true);

         for(int i=0; i<64*64; i++)
         {
            int unk1 = (buffer[i] & 0x00000300) >> 2;
            int unk2 = (buffer[i] & 0x00004000) >> 7;

            fputc(unk1,tga); fputc(unk2,tga); fputc(unk2,tga);
            fputc(0,tga);
         }
         fclose(tga);
      }

      // more tga writing here


      // level object dumping
      fseek(fd,offsets[j+9],SEEK_SET);
      fprintf(out,"objects for level %02u, at offset %08x:\n\n",j,offsets[j+9]);

      for(int n=0; n<48; n++)
      {
         unsigned int objval[2];
         fread(objval,4,2,fd);

         fprintf(out,"object %02x: ",n);

         fprintf(out,"id=%04x ",objval[0] & 0x000001ff);
         fprintf(out,"unk1=%02x ",(objval[0] & 0x0000ff00)>>9 );
         fprintf(out,"zpos=%02x ",(objval[0] & 0x007f0000)>>16 );
         fprintf(out,"dir=%u ",(objval[0] &    0x03800000)>>23 );
         fprintf(out,"xpos=%02x ",(objval[0] & (7<<26))>>26 );
         fprintf(out,"ypos=%02x ",(objval[0] &  (7<<29))>>29 );

         fprintf(out,"quality=%02x ",objval[1] & 0x0000003f);
         fprintf(out,"link1=%04x ",(objval[1] & (0x3ff<<6))>>6);
         fprintf(out,"unk2=%02x ",(objval[1] & (0x3f<<16))>>16);
         fprintf(out,"quan/link2=%04x\n",(objval[1] & (0x3ff<<22))>>22);
      }
      fprintf(out,"\n");
   }

   delete[] offsets;

   fclose(fd);
   fclose(out);

   return 0;
}
