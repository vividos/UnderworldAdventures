/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002,2003,2004 Underworld Adventures Team

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
/*! \file convloader.cpp

   \brief conversation code and conv. globals loader

*/

// needed includes
#include "common.hpp"
#include "import.hpp"
#include "io_endian.hpp"
#include "codevm.hpp"


//! Loads conversation globals from bglobals.dat or babglobs.dat
/*! When initial is set to true, the file to load only contains size entries,
    and no actual data; all globals are initalized with 0 then.
*/
void ua_uw_import::load_conv_globals(ua_conv_globals& globals,
   ua_settings& settings, const char* folder, bool initial)
{
   std::string bgname(settings.get_string(ua_setting_uw_path));
   bgname.append(folder);
   bgname.append(initial ? "babglobs.dat" : "bglobals.dat");

   // try to open file
   FILE *fd = fopen(bgname.c_str(),"rb");
   if (fd==NULL)
   {
      std::string text("error loading conv. globals file: ");
      text.append(bgname);
      throw ua_exception(text.c_str());
   }

   std::vector< std::vector<Uint16> >& allglobals = globals.get_allglobals();

   // read in all slot/size/[globals] infos
   while (!feof(fd))
   {
      Uint16 slot = fread16(fd);
      Uint16 size = fread16(fd);

      std::vector<Uint16> globals;
      globals.resize(size,0);

      if (!initial)
      {
         // read in globals
         for(Uint16 i=0; i<size; i++)
            globals.push_back(fread16(fd));
      }

      // put globals in allglobals list
      if (slot>allglobals.size())
         allglobals.resize(slot);

      allglobals.insert(allglobals.begin()+slot,globals);
   }

   fclose(fd);
}


// ua_conv_vm methods

/*! Loads conversation code from given file, into the virtual machine.
    It loads all imported symbols (functions and globals) and puts the opcodes
    into the vm's code segment. The method ua_conv_code_vm::init() can then be
    called after this method.

    \param vm conversation code virtual machine
    \param cnvfile conversation filename, e.g. cnv.ark
    \param conv convesation slot of code to load
*/
bool ua_uw_import::load_conv_code(ua_conv_code_vm& vm, const char* cnvfile,
   Uint16 conv)
{
   FILE *fd = fopen(cnvfile,"rb");
   if (fd==NULL)
      throw ua_exception("could not open conversation file");

   // read number of entries
   Uint16 entries = fread16(fd);

   if (conv>=entries)
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

   Uint32 unk1 = fread32(fd); // always 0x00000828

   Uint16 codesize = fread16(fd);
   Uint16 unk2 = fread16(fd); // always 0x0000
   Uint16 unk3 = fread16(fd); // always 0x0000

   Uint16 strblock = fread16(fd); // string block to use
   vm.set_strblock(strblock);

   Uint16 glob_reserved = fread16(fd); // number of stack words reserved for globals
   vm.set_globals_reserved(glob_reserved);

   // load imported functions
   load_conv_code_imported_funcs(vm, fd);

   // load code
   std::vector<Uint16>& code = vm.get_code_segment();

   code.resize(codesize,0);
   for(Uint16 i=0; i<codesize; i++)
      code[i] = fread16(fd);

   fclose(fd);

   // fix for Marrowsuck conversation; it has a wrong opcode on 0x076e
   if (conv == 6)
   {
      code[0x076e] = 0x0016; //op_PUSHI;
   }

   // fix for Sseetharee conversation; call to function at 0xffff
   if (conv == 15)
   {
      // just take another function that may seem reasonable
      code[0x0584] = 0x0666;
   }

   // fix for Judy conversation; argument to random is 2, but switch statement
   // checks for 3 values; the 3rd answer will never be given
   if (conv == 23)
   {
      // PUSHI #3
      code[0x04fd] = 3;
   }

   vm.set_conv_slot(conv);

   return true;
}

void ua_uw_import::load_conv_code_imported_funcs(ua_conv_code_vm& vm,
   FILE *fd)
{
   std::map<Uint16,ua_conv_imported_item>& imported_funcs = vm.get_imported_funcs();
   std::map<Uint16,ua_conv_imported_item>& imported_globals = vm.get_imported_globals();

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
      switch(ret_type)
      {
      case 0x0000: iitem.datatype = ua_dt_void; break;
      case 0x0129: iitem.datatype = ua_dt_int; break;
      case 0x012b: iitem.datatype = ua_dt_string; break;
      default:
         throw ua_exception("unknown return type in conv imports list");
      }

      iitem.name = funcname;

      // store imported item in appropriate list
      switch(import_type)
      {
      case 0x0111:
         // imported function
         imported_funcs[func_id] = iitem;
         break;

      case 0x010F:
         // imported global
         imported_globals[func_id] = iitem;
         break;

      default:
         throw ua_exception("unknown import type in conv imports list");
         break;
      }
   }
}
