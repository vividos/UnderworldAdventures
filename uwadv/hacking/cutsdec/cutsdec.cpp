// cutsdec.cpp
//

#include "../hacking.h"
#include <io.h>
#include <string.h>


void parse_cutscene(FILE *out,FILE *fd,const char *filename)
{
   char lpfstr[4];
   fread(lpfstr,1,4,fd);
   if (strncmp(lpfstr,"LPF ",4)!=0) return;

   unsigned short maxlps, lps;
   fread(&maxlps,1,2,fd);
   fread(&lps,1,2,fd);

   unsigned int nrecords;
   fread(&nrecords,1,4,fd);

   unsigned short maxrecords;
   fread(&maxrecords,1,2,fd);

   fprintf(out,"cutscene: %s\n",filename,lpfstr);
   fprintf(out,"large pages: %u records: %u\n",lps,nrecords);

   unsigned short lpftable_offset;
   fread(&lpftable_offset,1,2,fd);

   char animstr[4];
   fread(animstr,1,4,fd);
   if (strncmp(animstr,"ANIM",4)!=0) return;

   unsigned short width, height;
   fread(&width,1,2,fd);
   fread(&height,1,2,fd);

   int variant = fgetc(fd);
   int version = fgetc(fd);
   int deltaframe = fgetc(fd);
   int deltavalid = fgetc(fd);
   int pixeltype = fgetc(fd);
   int comprtype = fgetc(fd);
   int otherRecsPerFrm = fgetc(fd);
   int bitmaptype = fgetc(fd);
   fseek(fd,32,SEEK_CUR);

   unsigned int nframes;
   fread(&nframes,1,4,fd);

   unsigned short framespersec;
   fread(&framespersec,1,2,fd);

   fseek(fd,58,SEEK_CUR);

   fprintf(out,"size: %u x %u, %u frames, %u fps in %s\n",
      width,height,nframes,framespersec,
      version == 0 ? "18 cycl/s" : version == 1 ? "70 cycl/s" : "?? cycl/s"
      );

   // skip color cycle stuff
   fseek(fd,128,SEEK_CUR);

   fprintf(out,"\n");
}


const char *fname = UWPATH "cuts\\cs???.n??";

int main()
{
   FILE *out = fopen("./uw-cutscenes.txt","wt");

   _finddata_t find;
   long hnd = _findfirst(fname,&find);

   if (hnd==-1)
   {
      printf("no files found!\n");
      return 0;
   }

   do
   {
      char cutsname[256];

      // construct name
      sprintf(cutsname,UWPATH "cuts\\%s",find.name);

      FILE *fd = fopen(cutsname,"rb");

      if (fd==NULL)
      {
         printf("could not open file %s\n",find.name);
         return 0;
      }

      parse_cutscene(out,fd,find.name);

      fclose(fd);

   } while(0==_findnext(hnd,&find));

   _findclose(hnd);

   fclose(out);

	return 0;
}
