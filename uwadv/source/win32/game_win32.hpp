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
/*! \file game_win32.hpp

   \brief win32 specific game class

   overrides some virtual functions to implement win32 specific stuff, e.g.
   icon loading.

*/
//! \ingroup game

//@{

// include guard
#ifndef uwadv_game_win32_hpp_
#define uwadv_game_win32_hpp_

// needed includes
#include "uwadv.hpp"


// win32 specific game class

class ua_uwadv_game_win32: public ua_uwadv_game
{
public:
   //! ctor
   ua_uwadv_game_win32(){}
   //! dtor
   virtual ~ua_uwadv_game_win32(){}

   // customized virtual methods
   virtual void init();
   virtual void done();
   virtual void error_msg(const char* msg);

protected:
   // application icon
   HICON icon;
   HICON icon_small;
};


#endif
//@}
