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
/*! \file cfgfile.cpp

   \brief general config file handling

*/

// needed includes
#include "common.hpp"
#include "cfgfile.hpp"


// ua_cfgfile methods

ua_cfgfile::ua_cfgfile()
:is_writing(false),newfp(NULL)
{
}

void ua_cfgfile::load(const char *filename)
{
   // try to open from file
   SDL_RWops* rwops = SDL_RWFromFile(filename,"rb");
   if (rwops==NULL)
   {
      std::string text("could not open config file ");
      text.append(filename);
      throw ua_exception(text.c_str());
   }

   load(rwops);
}

void ua_cfgfile::load(SDL_RWops* rwops)
{
   // find out filelength
   SDL_RWseek(rwops,0L,SEEK_END);
   long filelen = SDL_RWtell(rwops);
   SDL_RWseek(rwops,0L,SEEK_SET);

   // read in all lines
   std::string line;

   while(SDL_RWtell(rwops)<filelen)
   {
      // read in one line
      line.erase();
      {
         char c=0;
         do
         {
            // read next char
            SDL_RWread(rwops,&c,1,1);
            if (c=='\r')
            {
               // carriage return? reread
               SDL_RWread(rwops,&c,1,1);
            }

            // append char
            if (c!='\n')
               line.append(1,c);
         }
         while(c!='\n');
      }

      // empty line?
      if (line.size()==0)
      {
         if (is_writing) write_raw_line(line);
         continue;
      }

      // trim spaces at start of line
      for(;line.size()>0 && isspace(line.at(0));)
         line.erase(0,1);

      // comment line?
      if (line.size()==0 || line.at(0)=='#')
      {
         if (is_writing) write_raw_line(line);
         continue;
      }

      // check for section start
      if (line.at(0)=='[' && line.size()>2)
      {
         std::string section(line.c_str()+1,line.size()-2);

         if (is_writing)
         {
            write_start_section(section);
            write_raw_line(line);
         }
         else
            load_start_section(section);

         continue;
      }

      // trim spaces at end of line
      {
         int len;
         do
         {
            len = line.size()-1;
            if (isspace(line.at(len)))
               line.erase(len);
            else
               break;
         }
         while(line.size()>0);
      }

      // empty line?
      if (line.size()==0)
      {
         if (is_writing) write_raw_line(line);
         continue;
      }

      // replace all '\t' with ' '
      std::string::size_type pos = 0;
      while( (pos = line.find('\t',pos) != std::string::npos) )
         line.replace(pos,1," ");

      // there must be at least one space, to separate key from value
      pos = line.find(' ');
      if (pos==std::string::npos)
      {
         if (is_writing) write_raw_line(line);
         continue;
      }

      // retrieve key and value
      {
         std::string key,value;
         key.assign(line.c_str(),pos);
         value.assign(line.c_str()+pos+1);

         // trim spaces at start of "value"
         for(;value.size()>0 && isspace(value.at(0));)
            value.erase(0,1);

         // hand over key and value
         if (is_writing)
         {
            write_replace(key,value);
            key.append(1,' ');
            key.append(value);
            write_raw_line(key);
         }
         else
            load_value(key,value);
      }
   }
}

void ua_cfgfile::write(const char *origfile, const char *newfile)
{
   // open the new file
   newfp = fopen(newfile,"wt");
   if (newfp==NULL)
   {
      std::string text("could not rewrite new config file ");
      text.append(newfile);
      throw ua_exception(text.c_str());
   }

   // load original file and rewrite new file
   is_writing = true;
   load(origfile);
   is_writing = false;

   fclose(newfp);
   newfp = NULL;
}

void ua_cfgfile::write_raw_line(const std::string& line)
{
   fputs(line.c_str(),newfp);
}
