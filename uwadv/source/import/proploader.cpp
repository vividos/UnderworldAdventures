/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002,2003 Underworld Adventures Team

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
/*! \file proploader.cpp

   \brief object property loader

*/

// needed includes
#include "common.hpp"
#include "import.hpp"
#include "properties.hpp"


// ua_object_properties methods

void ua_object_properties::import(ua_settings& settings)
{
   ua_uw_import import;
   import.load_properties(*this,
      settings.get_string(ua_setting_uw_path).c_str());
}


// ua_uw_import methods

void ua_uw_import::load_properties(ua_object_properties& prop,
   const char* path)
{
   ua_trace("loading object properties\n");

   // import common object properties
   {
      std::string common_name(path);
      common_name.append("data/comobj.dat");

      FILE* fd = fopen(common_name.c_str(),"rb");

      if (fd==NULL)
         throw ua_exception("could not open data/comobj.dat");

      prop.common_properties.clear();

      fseek(fd,0L,SEEK_END);
      long flen = ftell(fd);
      fseek(fd,2,SEEK_SET);

      unsigned int max = (flen-2)/11;
      for(unsigned int i=0; i<max; i++)
      {
         Uint8 data[11];
         for(unsigned int j=0; j<11; j++)
            data[j] = fgetc(fd);

         ua_common_obj_property comprop;

         comprop.radius = data[1] & 7;
         comprop.height = data[0];
         comprop.mass = ((data[2]<<8) | data[1]) >> 4;
         comprop.quality_class = (data[6] >> 2) & 3;
         comprop.quality_type = data[10] & 15;
         comprop.can_have_owner = (data[7] & 0x80) != 0;
         comprop.can_be_looked_at = (data[10] & 0x10) != 0;
         comprop.can_be_picked_up = ((data[3]>>5) & 1) != 0;

         prop.common_properties.push_back(comprop);
      }

      fclose(fd);
   }

   // import object properties
   {
   }
}
