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
   // load game strings
   ua_gamestrings gs;
   gs.load(UWPATH"data\\strings.pak");

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
      FILE *fd = fopen(UWPATH"data\\cmb.dat","rb");
      FILE *out = fopen("uw-combinations.txt","w");

      while(1)
      {
         unsigned short item1,item2,resitem;

         fread(&item1,2,1,fd);
         fread(&item2,2,1,fd);
         fread(&resitem,2,1,fd);

         if ((item1==0 && item2==0 && resitem==0) ||
             (item1==0xffff && item2==0xffff && resitem==0xffff))
            break;

         fprintf(out,"%04x + %04x = %04x:  \"%s\"%s + \"%s\"%s = \"%s\"%s\n",
            item1,item2,resitem,
            gs.get_string(4,item1&0x1ff).c_str(), (item1&0x8000)==0 ? "" : "(*)",
            gs.get_string(4,item2&0x1ff).c_str(), (item2&0x8000)==0 ? "" : "(*)",
            gs.get_string(4,resitem&0x1ff).c_str(), (resitem&0x8000)==0 ? "" : "(*)"
         );
      }

      fclose(fd);
      fclose(out);
   }

   // decoding comobj.dat
   {
      FILE *fd = fopen(UWPATH"data\\comobj.dat","rb");
      FILE *out = fopen("uw-common-obj-prop.txt","w");

      fseek(fd,0,SEEK_END);
      long flen = ftell(fd);
      fseek(fd,2,SEEK_SET);

      int entries = (flen-2)/11;
      fprintf(out,"%u objects.\n",entries);

      for(int i=0; i<entries; i++)
      {
         unsigned short mass,value,unknown1,unknown2;
         unsigned char armour,flags,scale;

         int pos = ftell(fd);

         armour = fgetc(fd);
         fread(&mass,2,1,fd);
         flags = fgetc(fd);
         fread(&value,2,1,fd);
         fread(&unknown1,2,1,fd);
         scale = fgetc(fd);
         fread(&unknown2,2,1,fd);

         fprintf(out,"%03x: armour=%02x mass=%03x stuff2=%01x flags=%02x value=%04x "
            "unknown1=%04x scale=%02x unknown2=%04x name=%s\n",
            i,armour,mass>>4,mass&15,flags,value,unknown1,scale,unknown2,
            gs.get_string(4,i).c_str() );

         pos = ftell(fd) - pos;
      }

      fclose(fd);
      fclose(out);
   }

   // decoding objects.dat
   {
      FILE *fd = fopen(UWPATH"data\\objects.dat","rb");
      FILE *out = fopen("uw-object-classes.txt","w");

      fseek(fd,0x02,SEEK_SET);
      fprintf(out,"melee weapons table\n\n");

      {
         fprintf(out,"type can be: 03: sword, 04: axe, 05: mace, 06: unarmed\n");
         unsigned char buffer[8];
         for(int i=0; i<0x10; i++)
         {
            fread(buffer,1,8,fd);

            fprintf(out,"%04x: slash=%02x bash=%02x stab=%02x type=%02x durability=%02x "
               "unknowns=%02x %02x %02x name=%s\n",
               i,buffer[0],buffer[1],buffer[2],buffer[6],buffer[7],
               buffer[3],buffer[4],buffer[5],gs.get_string(4,i).c_str() );
         }
         fprintf(out,"\n");
      }

      fseek(fd,0x82,SEEK_SET);
      fprintf(out,"ranged weapons table\n\n");

      {
         unsigned char buffer[3];
         for(int i=0x10; i<0x20; i++)
         {
            fread(buffer,1,3,fd);

            fprintf(out,"%04x: durability=%02x unknown1=%02x unknown2=%02x name=%s\n",
               i,buffer[0],buffer[1],buffer[2],gs.get_string(4,i).c_str());
         }
         fprintf(out,"\n");
      }

      fseek(fd,0xb2,SEEK_SET);
      fprintf(out,"armour and wearables table\n\n");

      {
         fprintf(out,"category can be: 00: shield, 01: body armour, 03: leggings, 04: gloves, 05: boots, 08: hat, 09: ring\n");
         unsigned char buffer[4];
         for(int i=0x20; i<0x40; i++)
         {
            fread(buffer,1,4,fd);

            fprintf(out,"%04x: protection=%02x durability=%02x unknown1=%02x category=%02x name=%s\n",
               i,buffer[0],buffer[1],buffer[2],buffer[3],gs.get_string(4,i).c_str());
         }
         fprintf(out,"\n");
      }

      fseek(fd,0x0132,SEEK_SET);
      fprintf(out,"critters table\n\n");

      {
         for(int i=0x40; i<0x80; i++)
         {
            fprintf(out,"%02x:",i);
            for(int j=0; j<48; j++)
            {
               fprintf(out," %02x",fgetc(fd));
               // if (j==23) fprintf(out,"\n   ",i);
            }
            fprintf(out," name=%s\n",gs.get_string(4,i).c_str());
         }
         fprintf(out,"\n");
      }

      fseek(fd,0x0d32,SEEK_SET);
      fprintf(out,"containers table\n\n");

      {
         unsigned char buffer[3];
         for(int i=0x80; i<0x90; i++)
         {
            fread(buffer,1,3,fd);

            fprintf(out,"%04x: capacity=%02x accept=%02x unknown=%02x name=%s\n",
               i,buffer[0],buffer[1],buffer[2],gs.get_string(4,i).c_str());
         }
         fprintf(out,"\n");
      }

      fseek(fd,0x0d62,SEEK_SET);
      fprintf(out,"lighting and food info table\n\n");

      {
         for(int i=0x90; i<0xa0; i++)
         {
            fprintf(out,"%02x: brightness=%02x duration=%02x name=%s\n",
               i,fgetc(fd),fgetc(fd),gs.get_string(4,i).c_str());
         }
         fprintf(out,"\n");
      }

      fseek(fd,0x0da2,SEEK_SET);
      fprintf(out,"animation object table\n\n");

      {
         unsigned char buffer[4];
         for(int i=0x01c0; i<0x01d0; i++)
         {
            fread(buffer,1,4,fd);

            fprintf(out,"%04x: startframe=%02x numframes=%02x unknown1=%02x unknown2=%02x "
               "name=%s\n",
               i,buffer[1],buffer[2],buffer[0],buffer[3],
               gs.get_string(4,i).c_str());
         }
      }

      fclose(fd);
      fclose(out);
   }

   return 0;
}
