// critdec.cpp
//

#include <stdio.h>
#include <string>


const char *cr_fmt = "d:\\store\\uw_demo\\crit\\cr%02opage.n%02o";

int main(int argc, char* argv[])
{
   const char *assocfilename = "d:\\store\\uw_demo\\crit\\dassoc.anm";

   for(int crit=0; crit<32; crit++)
   {
      // retrieve critter name
      char critname[8]; critname[0]=0;
      unsigned char anim=0;
      unsigned char variant=0;

      FILE *assoc = fopen(assocfilename,"rb");
      fseek(assoc,crit*8,SEEK_SET);
      fread(critname,1,8,assoc);

      fseek(assoc,256+crit*2,SEEK_SET);
      anim = fgetc(assoc);
      variant = fgetc(assoc);
      fclose(assoc);

      {
         char buffer[256];
         sprintf(buffer,cr_fmt,crit,0);

         FILE *pfile = fopen(buffer,"rb");
         if (pfile==NULL)
            continue;

         fclose(pfile);
      }

      printf("critter #%u: \"%.8s\", anim=%02x, variant=%02x\n",crit,critname,anim,variant);

      // read in all pages
      for(int page=0;;page++)
      {
         char buffer[256];
         sprintf(buffer,cr_fmt,crit,page);
         FILE *pfile = fopen(buffer,"rb");
         if (pfile==NULL)
            break;

         // read in segment directory
         int segbase = fgetc(pfile);
         int nsegs = fgetc(pfile);

         if (nsegs==0)
         {
            printf("no segments in file\n");
            continue;
         }

         unsigned char *segdiroffsets = new unsigned char[nsegs];
         fread(segdiroffsets,1,nsegs,pfile);

         printf("cr%02opage.n%02o:\n",crit,page);

         int nanimsegs = fgetc(pfile);

         for(int i=0; i<nanimsegs; i++)
         {
            printf("segment #%u (%02x): ",i,segdiroffsets[i]);

            for(int n=0; n<8; n++)
               printf("%02x ",fgetc(pfile));
            printf("\n");
         }

         int nauxpals = fgetc(pfile);
         if (nauxpals==-1) continue;

         unsigned char *auxpals = new unsigned char[32*nauxpals];
         fread(auxpals,32,nauxpals,pfile);

         int noffsets = fgetc(pfile);
         int unk1 = fgetc(pfile);

         unsigned short *alloffsets = new unsigned short[noffsets];
         fread(alloffsets,2,noffsets,pfile);

         int pos = ftell(pfile);

         printf("content: %u segs, %u auxpals, %u frames\n",
            nsegs,nauxpals,noffsets);

         for(int frame=0; frame<noffsets; frame++)
         {
            fseek(pfile,alloffsets[frame],SEEK_SET);
            int width,height,hotx,hoty,type;

            width = fgetc(pfile);
            height = fgetc(pfile);
            hotx = fgetc(pfile);
            hoty = fgetc(pfile);
            type = fgetc(pfile);

            printf("frame #%d, %ux%u, hot:(%u,%u), type=%u\n",
               frame,width,height,hotx,hoty,type);

            // seek to bitmap data
            fseek(pfile,alloffsets[frame],SEEK_SET);

            // TODO decode bitmap
         }

         delete[] alloffsets;
         delete[] auxpals;
         delete[] segdiroffsets;
         fclose(pfile);
      }

      printf("\n");
   }

   return 0;
}
