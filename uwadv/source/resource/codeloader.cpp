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
/*! \file codeloader.cpp

   conv code loading implementation

*/

// needed includes
#include "common.hpp"
#include "../conv/codevm.hpp"
#include "fread_endian.hpp"


// ua_conv_vm methods

bool ua_conv_code_vm::load_code(const char *cnvfile, Uint16 conv)
{
   conv_nr = conv;

   FILE *fd = fopen(cnvfile,"rb");
   if (fd==NULL)
      throw ua_exception("could not open conversation file");

   // read number of entries
   Uint16 entries = fread16(fd);

   if (conv>entries)
   {
      fclose(fd);
      throw ua_exception("invalid conversation!");
   }

   fseek(fd,conv*4+2,SEEK_SET);

   Uint32 offset = fread32(fd);

   if (offset==0)
   {
      // no conversation available
      fclose(fd);
      return false;
   }

   // count valid conv slots up to the one to load
   unsigned int slotcount=0;
   fseek(fd,2,SEEK_SET);

   for(Uint16 n=0; n<conv; n++)
   {
      Uint32 slotoffset = fread32(fd);
      if (slotoffset!=0)
         slotcount++;
   }

   strblock = slotcount+25;

   // read conversation header
   fseek(fd,offset,SEEK_SET);

   Uint32 unknown1 = fread32(fd);
   Uint32 thecodesize = fread32(fd);

   Uint16 fn_unknown1 = fread16(fd);
   Uint16 fn_unknown2 = fread16(fd);
   Uint16 fn_unknown3 = fread16(fd);

   // load imported functions
   load_imported_funcs(fd);

   // load code
   reserve(thecodesize);
   for(int i=0; i<codesize; i++)
      code[i] = fread16(fd);

   return true;
}

void ua_conv_code_vm::load_imported_funcs(FILE *fd)
{
   imported_funcs.clear();
   imported_globals.clear();

   // read number of imported funcs
   Uint16 funcs = fread16(fd);

   for(int i=0; i<funcs; i++)
   {
      // length of function name
      Uint16 fname_len = fread16(fd);
      if (fname_len>255) fname_len = 255;

      char funcname[256];
      fread(funcname,1,fname_len,fd);
      funcname[fname_len]=0;

      // function ID
      Uint16 func_id = fread16(fd);
      Uint16 fn_unknown1 = fread16(fd); // always seems to be 1
      Uint16 import_type = fread16(fd);
      Uint16 ret_type = fread16(fd);

      // fill imported item struct
      ua_conv_imported_item iitem;

      if (ret_type == 0x0000) iitem.ret_type = ua_rt_void;
      else
      if (ret_type == 0x0129) iitem.ret_type = ua_rt_int;
      else
      if (ret_type == 0x012b) iitem.ret_type = ua_rt_string;
      else
         throw ua_ex_error_loading;

      iitem.name = funcname;

      // store imported item in appropriate list
      if (import_type == 0x0111)
      {
         if (imported_funcs.capacity()<func_id)
            imported_funcs.resize(func_id+1);

         imported_funcs[func_id] = iitem;
      }
      else
      if (import_type == 0x010F)
      {
         if (imported_globals.capacity()<func_id)
            imported_globals.resize(func_id+1);

         imported_globals[func_id] = iitem;
      }
      else
         throw ua_ex_error_loading;
   }
}
