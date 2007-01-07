/* clears hidden flag from all objects in given lev.ark */

#include <SDL.h>

const char* filename = "e:\\uw1\\save2\\lev.ark";

#undef main

int main(int argc, char** argv)
{
   FILE* fd = fopen(filename,"r+b");

   Uint16 nblocks;
   fread(&nblocks,2,1,fd);

   for(unsigned int i=0; i<9; i++)
   {
      fseek(fd,i*4+2,SEEK_SET);

      Uint32 offset;
      fread(&offset,4,1,fd);
      fseek(fd,offset+0x4000,SEEK_SET);

      for(unsigned int n=0; n<0x0400; n++)
      {
         // read first obj word
         Uint16 objword;
         fread(&objword,2,1,fd);

         // modify word: clear bit
         objword &= ~(1L<<14);

         // write back word
         fseek(fd,-2,SEEK_CUR);
         fwrite(&objword,2,1,fd);

         // jump over other words
         fseek(fd,6,SEEK_CUR);

         // jump over npc data
         if (n<0x0100)
            fseek(fd,19,SEEK_CUR);
      }
   }

   return 0;
}
