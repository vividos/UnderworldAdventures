/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002 Michael Fink

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

   $Id$

*/
/*! \file cutsloader.cpp

   \brief cutscenes loader

   for more infos about the cutscene format, see the file docs/uw-formats.txt

*/

// needed includes
#include "common.hpp"
#include "cutscene.hpp"
#include "../import/io_endian.hpp"


// ua_cutscene methods

void ua_cutscene::load(ua_settings &settings, unsigned int main,
   unsigned int sub)
{
   char buffer[32];
   sprintf(buffer,"cuts/cs%03o.n%02o",main,sub);

   load(settings,buffer);
}

void ua_cutscene::load(ua_settings &settings, const char *relfilename)
{
   std::string filename(settings.get_string(ua_setting_uw_path));
   filename.append(relfilename);

   load(filename.c_str());
}

void ua_cutscene::load(const char *filename)
{
   FILE *fd = fopen(filename,"rb");
   if (fd==NULL)
   {
      std::string text("could not open file ");
      text.append(filename);
      throw ua_exception(text.c_str());
   }

   // get file length
   fseek(fd,0,SEEK_END);
   long filesize = ftell(fd);
   fseek(fd,0,SEEK_SET);

   // read file id
   char sign[4];
   fread(sign,1,4,fd);

   if (0!=strncmp(sign,"LPF ",4))
      throw ua_exception("wrong cutscene file type");

   // number of large pages and records
   fseek(fd,6,SEEK_SET);
   largepages = fread16(fd);
   records = fread32(fd);

   fseek(fd,0x10,SEEK_SET);
   fread(sign,1,4,fd);
   if (0!=strncmp(sign,"ANIM",4))
      throw ua_exception("wrong cutscene file type");

   // width and height
   xres = fread16(fd);
   yres = fread16(fd);
   create(xres,yres,0,0);

   // skip color cycling infos
   fseek(fd,0x0100,SEEK_SET);

   ua_onepalette& anim_palette = quadpalette;

   // read in palette
   for(int n=0; n<256; n++)
   {
      anim_palette[n][2] = fgetc(fd); // blue
      anim_palette[n][1] = fgetc(fd); // green
      anim_palette[n][0] = fgetc(fd); // red
      fgetc(fd); // extra pad byte
      anim_palette[n][3] = 0xff; // transparency
   }

   // now we are at the large page descriptor array
   lpdarray.resize(largepages);

   for(unsigned int i=0; i<largepages; i++)
   {
      lpdarray[i].base = fread16(fd);    // base record
      lpdarray[i].records = fread16(fd); // number of records
      lpdarray[i].bytes = fread16(fd);   // bytes in large page
   }

   // seek to start of the real large pages
   fseek(fd,0x0100 + 256*4 + 6*256,SEEK_SET);

   int pos = ftell(fd);

   // read in all large pages
   unsigned int lpsize = filesize-ftell(fd);
   lpages.resize((lpsize|0xffff)+1,0); // fill up to whole 64k page
   fread(&lpages[0],lpsize,1,fd);

   fclose(fd);

   curframe = (unsigned int)-1;
}
