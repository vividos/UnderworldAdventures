/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2003 Underworld Adventures Team

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
/*! \file import.hpp

   \brief game data import

   aaa

*/

// include guard
#ifndef uwadv_import_hpp_
#define uwadv_import_hpp_

// needed includes


// forward references
class ua_font;


//! imports common to uw1 and uw2
class ua_uw_import
{
public:


   //! loads a font
   void load_font(const char* fontname, ua_font& font);

protected:
};

//! uw1 imports
class ua_uw1_import: public ua_uw_import
{
public:

};

//! uw2 imports
class ua_uw2_import
{
public:

};

#endif
