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

   \brief conv code loading implementation

   ua_conv_code_vm::load_code() loads the code into the virtual machine,
   processing all globals and imported functions. it also determines which
   string block to use, by counting valid conv slots.

   for more infos about conv code, see file docs/uw-formats.txt

*/

// needed includes
#include "common.hpp"
#include "../conv/codevm.hpp"
#include "../import/io_endian.hpp"


// ua_conv_code_vm methods

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

      // determine return type
      if (ret_type == 0x0000) iitem.ret_type = ua_rt_void;
      else
      if (ret_type == 0x0129) iitem.ret_type = ua_rt_int;
      else
      if (ret_type == 0x012b) iitem.ret_type = ua_rt_string;
      else
         throw ua_exception("unknown return type in conv imports list");

      iitem.name = funcname;

      // store imported item in appropriate list
      if (import_type == 0x0111)
      {
         // imported function
         imported_funcs[func_id] = iitem;
      }
      else
      if (import_type == 0x010F)
      {
         // imported global
         imported_globals[func_id] = iitem;
      }
      else
         throw ua_exception("unknown import type in conv imports list");
   }
}


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

   // read conversation header
   fseek(fd,offset,SEEK_SET);

   Uint32 unk1 = fread32(fd); // always 0x0828

   // as there are only 16-bit operands for e.g. JUMP, 32-bit code doesn't
   // make sense
   codesize = static_cast<Uint16>(fread32(fd));
   Uint16 unk2 = fread16(fd); // always 0x0000
   strblock = fread16(fd); // string block to use

   // seems to be superfluous, since we know the number of words from the globals file
   glob_reserved = fread16(fd); // number of stack words reserved for globals

   // load imported functions
   load_imported_funcs(fd);

   // load code
   code.resize(codesize,0);
   for(Uint16 i=0; i<codesize; i++)
      code[i] = fread16(fd);

   fclose(fd);

   return true;
}
