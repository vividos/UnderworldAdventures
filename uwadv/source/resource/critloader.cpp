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
/*! \file critloader.cpp

   \brief critter images loading implementation

   for more infos about critters, see file docs/uw-formats.txt

*/

// needed includes
#include "common.hpp"
#include "critter.hpp"


// external functions

extern void ua_image_decode_rle(FILE *fd,std::vector<Uint8> &pixels,unsigned int bits,
   unsigned int datalen,unsigned int maxpix,unsigned char *auxpalidx);


// ua_critter methods

void ua_critter::load(const char* file, Uint8 auxpal)
{
   unsigned int curpage = 0;
   while(true)
   {
      // construct next pagefile name
      std::string pagefile(file);

      char buffer[8];
      sprintf(buffer,".n%02u",curpage++);
      pagefile.append(buffer);

      // try to open pagefile
      FILE* fd = fopen(pagefile.c_str(),"rb");
      if (fd==NULL)
         break; // no more page files

      // todo: load animation


      // close pagefile
      fclose(fd);
   }
}

void ua_critter_pool::load(ua_settings& settings)
{
   ua_trace("loading all critter animations\n");

   // load infos from "assoc.anm"
   char animnames[32][8];

   // open assoc.anm file
   std::string assocname(settings.get_string(ua_setting_uw_path));
   assocname.append("crit/assoc.anm");

   FILE* assoc = fopen(assocname.c_str(),"rb");
   if (assoc==NULL)
      throw ua_exception("could not find assoc.anm");

   // get animation names
   fread(animnames,32,8,assoc);

   // read in critter anim and auxpal values
   for(int i=0; i<64; i++)
   {
      // get anim and auxpal values
      Uint8 anim = fgetc(assoc);
      Uint8 auxpal = fgetc(assoc);

      ua_critter crit;

      // construct critter filename base
      if (animnames[anim][0]!=0)
      {
         std::string critfile(settings.get_string(ua_setting_uw_path));

         char buffer[16];
         sprintf(buffer,"crit/cr%02opage",anim); // yeah, octal!
         critfile.append(buffer);

         // load it
         crit.load(critfile.c_str(),auxpal);
      }

      // add to rest of the critters
      allcritters.push_back(crit);
   }

   fclose(assoc);
}
