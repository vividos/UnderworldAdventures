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
/*! \file cfgfile.hpp

   \brief general config file handling

   note: after calling calling load(SDL_RWops*) the file is not closed using
   SDL_RWclose()

*/

// include guard
#ifndef __uwadv_cfgfile_hpp_
#define __uwadv_cfgfile_hpp_

// needed includes


// classes

//! config file class
class ua_cfgfile
{
public:
   //! ctor
   ua_cfgfile();

   //! dtor
   virtual ~ua_cfgfile(){}


   // config file loading

   //! loads a filename
   void load(const char *filename);

   //! loads a config file from SDL_RWops
   void load(SDL_RWops* file);

   //! called when a new "[section]" starts
   virtual void load_start_section(const std::string& secname){}

   //! called to load a specific value
   virtual void load_value(const std::string& name, const std::string& value){}


   // config file (re)writing

   //! creates a new config file using the original file as template
   void write(const char *origfile, const char *newfile);

   //! called when a new "[section]" starts
   virtual void write_start_section(const std::string& secname);

   //! called to replace a value
   virtual void write_replace(const std::string& name, std::string& value){}

protected:
   //! writes a raw line (that didn't contain a key/value pair) to the new file
   void write_raw_line(const std::string& line);

protected:
   //! indicates if write() is currently called
   bool is_writing;

   //! new file, used by write()
   FILE* newfp;
};

#endif
