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
/*! \file hotspotlist.hpp

   \brief hotspot list

*/

// include guard
#ifndef uadebug_hotspotlist_hpp_
#define uadebug_hotspotlist_hpp_

// needed includes
#include "wx/listctrl.h"


// classes

//! hotspot list window
class ua_hotspotlist_frame: public wxMDIChildFrame
{
public:
   //! ctor
   ua_hotspotlist_frame(/*wxDocManager* doc_manager, */
      wxMDIParentFrame* parent, wxWindowID id,
      const wxPoint& pos, const wxSize& size, long style);

   //! hotspot frame name
   static const char* frame_name;

protected:
   // message handler

   //! called when list item is activated (either doubleclick or enter key)
   void OnListItemActivated(wxListEvent& event);

protected:
   //! list with hotspot entries
   wxListCtrl* hotspotlist;

   DECLARE_EVENT_TABLE()
};

#endif