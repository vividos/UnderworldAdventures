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
/*! \file playerinfo.hpp

   \brief player info list control

*/

// include guard
#ifndef uadebug_playerinfo_hpp_
#define uadebug_playerinfo_hpp_

// needed includes
#include "wx/listctrl.h" // list control


//! player info list control
class ua_playerinfo_list: public wxListCtrl
{
public:
   //! ctor
   ua_playerinfo_list(wxWindow *parent, const wxWindowID id, const wxPoint& pos,
      const wxSize& size, long style);

   //! updates data from underworld
   void UpdateData();

   //! adds bar to frame layout
   void AddBar(wxFrameLayout* pLayout);

   //! frame name for "player info" control
   static const char* frame_name;

protected:
   // event table
   DECLARE_EVENT_TABLE()
};

#endif
