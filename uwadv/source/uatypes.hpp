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
/*! \file uatypes.hpp

   commonly used typedefs

*/

// include guard
#ifndef __uwadv_uatypes_hpp_
#define __uwadv_uatypes_hpp_

// needed includes
#include <string>
#include <exception>


//! exception class

class ua_exception: public std::exception
{
public:
   //! ctor
   ua_exception(const char *desc){ description.assign(desc); };

   //! returns exception description
   virtual const char *what() const throw()
   {
      return description.c_str();
   }

protected:
   std::string description;
};

#endif
